/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <cstring>

#include <concrete/context.hpp>
#include <concrete/continuation.hpp>
#include <concrete/execution.hpp>
#include <concrete/io/buffer.hpp>
#include <concrete/io/http.hpp>
#include <concrete/objects/bool.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/module.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/portable.hpp>
#include <concrete/resource.hpp>
#include <concrete/util/assert.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/trace.hpp>

namespace concrete {

class Fork: public Continuation {
	friend class Pointer;

private:
	enum State { Connecting, Sending, ReceivingHeaders, ReceivingContent, Done };
	enum { ResponseBufferSize = 4096 };

protected:
	struct Data: Noncopyable {
		~Data() throw ()
		{
			reset();
		}

		void reset() throw ()
		{
			http.destroy();
			buffer.destroy();
		}

		bool is_lost() const throw ()
		{
			return http.is_lost() || buffer.is_lost();
		}

		Portable<Object>                  url;
		Portable<uint8_t>                 state;
		PortableResource<HTTPTransaction> http;
		PortableResource<Buffer>          buffer;
		Portable<bool>                    forked;
	} CONCRETE_PACKED;

	explicit Fork(unsigned int address) throw (): Continuation(address) {}

public:
	bool initiate(Object &result, const TupleObject &args, const DictObject &kwargs) const
	{
		data()->url = args.get_item(0).require<StringObject>();
		return leave_state(connect(result));
	}

	bool resume(Object &result) const
	{
		if (data()->forked) {
			result = BoolObject::False();
			return true;
		}

		if (data()->is_lost()) {
			Trace("fork resources lost");

			data()->reset();
			return leave_state(connect(result));
		}

		State state = State(*data()->state);

		switch (state) {
		case Connecting:       state = connecting(result);        break;
		case Sending:          state = sending(result);           break;
		case ReceivingHeaders: state = receiving_headers(result); break;
		case ReceivingContent: state = receiving_content(result); break;
		case Done:             assert(false);
		default:               throw IntegrityError(address());
		}

		return leave_state(state);
	}

private:
	bool leave_state(State state) const
	{
		data()->state = state;
		return state == Done;
	}

	State connect(Object &result) const
	{
		Trace("fork connect");

		auto url = data()->url->require<StringObject>();

		auto resource = PortableResource<HTTPTransaction>::New(HTTP::POST, url);
		data()->http = resource;

		return connecting(result);
	}

	State connecting(Object &result) const
	{
		Trace("fork connecting");

		if (data()->http->connected())
			return send(result);

		data()->http->suspend_until_connected();
		return Connecting;
	}

	State send(Object &result) const
	{
		Trace("fork send");

		// create buffer resource before snapshotting because it affects arena
		auto resource = PortableResource<Buffer>::New();
		data()->buffer = resource;

		auto snapshot = Arena::Active().snapshot();

		auto buffer = *resource;
		buffer->reset(snapshot.size);

		auto data_ptr = data();
		// critical section
		data_ptr->forked = true;
		buffer->produce(snapshot.base, snapshot.size);
		data_ptr->forked = false;

		data()->http->set_request_length(snapshot.size);
		data()->http->reset_request_buffer(buffer);

		return sending(result);
	}

	State sending(Object &result) const
	{
		Trace("fork sending");

		if (data()->http->content_sent())
			return receive_headers(result);

		data()->http->suspend_until_content_sent();
		return Sending;
	}

	State receive_headers(Object &result) const
	{
		Trace("fork receive headers");

		data()->http->reset_request_buffer();

		data()->buffer->reset(ResponseBufferSize);
		data()->http->reset_response_buffer(*data()->buffer);

		return receiving_headers(result);
	}

	State receiving_headers(Object &result) const
	{
		Trace("fork receiving headers");

		if (data()->http->headers_received())
			return receive_content(result);

		data()->http->suspend_until_headers_received();
		return ReceivingHeaders;
	}

	State receive_content(Object &result) const
	{
		Trace("fork receive content");

		if (data()->http->response_status() != HTTP::OK)
			throw RuntimeError("HTTP response error");

		auto length = data()->http->response_length();

		if (length < 0)
			throw RuntimeError("HTTP response length not specified");

		if (length > ResponseBufferSize)
			throw RuntimeError("HTTP response too long");

		return receiving_content(result);
	}

	State receiving_content(Object &result) const
	{
		Trace("fork receiving content");

		if (data()->http->content_received())
			return done(result);

		data()->http->suspend_until_content_received();
		return ReceivingContent;
	}

	State done(Object &result) const
	{
		Trace("fork done");

		result = BoolObject::True();
		return Done;
	}

	Data *data() const
	{
		return data_cast<Data>();
	}
};

} // namespace

CONCRETE_INTERNAL_CONTINUATION(ConcreteModule_Fork, Fork)
