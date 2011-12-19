NAME		:= concrete-core
CXXSUFFIX	:= .cpp
SOURCES		:= $(wildcard concrete/*.cppy concrete/*/*.cppy concrete/*/*/*.cppy)
CFLAGS		+= -fvisibility=hidden -fvisibility-inlines-hidden

PREPARE_POST	:= prepare/prepare.py -d $(O)/src --post

include build/library.mk
