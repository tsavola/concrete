/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <cstring>

#include <sys/socket.h>
#include <netdb.h>

#include <concrete/context.hpp>
#include <concrete/continuation.hpp>
#include <concrete/execution.hpp>
#include <concrete/io/buffer.hpp>
#include <concrete/io/resolve.hpp>
#include <concrete/io/socket.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/long.hpp>
#include <concrete/objects/module.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/portable.hpp>
#include <concrete/resource.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/trace.hpp>

namespace concrete {

class Fork: public Continuation {
	friend class Pointer;

protected:
	enum {
		ResolveMode,
		ConnectMode,
		WriteMode,
	};

	struct Data: Noncopyable {
		~Data() throw ()
		{
			reset();
		}

		void reset() throw ()
		{
			resolve.destroy();
			socket.destroy();
			buffer.destroy();
		}

		bool is_lost() const throw ()
		{
			return resolve.is_lost() || socket.is_lost() || buffer.is_lost();
		}

		Portable<Object>                  host;
		Portable<Object>                  port;
		PortableResource<ResolveResource> resolve;
		PortableResource<SocketResource>  socket;
		PortableResource<BufferResource>  buffer;
		Portable<uint8_t>                 mode;
		Portable<bool>                    forked;
	} CONCRETE_PACKED;

	explicit Fork(unsigned int address) throw ():
		Continuation(address)
	{
	}

public:
	bool initiate(Object &result, const TupleObject &args, const DictObject &kwargs) const
	{
		auto address = args.get_item(0).require<TupleObject>();
		auto host = address.get_item(0).require<StringObject>();
		auto port = address.get_item(1).require<LongObject>().str();

		data()->host = host;
		data()->port = port;

		initiate_resolve();
		return false;
	}

	bool resume(Object &result) const
	{
		if (data()->forked)
			return true;

		if (data()->is_lost()) {
			data()->reset();
			initiate_resolve();
			return false;
		}

		switch (data()->mode) {
		case ResolveMode:
			resume_resolve();
			return false;

		case ConnectMode:
			resume_connect();
			return false;

		case WriteMode:
			return resume_write(result);

		default:
			throw IntegrityError(address());
		}
	}

private:
	void initiate_resolve() const
	{
		Trace("fork: initiate resolve");

		auto host = data()->host->cast<StringObject>().string();
		auto port = data()->port->cast<StringObject>().string();

		data()->resolve.create(host, port);
		data()->mode = ResolveMode;

		data()->resolve->wait_addrinfo();
	}

	void resume_resolve() const
	{
		Trace("fork: resume resolve");

		auto addrinfo = data()->resolve->addrinfo();
		if (addrinfo)
			initiate_connect(addrinfo);
		else
			data()->resolve->wait_addrinfo();
	}

	void initiate_connect(const struct addrinfo *addrinfo) const
	{
		Trace("fork: initiate connect");

		int family;
		socklen_t addrlen = 0;
		struct sockaddr_storage addr;

		for (auto i = addrinfo; i; i = i->ai_next)
			if (i->ai_socktype == SOCK_STREAM) {
				family = i->ai_family;
				addrlen = i->ai_addrlen;
				std::memcpy(&addr, i->ai_addr, addrlen);

				break;
			}

		if (addrlen == 0)
			throw ResourceError();

		data()->socket.create(family, SOCK_STREAM);
		data()->mode = ConnectMode;

		data()->socket->wait_connection(reinterpret_cast<struct sockaddr *> (&addr), addrlen);
	}

	void resume_connect() const
	{
		Trace("fork: resume connect");

		if (data()->socket->connected())
			initiate_write();
		else
			data()->socket->wait_connection();
	}

	void initiate_write() const
	{
		Trace("fork: initiate write");

		data()->buffer.create();
		data()->mode = WriteMode;

		auto snapshot = Arena::Active().snapshot();

		auto buffer = *data()->buffer;
		buffer->reset(snapshot.size);

		data()->forked = true;
		std::memcpy(buffer->data(), snapshot.base, snapshot.size);
		data()->forked = false;

		data()->socket->wait_writability();
	}

	bool resume_write(Object &result) const
	{
		Trace("fork: resume write");

		auto socket = *data()->socket;
		auto buffer = *data()->buffer;

		if (!buffer->write(*socket))
			throw ResourceError();

		if (buffer->remaining()) {
			socket->wait_writability();
			return false;
		} else {
			result = LongObject::New(1);
			return true;
		}
	}

	Data *data() const
	{
		return data_cast<Data>();
	}
};

} // namespace

CONCRETE_INTERNAL_CONTINUATION(ConcreteModule_Fork, Fork)
