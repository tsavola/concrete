NAME			:= concrete-core
MANUAL_SOURCES		:= $(wildcard concrete/*.cpp concrete/*/*.cpp concrete/*/*/*.cpp)
MANUAL_HEADERS		:= $(wildcard concrete/*.hpp concrete/*/*.hpp)
TEMPLATE_SOURCES	:= $(wildcard concrete/*.cppy concrete/*/*.cppy)
TEMPLATE_HEADERS	:= $(wildcard concrete/*.hppy concrete/*/*.hppy)
GENERATED_SOURCES	:= $(patsubst %.cppy,$(O)/%.cpp,$(TEMPLATE_SOURCES))
GENERATED_HEADERS	:= $(patsubst %.hppy,$(O)/%.hpp,$(TEMPLATE_HEADERS))
INPUTS			:= $(TEMPLATE_HEADERS) $(TEMPLATE_SOURCES)
OUTPUTS			:= $(GENERATED_SOURCES) $(GENERATED_HEADERS)
SOURCES			:= $(MANUAL_SOURCES) $(GENERATED_SOURCES)
CFLAGS			+= -fvisibility=hidden -fvisibility-inlines-hidden

include build/library.mk

$(OBJECTS) $(PIC_OBJECTS): $(OUTPUTS)

$(OUTPUTS): $(INPUTS)
	$(QUIET) $(PYTHON) prepare/prepare.py -d $(O) $(INPUTS)
