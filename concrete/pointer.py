#
# Copyright (c) 2011, 2012  Timo Savola
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#

pointers_by_name = {}

class Pointer:

	def __init__(self, name, parent_name, has_construct=True, has_destroy=True, has_data=True):
		self.name = name
		self.parent_name = parent_name
		self.has_construct = has_construct
		self.has_destroy = has_destroy
		self.has_data = has_data

	def declare(self):
		echo("friend class Pointer;")

		echo("/** Cast an Arena offset into a typed pointer. */")
		echo("protected: explicit {self.name}(unsigned int address) throw ():")
		echo("  {self.parent_name}(address) {{}}")

		echo("/** Another reference. */")
		echo("public: {self.name}(const {self.name} &other) throw ():")
		echo("  {self.parent_name}(other) {{}}")

		if self.has_construct:
			echo("/** Type-specific null pointer. */")
			echo("public: {self.name}() throw () {{}}")

		if self.has_destroy:
			echo("/** Delete Data and nullify the pointer. */")
			echo("public: void destroy() throw ();")

		if self.has_data:
			echo("/** @struct concrete::{self.name}::Data")
			echo("    {self.name} state.  Stored in the Arena. */")
			echo("protected: struct Data;")

			echo("/** Direct short-term access to Arena memory.")
			echo("    Valid until the next memory allocation. */")
			echo("private: Data *data() const;")

		echo("public:")

	def implement(self, destroy_method=True, data_method=True):
		if self.has_destroy and destroy_method:
			echo("void {self.name}::destroy() throw ()")
			echo("  {{ DestroyPointer(*this); }}")

		if self.has_data and data_method:
			echo("{self.name}::Data *{self.name}::data() const")
			echo("  {{ return data_cast<Data>(); }}")

@producer
def DeclarePointer(name, parent_name="Pointer", **options):
	global pointers_by_name
	pointers_by_name[name] = o = Pointer(name, parent_name, **options)
	o.declare()

@consumer
def ImplementPointer(name, **options):
	pointers_by_name[name].implement(**options)
