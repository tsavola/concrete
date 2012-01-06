PY_SOURCE	:= example/test.py
O_PY_SOURCE	:= $(O)/obj/$(PY_SOURCE)
O_PY_BYTECODE	:= $(O)/obj/$(shell $(PYTHON) -c 'import sys; v = sys.version_info[:2]; print("example/test.pyc" if v == (3, 1) else "example/__pycache__/test.cpython-%d%d.pyc" % v)')

NAME		:= example
SOURCES		:= $(wildcard example/*.c)
CPPFLAGS	+= -Ilibrary -DEXAMPLE_BYTECODE="\""$(O_PY_BYTECODE)"\""

DEPENDS		+= $(O)/lib/libconcrete.so
LIBS		+= -L$(O)/lib -lconcrete

include build/test.mk

$(O_NAME): $(O_PY_BYTECODE)

$(O_PY_BYTECODE): $(PY_SOURCE)
	$(call echo,Compile,$@)
	$(QUIET) ln -sf $(PWD)/$(PY_SOURCE) $(O_PY_SOURCE)
	$(QUIET) $(PYTHON) -m py_compile $(O_PY_SOURCE)
