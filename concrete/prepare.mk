TEMPLATES	:= $(wildcard concrete/*.*py concrete/*/*.*py concrete/*/*/*.*py)

include build/common.mk

build::
	$(QUIET) $(PYTHON) prepare/prepare.py -d $(O)/src $(TEMPLATES)
