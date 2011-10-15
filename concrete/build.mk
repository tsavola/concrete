NAME		:= concrete-core
SOURCES		:= $(wildcard concrete/*.cpp \
		              concrete/*/*.cpp \
		              concrete/*/*/*.cpp) \
		   $(patsubst %.cppy,$(O)/%.cpp, \
		           $(wildcard concrete/*.cppy \
		                      concrete/*/*.cppy \
		                      concrete/*/*/*.cppy))
CFLAGS		+= -fvisibility=hidden -fvisibility-inlines-hidden

include build/library.mk
