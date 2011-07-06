NAME		:= concrete-core
SOURCES		:= $(wildcard concrete/*.cpp concrete/*/*.cpp concrete/*/*/*.cpp)
CFLAGS		+= -fvisibility=hidden -fvisibility-inlines-hidden

include build/library.mk
