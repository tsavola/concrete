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
	CONCRETE_CONTINUATION_DEFAULT_DECL(Fork, Continuation)

protected:
	enum State { Open, Opening, ReceivingContent, Done };
	enum { ResponseBufferSize = 4096 };

	struct Data: Noncopyable {
		~Data() throw ()
		{
			reset();
		}

		void reset() throw ()
		{
			request.destroy();
			response.destroy();
			http.destroy();
		}

		bool is_lost() const throw ()
		{
			return request.is_lost() || response.is_lost() || http.is_lost();
		}

		Portable<Object>                  url;
		Portable<uint8_t>                 state;
		PortableResource<Buffer>          request;
		PortableResource<Buffer>          response;
		PortableResource<HTTPTransaction> http;
		Portable<bool>                    forked;
	} CONCRETE_PACKED;

public:
	bool initiate(Object &result, const TupleObject &args, const DictObject &kwargs) const
	{
		data()->url = args.get_item(0).require<StringObject>();

		// postpone actual work to resume() so that everything is set
		// up by Execution::initiate_call
		return leave_state(Open);
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
			return leave_state(open(result));
		}

		State state = State(*data()->state);

		switch (state) {
		case Open:             state = open(result);              break;
		case Opening:          state = opening(result);           break;
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

	State open(Object &result) const
	{
		Trace("fork open");

		auto url = data()->url->require<StringObject>();

		// create buffer resource before snapshotting because it affects arena
		auto request = PortableResource<Buffer>::New();
		data()->request = request;

		auto snapshot = Arena::Active().snapshot();

		request->reset(snapshot.size);

		auto data_ptr = data();
		// critical section
		data_ptr->forked = true;
		request->produce(snapshot.base, snapshot.size);
		data_ptr->forked = false;

		auto response = PortableResource<Buffer>::New(size_t(ResponseBufferSize));
		data()->response = response;

		auto http = PortableResource<HTTPTransaction>::New(HTTP::POST, url, *request);
		data()->http = http;

		http->reset_response_buffer(*response);

		return opening(result);
	}

	State opening(Object &result) const
	{
		Trace("fork opening");

		if (data()->http->headers_received())
			return receive_content(result);

		data()->http->suspend_until_headers_received();

		return Opening;
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
};

CONCRETE_CONTINUATION_DEFAULT_IMPL(Fork)

} // namespace

CONCRETE_INTERNAL_CONTINUATION(ConcreteModule_Fork, Fork)
