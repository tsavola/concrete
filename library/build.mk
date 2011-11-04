NAME		:= concrete
VERSION		:= 0
SOURCES		:= $(wildcard library/*.cpp library/*/*.cpp library/io/$(LIBRARY_RESOLVER)/*.cpp)
CFLAGS		+= -fvisibility=hidden -fvisibility-inlines-hidden

DEPENDS		+= $(O)/lib/libconcrete-core.pic.a
LIBS		+= $(O)/lib/libconcrete-core.pic.a

include build/library.mk
