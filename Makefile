CPPFLAGS	+= -I.
CFLAGS		+= -g -Wall
CXXFLAGS	+= -std=gnu++0x

LIBRARIES	:= concrete
TESTS		:= example test

build: concrete

concrete: concrete-static
example: concrete
test: concrete

include build/project.mk

SRCDIRS		:= $(LIBRARIES) $(TESTS)
SOURCES		:= $(shell find $(SRCDIRS) -name '*.[cht]pp')

todo::
	$(QUIET) grep -niE "(todo|xxx)" $(SOURCES) | sed -r \
		-e "s,^(.*:[[:digit:]]+:).*[[:space:]]*//,\1," \
		-e "s,[[:space:]]*/?\*+,," \
		-e "s,[[:space:]]*\*+/[[:space:]]*,,"
