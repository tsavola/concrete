include build/dummy.mk

check::
	$(call echo,Check,python)
	$(QUIET) LD_LIBRARY_PATH=$(O)/lib PYTHONPATH=python $(PYTHON) -m test
