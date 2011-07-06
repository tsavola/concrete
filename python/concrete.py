#!/usr/bin/env python3.1

__all__ = ["Context", "Error"]

from ctypes import *

libc = CDLL("libc.so.6")
libconcrete = CDLL("libconcrete.so.0")

class ErrorStruct(Structure):
	_fields_ = [
		("type",    c_uint),
		("message", c_char_p),
	]

libc.malloc.argtypes = [c_size_t]
libc.malloc.restype = POINTER(c_char)

libconcrete.concrete_create.argtypes = [POINTER(ErrorStruct)]
libconcrete.concrete_create.restype = c_void_p

libconcrete.concrete_resume.argtypes = [c_void_p, c_size_t, POINTER(ErrorStruct)]
libconcrete.concrete_resume.restype = c_void_p

libconcrete.concrete_load.argtypes = [c_void_p, c_void_p, c_size_t, POINTER(ErrorStruct)]
libconcrete.concrete_load.restype = None

libconcrete.concrete_execute.argtypes = [c_void_p, POINTER(ErrorStruct)]
libconcrete.concrete_execute.restype = c_bool

libconcrete.concrete_snapshot.argtypes = [c_void_p, POINTER(c_void_p), POINTER(c_size_t)]
libconcrete.concrete_snapshot.restype = None

libconcrete.concrete_destroy.argtypes = [c_void_p]
libconcrete.concrete_destroy.restype = None

ERROR_NONE       = 0
ERROR_SYSTEM     = 1
ERROR_INTEGRITY  = 2
ERROR_ALLOCATION = 3
ERROR_CODE       = 4

class Error(Exception):
	def __init__(self, struct):
		Exception.__init__(self, str(struct.message))
		self.type = struct.type

class Context(object):
	def __init__(self, snapshot=None):
		self.__error = ErrorStruct()

		if snapshot is None:
			self.__context = libconcrete.concrete_create(self.__error)
		else:
			size = len(snapshot)
			data = libc.malloc(size)
			if data is None:
				raise MemoryError()

			for i, byte in enumerate(snapshot):
				data[i] = byte

			self.__context = libconcrete.concrete_resume(data, size, self.__error)

		self.__check_error()

		self.executable = True

	def __del__(self):
		self.destroy()

	def __enter__(self):
		return self

	def __exit__(self, *exc):
		self.destroy()

	def load(self, data):
		assert self.__context

		libconcrete.concrete_load(self.__context, data, len(data), self.__error)
		self.__check_error()

		self.executable = True

	def execute(self):
		assert self.__context

		self.executable = libconcrete.concrete_execute(self.__context, self.__error)
		self.__check_error()

	def snapshot(self):
		assert self.__context

		base_ptr = pointer(c_void_p())
		size_ptr = pointer(c_size_t())

		libconcrete.concrete_snapshot(self.__context, base_ptr, size_ptr)

		base = cast(base_ptr[0], POINTER(c_char))
		size = size_ptr[0]

		return base[:size]

	def destroy(self):
		if self.__context:
			libconcrete.concrete_destroy(self.__context)
			self.__context = None
			self.executable = False

	def __check_error(self):
		if self.__error.type:
			raise Error(self.__error)

def test():
	import sys

	with Context() as context:
		with open("debug/obj/example/test.pyc", "rb") as file:
			context.load(file.read())

		snapshot = context.snapshot()

	print("Changing context")

	with Context(snapshot) as context:
		while context.executable:
			try:
				context.execute()
			except Error as e:
				print("Error:", e, file=sys.stderr)

if __name__ == "__main__":
	test()
