include build/dummy.mk

check::
	$(call echo,Check,python)
	$(QUIET) LD_LIBRARY_PATH=$(O)/lib $(PYTHON) python/concrete.py
