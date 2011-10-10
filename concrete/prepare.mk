TEMPLATES	:= $(wildcard concrete/*.*ppy concrete/*/*.*ppy)

include build/common.mk

build::
	$(QUIET) $(PYTHON) prepare/prepare.py -d $(O) $(TEMPLATES)
