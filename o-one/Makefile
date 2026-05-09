default: all
.SUFFIXES:

sed := sed
cxx := clang++

cxx.flags += -std=c++20
cxx.flags += -Wall -Wextra -Wstrict-aliasing -ferror-limit=1
cxx.flags += -O3 -g
cxx.flags += -fno-semantic-interposition -fstrict-aliasing
cxx.flags += -mlong-double-80 -fno-fast-math

cxx.flags += -mfma -mavx2 -mrdrnd -flto=thin

ldxx.flags += -lmpfr -pthread -flto

MAKEFLAGS += --no-builtin-rules
MAKEFLAGS += --no-builtin-variables
MAKEFLAGS += --no-print-directory

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


src.dir := $(root.dir)
bin.dir := ./bin
tmp.dir := ./.libs

cpp.files := $(wildcard $(src.dir)/*.cpp)

ifeq (,$(cpp.files))
$(error "no source files")
endif

src->o = $(patsubst %.cpp,$(tmp.dir)/%.o,$(1))
src->d = $(patsubst %.cpp,$(tmp.dir)/%.o.d,$(1))
src->exe = $(patsubst %.cpp,$(bin.dir)/%.exe,$(1))
exe->d = $(patsubst %.exe,$(tmp.dir)/%.d,$(1))

o.files := $(foreach s,$(cpp.files),$(call src->o,$(s)))
o.d.files := $(foreach s,$(cpp.files),$(call src->d,$(s)))
exe.files := $(foreach s,$(cpp.files),$(call src->exe,$(s)))
exe.d.files := $(foreach e,$(exe.files),$(call exe->d,$(e)))

d.files := $(o.d.files) $(exe.d.files)

ifeq (,$(is.clean))
-include $(d.files)
endif

all: $(exe.files)

%/.:
	mkdir -p $@

define cpp.link1=
$(2): $(1) | $$(dir $(2))/. $$(dir $(3))/.
	$(cxx) $(1) $(ldxx.flags) -o $(2) -Wl,-dependency-file,$(3)
endef

define cpp.compile1=
$(2): $(1) | $$(dir $(2))/. $$(dir $(3))/.
	$(cxx) $(cxx.flags) $(1) -c -o $(2) -MMD -MF $(3)
	$(sed) -i -E 's,$(2)\s*:,$(2) $(3) :,' $(3)
endef


$(foreach cpp,$(cpp.files),$(eval $(call cpp.link1,$(call src->o,$(cpp)),$(call src->exe,$(cpp)),$(call exe->d,$(call src->exe,$(cpp))))))
$(foreach cpp,$(cpp.files),$(eval $(call cpp.compile1,$(cpp),$(call src->o,$(cpp)),$(call src->d,$(cpp)))))


clean:
	rm -f $(o.files) $(d.files) $(exe.files)

.PHONY: clean

ifeq (,$(is.verbose))
.SILENT:
endif
