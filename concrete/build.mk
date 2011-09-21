NAME		:= concrete-core
MANUAL_SOURCES	:= $(wildcard concrete/*.cpp concrete/*/*.cpp concrete/*/*/*.cpp)
MANUAL_HEADERS	:= $(wildcard concrete/*.hpp concrete/*/*.hpp)
GENERATED_SOURCE:= $(O)/src/concrete/generated.cpp
SOURCES		:= $(MANUAL_SOURCES) $(GENERATED_SOURCE)
CFLAGS		+= -fvisibility=hidden -fvisibility-inlines-hidden

include build/library.mk

$(GENERATED_SOURCE): $(MANUAL_SOURCES) $(MANUAL_HEADERS) generate.py
	$(call echo,Generate,$@)
	$(QUIET) mkdir -p $(dir $@)
	$(QUIET) $(PYTHON) generate.py $@ $(MANUAL_SOURCES) $(MANUAL_HEADERS)
