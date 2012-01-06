#
# Copyright (c) 2011, 2012  Timo Savola
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#

__all__ = [
	"Context",
	"Error",
	"SystemError",
	"IntegrityError",
	"AllocationError",
	"CodeError",
]

import marshal
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

class Error(Exception): pass
class SystemError(Error): pass
class IntegrityError(Error): pass
class AllocationError(Error): pass
class CodeError(Error): pass

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

	def load(self, code):
		assert self.__context

		libconcrete.concrete_load(self.__context, code, len(code), self.__error)
		self.__check_error()

		self.executable = True

	def load_source(self, filename, sourcecode=None):
		if sourcecode is None:
			with open(filename) as file:
				sourcecode = file.read()

		return self.load(marshal.dumps(compile(sourcecode, filename, "exec")))

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
			message = str(self.__error.message)

			if self.__error.type == ERROR_SYSTEM:
				raise SystemError(message)
			elif self.__error.type == ERROR_INTEGRITY:
				raise IntegrityError(message)
			elif self.__error.type == ERROR_ALLOCATION:
				raise AllocationError(message)
			elif self.__error.type == ERROR_CODE:
				raise CodeError(message)

			assert False
