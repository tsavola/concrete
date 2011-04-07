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

#include <concrete/block.hpp>
#include <concrete/context.hpp>
#include <concrete/execute.hpp>
#include <concrete/io/buffer.hpp>
#include <concrete/io/resolve.hpp>
#include <concrete/io/socket.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/long.hpp>
#include <concrete/objects/module.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/resource.hpp>

namespace concrete {

class ForkState: public Block {
public:
	ForkState() throw ():
		forked(false)
	{
	}

	~ForkState() throw ()
	{
		reset();
	}

	void reset() throw ()
	{
		if (resolve_id) {
			Context::DeleteResource(resolve_id);
			resolve_id = NULL;
		}

		if (socket_id) {
			Context::DeleteResource(socket_id);
			socket_id = NULL;
		}

		if (buffer_id) {
			Context::DeleteResource(buffer_id);
			buffer_id = NULL;
		}
	}

	bool lost() const throw ()
	{
		return Context::ResourceLost(resolve_id) ||
		       Context::ResourceLost(socket_id) ||
		       Context::ResourceLost(buffer_id);
	}

	PortableObject host;
	PortableObject port;
	PortableResourceId resolve_id;
	PortableResourceId socket_id;
	PortableResourceId buffer_id;
	Portable<uint8_t> mode;
	Portable<uint8_t> forked;

} CONCRETE_PACKED;

class Fork: public Continuable {
	enum Mode {
		ResolveMode,
		ConnectMode,
		WriteMode,
	};

public:
	bool call(Object &result, const TupleObject &args, const DictObject &kwargs) const
	{
		auto address = args.get_item(0).require<TupleObject>();
		auto host = address.get_item(0).require<StringObject>();
		auto port = address.get_item(1).require<LongObject>().str();

		state()->host = host;
		state()->port = port;

		init_resolve();
		return false;
	}

	bool resume(Object &result) const
	{
		if (state()->forked)
			return true;

		if (state()->lost()) {
			state()->reset();
			init_resolve();
			return false;
		}

		switch (state()->mode) {
		case ResolveMode:
			resume_resolve();
			return false;

		case ConnectMode:
			resume_connect();
			return false;

		case WriteMode:
			return resume_write(result);

		default:
			throw IntegrityError(state());
		}
	}

private:
	void init_resolve() const
	{
		ConcreteTrace(("fork: init resolve"));

		auto resolve = Context::NewResource<ResolveResource>(
			state()->host.cast<StringObject>().string(),
			state()->port.cast<StringObject>().string());

		state()->resolve_id = resolve.id;
		state()->mode = ResolveMode;

		resolve.resource.wait_addrinfo();
	}

	void resume_resolve() const
	{
		ConcreteTrace(("fork: resume resolve"));

		auto &resolve = Context::Resource<ResolveResource>(state()->resolve_id);

		auto addrinfo = resolve.addrinfo();
		if (addrinfo)
			init_connect(addrinfo);
		else
			resolve.wait_addrinfo();
	}

	void init_connect(const struct addrinfo *addrinfo) const
	{
		ConcreteTrace(("fork: init connect"));

		int family;
		socklen_t addrlen = 0;
		struct sockaddr_storage addr;

		for (auto i = addrinfo; i; i = i->ai_next) {
			if (i->ai_socktype == SOCK_STREAM) {
				family = i->ai_family;
				addrlen = i->ai_addrlen;
				std::memcpy(&addr, i->ai_addr, addrlen);

				break;
			}
		}

		if (addrlen == 0)
			throw ResourceError();

		auto socket = Context::NewResource<SocketResource>(family, SOCK_STREAM);

		state()->socket_id = socket.id;
		state()->mode = ConnectMode;

		socket.resource.wait_connection(
			reinterpret_cast<struct sockaddr *> (&addr),
			addrlen);
	}

	void resume_connect() const
	{
		ConcreteTrace(("fork: resume connect"));

		auto &socket = Context::Resource<SocketResource>(state()->socket_id);

		if (socket.connected())
			init_write();
		else
			socket.wait_connection();
	}

	void init_write() const
	{
		ConcreteTrace(("fork: init write"));

		auto context = Context::Active().snapshot();
		auto executor = Executor::Active().snapshot();

		auto buffer = Context::NewResource<BufferResource>(
			context.head_size() + context.data_size() + executor.size());

		char *data = buffer.resource.data();
		size_t offset = 0;

		state()->forked = true;

		std::memcpy(data + offset, context.head_ptr(), context.head_size());
		offset += context.head_size();
		std::memcpy(data + offset, context.data_ptr(), context.data_size());
		offset += context.data_size();
		std::memcpy(data + offset, executor.ptr(), executor.size());

		state()->forked = false;

		state()->buffer_id = buffer.id;
		state()->mode = WriteMode;

		auto &socket = Context::Resource<SocketResource>(state()->socket_id);

		socket.wait_writability();
	}

	bool resume_write(Object &result) const
	{
		ConcreteTrace(("fork: resume write"));

		auto &socket = Context::Resource<SocketResource>(state()->socket_id);
		auto &buffer = Context::Resource<BufferResource>(state()->buffer_id);

		if (!buffer.write(socket))
			throw ResourceError();

		if (buffer.remaining()) {
			socket.wait_writability();
			return false;
		} else {
			result = LongObject::New(1);
			return true;
		}
	}

	ForkState *state() const
	{
		return state_pointer<ForkState>();
	}
};

} // namespace

CONCRETE_INTERNAL_CONTINUABLE(ConcreteModule_Fork, Fork, ForkState)
