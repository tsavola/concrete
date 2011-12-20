#
# Copyright (c) 2011  Timo Savola
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#

def ImplementInternalNestedCall(symbol, args="args", kwargs="kwargs"):
	declare = symbol + "_declare"
	echo("static NestedCall {declare}(const TupleObject &, const DictObject &);")

	InternalContinuation(symbol, "NestedCallContinuation", [declare])

	echo("NestedCall {declare}(const TupleObject &{args}, const DictObject &{kwargs})")
