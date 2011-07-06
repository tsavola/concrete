/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "api.hpp"

#include <cassert>
#include <exception>

#include <concrete/execution.hpp>
#include <concrete/objects/code.hpp>
#include <concrete/util/trace.hpp>

using namespace concrete;

namespace {

template <typename Function, typename... Args>
static void nonthrowing_call(ConcreteError *error, Function function, Args... args) throw ()
{
	try {
		function(args...);

		error->type = CONCRETE_ERROR_NONE;
		error->message = "No error";

	} catch (const IntegrityError &e) {
		error->type = CONCRETE_ERROR_INTEGRITY;
		error->message = e.what();

	} catch (const AllocationError &e) {
		error->type = CONCRETE_ERROR_ALLOCATION;
		error->message = e.what();

	} catch (const CodeException &e) {
		error->type = CONCRETE_ERROR_CODE;
		error->message = e.what();

	} catch (const std::exception &e) {
		error->type = CONCRETE_ERROR_SYSTEM;
		error->message = e.what();

	} catch (...) {
		error->type = CONCRETE_ERROR_SYSTEM;
		error->message = "Unknown C++ exception";
	}

	if (error->type)
		Trace("error type=%u message=\"%s\"", error->type, error->message);
}

static void create(ConcreteContext **wrap_ptr)
{
	std::unique_ptr<ConcreteContext> wrap(new ConcreteContext);

	wrap->event_loop.reset(new LibeventLoop);
	wrap->context.reset(new Context(*wrap->event_loop.get()));

	*wrap_ptr = wrap.get();

	wrap.release();
}

static void resume(ConcreteContext **wrap_ptr, void *data, size_t size)
{
	std::unique_ptr<ConcreteContext> wrap(new ConcreteContext);

	wrap->event_loop.reset(new LibeventLoop);
	wrap->context.reset(new Context(*wrap->event_loop.get(), data, size));

	*wrap_ptr = wrap.get();

	wrap.release();
}

static void load(Context *context, void *data, size_t size)
{
	context->add_execution(Execution::New(CodeObject::Load(data, size)));
}

static void execute(Context *context)
{
	context->execute();
}

static void executable(Context *context, bool *result)
{
	*result = context->executable();
}

static void destroy(ConcreteContext *wrap)
{
	delete wrap;
}

} // namespace

ConcreteContext *concrete_create(ConcreteError *error)
{
	assert(error);

	Trace("concrete_create(error=%p)", error);

	ConcreteContext *wrap = NULL;
	nonthrowing_call(error, create, &wrap);

	return wrap;
}

ConcreteContext *concrete_resume(void *data, size_t size, ConcreteError *error)
{
	assert(error);

	Trace("concrete_resume(data=%p, size=%lu, error=%p)", data, size, error);

	ConcreteContext *wrap = NULL;
	nonthrowing_call(error, resume, &wrap, data, size);

	return wrap;
}

void concrete_load(ConcreteContext *wrap, void *data, size_t size, ConcreteError *error)
{
	assert(wrap);
	assert(error);
	assert(!wrap->context->is_active());

	Trace("concrete_load(wrap=%p, data=%p, size=%lu, error=%p)", wrap, data, size, error);

	ScopedContext activate(*wrap->context.get());
	nonthrowing_call(error, load, wrap->context.get(), data, size);
}

bool concrete_execute(ConcreteContext *wrap, ConcreteError *error)
{
	assert(wrap);
	assert(error);
	assert(!wrap->context->is_active());

	Trace("concrete_execute(wrap=%p, error=%p)", wrap, error);

	ScopedContext activate(*wrap->context.get());
	bool result = false;

	nonthrowing_call(error, execute, wrap->context.get());

	if (error->type == CONCRETE_ERROR_CODE)
		nonthrowing_call(error, executable, wrap->context.get(), &result);

	return result;
}

void concrete_snapshot(ConcreteContext *wrap, const void **base_ptr, size_t *size_ptr)
{
	assert(wrap);
	assert(!wrap->context->is_active());

	Trace("concrete_snapshot(wrap=%p, base_ptr=%p, size_ptr=%lu)", wrap, base_ptr, size_ptr);

	auto snapshot = wrap->context->arena().snapshot();
	*base_ptr = snapshot.base;
	*size_ptr = snapshot.size;
}

void concrete_destroy(ConcreteContext *wrap)
{
	Trace("concrete_destroy(wrap=%p)", wrap);

	if (wrap) {
		assert(!wrap->context->is_active());

		ConcreteError error;
		nonthrowing_call(&error, destroy, wrap);

		if (error.type) {
			Trace("context destructor threw error type=%u message=\"%s\"",
			      error.type, error.message);
		}
	}
}
