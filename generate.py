import os
import re
import sys

Parsers = []
Generators = []

def main(output_filename, *input_filenames):
	for filename in sorted(input_filenames):
		with open(filename) as inputfile:
			for line in inputfile:
				for pattern, parser in Parsers:
					match = pattern.match(line)
					if match:
						parser(filename, *match.groups())

	indent = re.compile(r"  ")
	error = True

	try:
		with open(output_filename, "w") as output:
			for generator in Generators:
				for fragment in generator():
					print(indent.sub("\t", str(fragment)), file=output)

				print(file=output)

		error = False

	finally:
		if error and os.path.exists(output_filename):
			os.remove(output_filename)

def parse(pattern):
	def wrapper(func):
		Parsers.append((re.compile(pattern), func))
		return func

	return wrapper

def generate(func):
	Generators.append(func)
	return func

Headers = set()

@parse(r".")
def Header(filename):
	if (filename.endswith(".hpp") and
	    not filename.endswith("-inline.hpp") and
	    filename != "concrete/internals.hpp"):
		Headers.add(filename)

@generate
def Includes():
	for filename in sorted(Headers):
		yield "#include <%s>" % filename

@generate
def NamespaceBegin():
	yield "namespace concrete {"

ObjectsByName = {}
TypedObjectNames = set()

@parse(r"\s*CONCRETE_OBJECT_.*DECL.*\(\s*(\S+)\s*,\s*(\S+)\s*\)")
class Object:

	def __init__(self, filename, name, parent_name):
		self.name = name
		self.parent_name = parent_name
		self.children = set()

		ObjectsByName[name] = self

	@property
	def parent(self):
		return ObjectsByName.get(self.parent_name)

	@property
	def typed(self):
		return self.name in TypedObjectNames

	def add_child(self, child):
		self.children.add(child)

@parse(r"\s*CONCRETE_OBJECT_((DEFAULT|DATALESS)_DECL|DECL_TYPE)\(\s*(\S+)\s*,\s*\S+\s*\)")
def TypedObject(filename, _1, _2, name):
	TypedObjectNames.add(name)

@generate
def TypeChecks():
	for o in ObjectsByName.values():
		if o.parent:
			o.parent.add_child(o)

	for o in ObjectsByName.values():
		if not o.children:
			continue

		names = [child.name for child in o.children]

		if o.typed:
			names.append(o.name)

		yield "bool TypeCheck<%s>::operator()(const TypeObject &type)" % o.name
		yield "{"
		yield "  return %s;" % " || ".join("type == %s::Type()" % n for n in sorted(names))
		yield "}"

@generate
def ObjectDestroy():
	yield "void Object::Destroy(unsigned int address, Data *data) throw ()"
	yield "{"
	yield "  auto context = Context::Active().nonthrowing_data();"
	yield "  if (!context)"
	yield "    return;"
	yield "  auto type = data->type();"
	yield "  if (type == context->object_type)"
	yield "    DestroyData(address, data);"

	for name, o in ObjectsByName.items():
		if o.typed and name not in ("Object", "NoneObject"):
			type_member = name.replace("Object", "_type").lower()

			yield "  else if (type == context->%s)" % type_member
			yield "    DestroyData(address, static_cast<%s::Data *> (data));" % name

	yield "  else"
	yield "    assert(false);"
	yield "}"

@generate
def NamespaceEnd():
	yield "} // namespace"

if __name__ == "__main__":
	main(*sys.argv[1:])
