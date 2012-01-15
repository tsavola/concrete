#
# Copyright (c) 2011  Timo Savola
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#

objects_by_name = {}

class Object(Pointer):

	def __init__(self, name, parent_name, has_type=True, has_data=True):
		super().__init__(name, parent_name, has_construct=False, has_destroy=False, has_data=has_data)

		self.has_type = has_type

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

	def declare(self):
		super().declare()

		echo("friend class Object;")

		if self.has_type:
			echo("static TypeObject Type();")
			echo("template <typename Visitor> void visit(Visitor &v) const;")

	def implement(self, type_method=True, data_method=True):
		super().implement(data_method=data_method)

		if self.has_type and type_method:
			echo("TypeObject {self.name}::Type()")
			echo("  {{ return Context::Active().data()->{self.short}_type; }}")

@producer
def RegisterObject(name, parent_name=None, **options):
	global objects_by_name
	objects_by_name[name] = o = Object(name, parent_name, **options)
	return o

@producer
def DeclareObject(name, parent_name="Object", **options):
	RegisterObject(name, parent_name, **options).declare()

@consumer
def ImplementObject(name, **options):
	objects_by_name[name].implement(**options)

@consumer
def GetObject(name):
	return objects_by_name[name]

@consumer
def GetObjects():
	return sorted(objects_by_name.values())
