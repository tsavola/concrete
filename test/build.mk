CONCRETE	:= $(O)/lib/libconcrete.a

NAME		:= test
SOURCES		:= $(wildcard test/*.cpp)
CPPFLAGS	+= -pthread
LDFLAGS		+= -pthread
DEPENDS		+= $(CONCRETE)
LIBS		+= $(CONCRETE) -levent -lgtest

include build/test.mk
