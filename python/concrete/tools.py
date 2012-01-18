#!/usr/bin/env python3
#
# Copyright (c) 2012  Timo Savola
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#

import argparse
import struct

class Arena(object):

	class Node(object):

		_alignment = 8

		@classmethod
		def _aligned_size(cls, size):
			return (size + cls._alignment - 1) & ~(cls._alignment - 1)

		def __init__(self, arena, address):
			self.arena   = arena
			self.address = address

		def __eq__(self, other):
			return self.address == other.address

		def __lt__(self, other):
			return self.address < other.address

		def _uint32(self, offset):
			return self.arena._uint32(self.address + offset)

		def _data(self, offset, size):
			return self.arena._data(self.address + offset, size)

	class Allocation(Node):

		def __str__(self):
			return "Allocation at %u size %u %r" % (self.data_address, self.data_size, self.data)

		@property
		def size(self):
			return self._aligned_size(4 + self.data_size)

		@property
		def data_address(self):
			return self.address + 4

		@property
		def data_size(self):
			return self._uint32(0)

		@property
		def data(self):
			return self._data(4, self.data_size)

	class FreeNode(Node):

		def __str__(self):
			return "Free space from %u to %u" % (self.address, self.address + self.size)

		@property
		def size(self):
			return self._uint32(0)

		@property
		def next_node(self):
			address = self.next_address
			return self.arena.free_nodes[address] if address else None

		@property
		def next_address(self):
			return self._uint32(4)

	_initial_address = 4

	def __init__(self, data):
		self.data = data

	def init(self):
		self.allocations = {}
		self.free_nodes  = {}

		if len(self.data) < self._initial_address:
			return

		last_node = None
		next_addr = self._uint32(0)

		while next_addr:
			self.__init_allocations(last_node, next_addr)

			node = self.FreeNode(self, next_addr)
			self.free_nodes[node.address] = node

			last_node = node
			next_addr = node.next_address

			assert not next_addr or last_node.address < next_addr

		self.__init_allocations(last_node, len(self.data))

	def __init_allocations(self, prev_node, end):
		address = prev_node.address + prev_node.size if prev_node else self._initial_address

		while address < end:
			node = self.Allocation(self, address)
			self.allocations[node.address] = node

			address = node.address + node.size

	@property
	def nodes(self):
		nodes = {}
		nodes.update(self.allocations)
		nodes.update(self.free_nodes)
		return nodes

	def dump(self):
		for node in sorted(self.nodes.values()):
			print(node)

	def _uint32(self, address):
		return struct.unpack("<I", self._data(address, 4))[0]

	def _data(self, address, size):
		if address + size > len(self.data):
			raise Exception("address %u size %u out of arena (size %u)" % (address, size, len(self.data)))

		return self.data[address:address+size]

def main():
	parser = argparse.ArgumentParser()
	subparsers = parser.add_subparsers()

	arena_parser = subparsers.add_parser("arena")
	arena_parser.set_defaults(func=arena_command)
	arena_parser.add_argument("filename", type=str, metavar="FILE")
	arena_parser.add_argument("--dump", action="store_true")

	args = parser.parse_args()
	args.func(args)

def arena_command(args):
	error = None

	with open(args.filename, "rb") as file:
		arena = Arena(file.read())

	try:
		arena.init()
	except Exception as e:
		error = e

	if args.dump:
		arena.dump()

	if error:
		raise error

if __name__ == "__main__":
	main()
