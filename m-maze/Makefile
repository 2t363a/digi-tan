default: all
.SUFFIXES:

#!---
binary.name := maze.exe
#!---

MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables
MAKEFLAGS += --no-print-directory

export PYTHONPYCACHEPREFIX=${HOME}/.cache/py/

make.restarted := $(if $(MAKE_RESTARTS),!,)
make.goal := $(lastword $(MAKECMDGOALS))
is.clean := $(if $(findstring clean,$(make.goal)),!,)
is.verbose := $(if $(or $(V),$(v),$(VERBOSE)),!,)
makefile.name := $(lastword $(MAKEFILE_LIST))

.realpath = $(shell realpath $(1))
.realpath2 = $(shell realpath $(1) --relative-to $(2))
.dirname = $(shell dirname $(1))

.find.f = $(wildcard $(1)/*.$(2))
.find.d = $(wildcard $(1)/*/.)
.find = $(call .find.f,$(1),$(2)) $(foreach d,$(call .find.d,$(1)),$(call .find,$(d),$(2)))

makefile.dir := $(call .realpath,$(makefile.name))
root.dir := $(call .dirname,$(call .realpath2,$(makefile.name),.))

include $(root.dir)/tools.mk
include $(root.dir)/flags.mk

# ---
src.dir := $(root.dir)/src
py.dir := $(root.dir)/py

work.dir := .
tmp.dir := $(work.dir)/.libs
lib.dir := $(work.dir)/lib
bin.dir := $(work.dir)/bin

binary.file := $(bin.dir)/$(binary.name)

cc.files := $(call .find,$(src.dir),cc)
py.files := $(call .find,$(py.dir),py)

.src->o = $(patsubst $(src.dir)/%,$(tmp.dir)/%.o,$(1))
.any->d = %(1).d

o.cc.files := $(foreach cc,$(cc.files),$(call .src->o,$(cc)))
o.files := $(o.cc.files)
d.files := $(foreach o,$(o.files),$(call .any->d,$(o)))

ifeq (,$(is.clean))
-include $(d.files)
endif

ifeq (,$(o.files))
$(error "no object files)
endif

all: $(binary.file)

%/.:
	mkdir -p $@

run: $(binary.file)
	$(call .realpath,$(binary.file))

$(binary.file): $(o.files) | $(dir $(binary.file))/.
	$(ldxx) $(o.files) $(ldxx.flags) -o $(binary.file)
	echo "LDXX $(call .realpath,$(binary.file))"

define cc.compile1=
$(2): $(1) | $$(dir $(2))/.
	$(cxx) $(cxx.flags) $(1) -c -o $(2) -MMD -MF $(2).d
	$(sed) -i -E 's,$(2)\s*:,$(2) $(2).d :,' $(2).d
endef

$(foreach cc,$(cc.files),$(eval $(call cc.compile1,$(cc),$(call .src->o,$(cc)))))

py-format:
ifneq (,$(black))
	$(black) $(py.files)
endif

cxx-format:
ifneq (,$(clang-format))
	clang-format -i --style=file $(cc.files)
endif

format: py-format cxx-format

clean:
	rm -f $(o.files) $(binary.file)
	echo "clean"

cleanall:
	rm -rf $(tmp.dir) $(bin.dir) $(lib.dir)
	echo "cleanall"

.PHONY: clean py-format cxx-format

ifeq (,$(is.verbose))
.SILENT:
endif
