PYTHON		:= python3.1

PY_SOURCE	:= example/example.py
O_PY_SOURCE	:= $(O)/obj/$(PY_SOURCE)
O_PY_BYTECODE	:= $(O_PY_SOURCE)c

NAME		:= example
SOURCES		:= $(wildcard example/example.cpp example/modules/*.cpp)
CPPFLAGS	+= -DEXAMPLE_BYTECODE="\""$(O_PY_BYTECODE)"\""

CONCRETE	:= $(O)/lib/libconcrete.a
DEPENDS		+= $(CONCRETE)
LIBS		+= $(CONCRETE) -levent -lanl

include build/test.mk

$(O_NAME): $(O_PY_BYTECODE)

$(O_PY_BYTECODE): $(PY_SOURCE)
	$(call echo,Compile,$@)
	$(QUIET) ln -sf $(PWD)/$(PY_SOURCE) $(O_PY_SOURCE)
	$(QUIET) $(PYTHON) -m py_compile $(O_PY_SOURCE)
