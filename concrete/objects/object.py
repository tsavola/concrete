#
# Copyright (c) 2011  Timo Savola
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#

objects_by_name = {}

class Object:

	def __init__(self, name, parent_name=None, has_type=True, has_data=True):
		self.name = name
		self.parent_name = parent_name
		self.has_type = has_type
		self.has_data = has_data

	def __lt__(self, other):
		return self.name < other.name

	def __eq__(self, other):
		return self.name == other.name

	@property
	def short(self):
		if self.name == "Object":
			return "object"
		else:
			return self.name.replace("Object", "").lower()

	@property
	def parent(self):
		return self.parent_name and objects_by_name[self.parent_name]

	@property
	def children(self):
		return [o for o in GetObjects() if o.parent is self]

@producer
def RegisterObject(name, parent_name=None, **kwargs):
	global objects_by_name
	objects_by_name[name] = o = Object(name, parent_name, **kwargs)
	return o

@producer
def DeclareObject(name, parent_name="Object", **kwargs):
	o = RegisterObject(name, parent_name, **kwargs)

	echo("  CONCRETE_POINTER_DECL_COMMON({o.name}, {o.parent_name})")

	if o.has_data:
		echo("  CONCRETE_POINTER_DECL_DATA({o.name})")

	echo("  friend class concrete::Object;")
	echo("public:")

	if o.has_type:
		echo("  static concrete::TypeObject Type();")

@consumer
def ImplementObject(name, type_method=True, data_method=True):
	o = objects_by_name[name]

	if o.has_data and data_method:
		echo("CONCRETE_POINTER_IMPL_DATA({o.name})")

	if o.has_type and type_method:
		echo("concrete::TypeObject {o.name}::Type()")
		echo("{{")
		echo("  return concrete::Context::Active().data()->{o.short}_type;")
		echo("}}")

@consumer
def GetObject(name):
	return objects_by_name[name]

@consumer
def GetObjects():
	return sorted(objects_by_name.values())
