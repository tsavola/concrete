NAME		:= example-server
SOURCES		:= $(wildcard example/server.cpp example/modules/*.cpp)

CONCRETE	:= $(O)/lib/libconcrete.a
DEPENDS		+= $(CONCRETE)
LIBS		+= $(CONCRETE) -levent -lanl

include build/binary.mk
