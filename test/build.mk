CONCRETE	:= $(O)/lib/libconcrete.a

NAME		:= test
SOURCES		:= $(wildcard test/*.cpp)
CPPFLAGS	+= -pthread
LDFLAGS		+= -pthread
DEPENDS		+= $(CONCRETE)
LIBS		+= $(CONCRETE) -lgtest

include build/test.mk
