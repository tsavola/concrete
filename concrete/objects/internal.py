#
# Copyright (c) 2011  Timo Savola
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#

def Internal(symbol):
	echo("Object internal_symbol::{symbol}(")
	echo("  Continuation &cont,")
	echo("  Continuation::Stage stage,")
	echo("  const TupleObject *args,")
	echo("  const DictObject *kwargs)")

def InternalFunction(symbol, funcname=None, args="args", kwargs="kwargs"):
	if not funcname:
		funcname = "concrete::" + symbol

	Internal(symbol)
	echo("  {{ return {funcname}(*args, *kwargs); }}")

def ImplementInternalFunction(symbol, args="args", kwargs="kwargs"):
	ImplementInternalFunctions([symbol], args, kwargs)

def ImplementInternalFunctions(symbols, args="args", kwargs="kwargs"):
	funcname = symbols[0]

	echo("static Object {funcname}(const TupleObject &, const DictObject &);")

	for symbol in symbols:
		InternalFunction(symbol, "concrete::" + funcname)

	echo("Object {funcname}(const TupleObject &{args}, const DictObject &{kwargs})")

def ImplementInternalTemplateFunctions(params, symbols, args="args", kwargs="kwargs"):
	for funcname in symbols:
		break

	echo("template <{params}>")
	echo("static Object {funcname}(const TupleObject &, const DictObject &);")

	for symbol, call_params in symbols.items():
		InternalFunction(symbol, "concrete::%s<%s>" % (funcname, call_params))

	echo("template <{params}>")
	echo("Object {funcname}(const TupleObject &{args}, const DictObject &{kwargs})")

def InternalContinuation(symbol, typename=None, params=None):
	if not typename:
		typename = "concrete::" + symbol

	call_params = (", " + ", ".join(params)) if params else ""

	Internal(symbol)
	echo("  {{ return Continuation::Call<{typename}>(cont, stage, args, kwargs{call_params}); }}")

@consumer
def ImplementInternalContinuation(symbol, typename=None, params=None):
	InternalContinuation(symbol, typename, params)
	ImplementPointer(symbol)
