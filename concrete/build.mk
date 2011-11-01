NAME		:= concrete-core
SOURCES		:= $(wildcard concrete/*.cpp  concrete/*/*.cpp  concrete/*/*/*.cpp) \
		   $(wildcard concrete/*.cppy concrete/*/*.cppy concrete/*/*/*.cppy)
CFLAGS		+= -fvisibility=hidden -fvisibility-inlines-hidden

PREPARE_POST	:= prepare/prepare.py -d $(O)/src --post

include build/library.mk
