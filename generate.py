import os
import re
import sys

Parsers = []

def main(*inputs):
	templates = []

	for inputname in sorted(inputs):
		with open(inputname) as inputfile:
			for line in inputfile:
				for pattern, parser in Parsers:
					match = pattern.match(line)
					if match:
						parser(inputname, *match.groups())

		if inputname.endswith("t"):
			templates.append(inputname)

	for inputname in templates:
		outputname = os.path.join("generated", inputname[:-1])
		tempname = outputname + ".tmp"

		with open(inputname) as inputfile:
			with open(tempname, "w") as tempfile:
				generate(inputname, inputfile, tempfile)

		os.rename(tempname, outputname)

def generate(inputname, inputfile, outputfile):
	block = False
	indent = None
	code = ""

	for text in inputfile:
		while text:
			if block:
				i = text.find("}}}")
				if i >= 0:
					block = False
					if text[:i].strip():
						code += text[:i]
					evaluate(inputname, code, indent, outputfile)
					indent = None
					code = ""
					text = text[i+3:]
				else:
					if text.strip():
						code += text
					text = ""
			else:
				i = text.find("{{{")
				if i >= 0:
					block = True
					print(text[:i], end="", file=outputfile)
					indent = re.sub(r"[^\t]", " ", text[:i])
					text = indent + "   " + text[i+3:]
					if not text.strip():
						text = ""
					text = "if True:\n" + text
				else:
					print(text, end="", file=outputfile)
					text = ""

def evaluate(inputname, code, indent, outputfile):
	def echo(line, delim=None, newline=True):
		lines.append((line.format(**codelocals), delim, newline))

	codelocals = dict(echo=echo)
	lines = []

	exec(compile(code, inputname, "exec"), globals(), codelocals)

	for i, (line, delim, newline) in enumerate(lines):
		if i > 0:
			line = indent + line

		if delim and i < len(lines) - 1:
			line += delim

		if newline and i < len(lines) - 1:
			print(line, file=outputfile)
		else:
			print(line, end="", file=outputfile)

def parse(pattern):
	def wrapper(func):
		Parsers.append((re.compile(pattern), func))
		return func

	return wrapper

Headers = set()
Objects = []
ObjectsByName = {}
TypedObjectNames = set()
DataObjectNames = set()

@parse(r".")
def Header(filename):
	filename = filename.replace(".hppt", ".hpp")

	if (filename.endswith(".hpp") and
	    not filename.endswith("-inline.hpp") and
	    filename != "concrete/internals.hpp"):
		Headers.add(filename)

@parse(r"\s*CONCRETE_OBJECT_.*DECL.*\(\s*(\S+)\s*,\s*(\S+)\s*\)")
class Object:

	def __init__(self, filename, name, parent_name):
		self.name = name
		self.parent_name = parent_name
		self.__children = None

		Objects.append(self)
		ObjectsByName[name] = self

	@property
	def short(self):
		if self.name == "Object":
			return "object"
		else:
			return self.name.replace("Object", "").lower()

	@property
	def typed(self):
		return self.name in TypedObjectNames

	@property
	def data(self):
		return self.name in DataObjectNames

	@property
	def children(self):
		if self.__children is None:
			self.__children = set()

			for o in Objects:
				if o.parent_name == self.name:
					self.__children.add(o)

		return self.__children

@parse(r"\s*CONCRETE_OBJECT_((DEFAULT|DATALESS)_DECL|DECL_TYPE)\(\s*(\S+)\s*,\s*\S+\s*\)")
def TypedObject(filename, _1, _2, name):
	TypedObjectNames.add(name)

@parse(r"\s*CONCRETE_OBJECT_(DEFAULT_DECL|DECL_DATA)\(\s*(\S+)\s*,\s*\S+\s*\)")
def DataObject(filename, _1, name):
	DataObjectNames.add(name)

Object("synthetic", "Object", None)
TypedObjectNames.add("Object")

if __name__ == "__main__":
	main(*sys.argv[1:])
