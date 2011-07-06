NAME		:= concrete
VERSION		:= 0
SOURCES		:= $(wildcard lib/*.cpp lib/*/*.cpp)
CFLAGS		+= -fvisibility=hidden -fvisibility-inlines-hidden

DEPENDS		+= $(O)/lib/libconcrete-core.pic.a
LIBS		+= $(O)/lib/libconcrete-core.pic.a -levent -lanl

include build/library.mk
