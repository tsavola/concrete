PYTHON		?= python3.1

-include config.mk

CPPFLAGS	+= -I. -I$(O) -Iboost
CFLAGS		+= -g -Wall
CXXFLAGS	+= -std=gnu++0x

LIBRARIES	:= concrete library
TESTS		:= example python

.DEFAULT_GOAL	:= build

build: library-shared

library-shared: concrete-reloc
example: library-shared

export PYTHON

include build/project.mk

HEADERS		:= $(shell find $(LIBRARIES) -name "*.hpp")

TAGS: $(HEADERS)
	$(call echo,Generate,$@)
	$(QUIET) etags $(HEADERS)

SOURCEDIRS	:= $(LIBRARIES) $(TESTS)
SOURCES		:= $(shell find $(SOURCEDIRS) -name "*.[ch]pp" -or -name "*.[ch]")

todo::
	$(QUIET) grep -niE "(todo|xxx)" $(SOURCES) | sed -r \
		-e "s,^(.*:[[:digit:]]+:).*[[:space:]]*//,\1," \
		-e "s,[[:space:]]*/?\*+,," \
		-e "s,[[:space:]]*\*+/[[:space:]]*,,"

GIT_VERSION	= $(shell git describe --dirty --tags --always)
GIT_BRANCH	= $(shell git rev-parse --symbolic-full-name --abbrev-ref HEAD)
GIT_REMOTE	= $(shell git config branch.$(GIT_BRANCH).remote)
GIT_URL		= $(shell git config remote.$(GIT_REMOTE).url)

doc:: doc/html/index.html

doc/html/index.html: doc/Doxyfile doc/main.txt $(shell find concrete -name '*.hpp')
	$(call echo,Doxygen,doc/html)
	$(QUIET) sed < doc/Doxyfile > doc/Doxyfile.version \
		-r "s/^(PROJECT_NUMBER[[:space:]]*=[[:space:]]*)$$/\1git-$(GIT_VERSION)/"
	$(QUIET) doxygen doc/Doxyfile.version

doc-github:: doc
	$(QUIET) ( \
		set -x && \
		test -f Makefile && \
		test -d concrete && \
		test -d .git && \
		cd doc/html && \
		rm -rf .git && \
		git init && \
		git add . && \
		git commit -m $(GIT_VERSION) && \
		git push -f $(GIT_URL) $(GIT_BRANCH):gh-pages && \
		rm -rf .git \
	)
