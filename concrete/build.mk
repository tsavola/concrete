NAME			:= concrete-core
MANUAL_SOURCES		:= $(wildcard concrete/*.cpp concrete/*/*.cpp concrete/*/*/*.cpp)
MANUAL_HEADERS		:= $(wildcard concrete/*.hpp concrete/*/*.hpp)
TEMPLATE_SOURCES	:= $(wildcard concrete/*.cppt concrete/*/*.cppt)
TEMPLATE_HEADERS	:= $(wildcard concrete/*.hppt)
GENERATED_SOURCES	:= $(patsubst %.cppt,generated/%.cpp,$(TEMPLATE_SOURCES))
GENERATED_HEADERS	:= $(patsubst %.hppt,generated/%.hpp,$(TEMPLATE_HEADERS))
INPUTS			:= $(MANUAL_SOURCES) $(TEMPLATE_SOURCES) $(MANUAL_HEADERS) $(TEMPLATE_HEADERS)
OUTPUTS			:= $(GENERATED_SOURCES) $(GENERATED_HEADERS)
SOURCES			:= $(MANUAL_SOURCES) $(GENERATED_SOURCES)
CFLAGS			+= -fvisibility=hidden -fvisibility-inlines-hidden

include build/library.mk

$(OBJECTS) $(PIC_OBJECTS): $(OUTPUTS)

$(OUTPUTS): $(INPUTS) generate.py
	$(call echo,Generate,generated)
	$(QUIET) mkdir -p $(dir $(OUTPUTS))
	$(QUIET) $(PYTHON) generate.py $(INPUTS)
