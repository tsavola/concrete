#
# Copyright (c) 2012  Timo Savola
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#

import collections

internals_tree = collections.defaultdict(list)

@producer
def DeclareInternal(group, entry):
	global internals_tree
	internals_tree[group].append(entry)
	echo("CONCRETE_INTERNAL_SYMBOL({group}_{entry})")

@consumer
def SetModuleInternals(group, dict="dict"):
	global internals_tree
	for entry in internals_tree[group]:
		internal = entry.lower()
		echo('{dict}.set_item(StringObject::New("{internal}"), InternalObject::New(internal::{group}_{entry}));')
