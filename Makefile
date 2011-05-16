#  ASXSoft Nuke - Operating System - kernel5 - Codename: 理コ込
#  Copyright (C) 2010 Patrick Pokatilo
#
#  This file is part of Nuke (理コ込).
#
#  Nuke (理コ込) is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Nuke (理コ込) is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Nuke (理コ込).  If not, see <http://www.gnu.org/licenses/>.

ARCH = $(shell uname -m)
DIR_CWD = $(shell pwd)

ifeq ($(ARCH),x86_64)
ARCHFLAGS = $(ARCHFLAGS_AMD64)
ARCH = $(ARCH_AMD64)
else ifeq ($(ARCH),x86)
ARCHFLAGS = $(ARCHFLAGS_I386)
ARCH = $(ARCH_I386)
endif

DIR_OBJ = $(DIR_CWD)/obj
DIR_SRC = $(DIR_CWD)/src
DIR_BIN = $(DIR_CWD)/bin
DIR_INC = $(DIR_CWD)/inc
DIR_RES = $(DIR_CWD)/res
DIR_EXT = $(DIR_OBJ)/ext

TAR_KER = $(DIR_BIN)/kernel
TAR_SMP = $(DIR_OBJ)/smpinit

MAKE = make -s
CC = gcc
LD = ld
ASM = gcc
SH = bash

LDFLAGS= -n
CFLAGS = -g -g3 -Wall -Wextra -Werror -std=gnu99 -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -fno-leading-underscore -O0 -ansi
ARCHFLAGS_AMD64 = -m64
ARCHFLAGS_I386 = -m32
ARCH_AMD64 = amd64
ARCH_I386 = i386

.PHONY: all amd64 i386 i386-extract clean clean-for-real clean-deep clean-deep-for-real image-begin image-entry image-end version dirs tools test todo fixme lines

all: dirs clean-deep tools
	@echo '-------- Building ASXSoft Nuke - Operating System - kernel5 - Codename: 理コ込'
	@echo
	@echo "           ████████████████████▒                       ██▒  ███████▒          "
	@echo "               █▒   █▒   █▒   █▒   ████████████████▒    ██▒      ██▒          "
	@echo "               █▒   ███████████▒   ████████████████▒     █▒     ████▒         "
	@echo "            ███████▒█▒   █▒   █▒                ███▒           ██▒ ██▒        "
	@echo "               █▒   ███████████▒                ███▒   ███▒   ██▒   ██▒       "
	@echo "               █▒        █▒                     ███▒     █▒  ██▒     ██▒      "
	@echo "               █████▒█████████▒    ████████████████▒     █▒ ██▒       ██▒     "
	@echo "            █████▒       █▒        ████████████████▒    ███▒             ██▒  "
	@echo "                    ███████████▒                       █▒ ████████████████▒   "
	@echo
	@$(MAKE) version
	@$(MAKE) image-begin
	@$(MAKE) "ARCH=$(ARCH)" "ARCHFLAGS=$(ARCHFLAGS)" $(ARCH)
	@$(MAKE) image-end

amd64: dirs clean
	@echo '-------- Architecture amd64'
	@$(MAKE) "ARCH=$(ARCH_AMD64)" "ARCHFLAGS=$(ARCHFLAGS_AMD64)" $(patsubst $(DIR_SRC)/%.c,$(DIR_OBJ)/%_c.o,$(patsubst $(DIR_SRC)/%.S,$(DIR_OBJ)/%_S.o,$(sort $(shell find $(DIR_SRC)/kernel \( -iregex ".*\.c" -or -iregex ".*\.S" \) -and -not -iregex "$(DIR_SRC)/kernel/arch/.*")) $(sort $(shell find $(DIR_SRC)/kernel/arch/$(ARCH_AMD64) -iregex ".*\.c" -or -iregex ".*\.S"))))
	@$(MAKE) "ARCH=$(ARCH_AMD64)" "ARCHFLAGS=$(ARCHFLAGS_AMD64)" $(TAR_KER)
	@$(MAKE) "ARCH=$(ARCH_AMD64)" image-entry
	@$(MAKE) "ARCH=$(ARCH_I386)" "ARCHFLAGS=$(ARCHFLAGS_I386)" i386

i386: dirs clean
	@echo '-------- Architecture i386'
	@$(MAKE) "ARCH=$(ARCH_I386)" "ARCHFLAGS=$(ARCHFLAGS_I386)" $(patsubst $(DIR_SRC)/%.c,$(DIR_OBJ)/%_c.o,$(patsubst $(DIR_SRC)/%.S,$(DIR_OBJ)/%_S.o,$(sort $(shell find $(DIR_SRC)/kernel \( -iregex ".*\.c" -or -iregex ".*\.S" \) -and -not -iregex "$(DIR_SRC)/kernel/arch/.*")) $(sort $(shell find $(DIR_SRC)/kernel/arch/$(ARCH_I386) -iregex ".*\.c" -or -iregex ".*\.S"))))
	@$(MAKE) i386-extract
	@$(MAKE) "ARCH=$(ARCH_I386)" "ARCHFLAGS=$(ARCHFLAGS_I386)" $(TAR_KER)
	@$(MAKE) "ARCH=$(ARCH_I386)" image-entry

i386-extract: $(shell $(CC) $(ARCHFLAGS_I386) --print-libgcc-file-name) dirs
	@$(AR) -x $$($(CC) $(ARCHFLAGS_I386) --print-libgcc-file-name) _umoddi3.o _udivdi3.o
	@mv _umoddi3.o _udivdi3.o $(DIR_EXT)

tools: dirs
	@echo '-------- Tools'
	@$(MAKE) $(patsubst $(DIR_SRC)/tools/%.c,$(DIR_BIN)/tools/%, $(shell find $(DIR_SRC)/tools -iregex ".*\.c"))

$(DIR_BIN)/tools/%: $(DIR_SRC)/tools/%.c
	@mkdir -p $(dir $@)
	@echo 'CC       $(patsubst $(DIR_BIN)/%,%,$@)'
	@$(CC) -O6 -o $@ $^

$(TAR_KER): $(patsubst $(DIR_SRC)/%.c,$(DIR_OBJ)/%_c.o,$(patsubst $(DIR_SRC)/%.S,$(DIR_OBJ)/%_S.o,$(shell find $(DIR_SRC)/kernel \( -iregex ".*\.c" -or -iregex ".*\.S" \) -and -not -iregex "$(DIR_SRC)/kernel/arch/.*") $(shell find $(DIR_SRC)/kernel/arch/$(ARCH) -iregex ".*\.c" -or -iregex ".*\.S"))) $(shell find $(DIR_EXT) -type f 2> /dev/null)
	@mkdir -p $(dir $@)
	@echo 'CC       $(patsubst $(DIR_BIN)/%,%,$@_$(ARCH))'
	@$(LD) $(LDFLAGS) -o $@_$(ARCH) $^ -T$(DIR_SRC)/kernel/link/$(ARCH).ld

$(TAR_SMP): $(patsubst $(DIR_SRC)/%.S,$(DIR_OBJ)/%_S.o,$(shell find $(DIR_SRC)/smpinit \( -iregex ".*\.S" \) -and -not -iregex "$(DIR_SRC)/smpinit/arch/.*") $(shell find $(DIR_SRC)/smpinit/arch/$(ARCH) -iregex ".*\.S"))
	@mkdir -p $(dir $@)
	@echo 'ASM      $(patsubst $(DIR_OBJ)/%,%,$@.bin)'
	@$(LD) $(LDFLAGS) -o $@.bin $^ -T$(DIR_SRC)/smpinit/link/$(ARCH).ld

$(DIR_OBJ)/kernel/smpinit_S.o: $(DIR_SRC)/kernel/smpinit.S $(TAR_SMP)
	@mkdir -p $(dir $@)
	@echo 'ASM      $(patsubst $(DIR_SRC)/%,%,$<)'
	@$(CC) -c $(CFLAGS) $(ARCHFLAGS) -I$(DIR_OBJ) -o $@ $<

$(DIR_OBJ)/kernel/%_c.o: $(DIR_SRC)/kernel/%.c
	@mkdir -p $(dir $@)
	@echo 'CC       $(patsubst $(DIR_SRC)/%,%,$<)'
	@$(CC) -c $(CFLAGS) $(ARCHFLAGS) -I$(DIR_INC)/kernel/arch/$(ARCH) -I$(DIR_INC)/kernel -I$(DIR_INC)/arch/$(ARCH) -I$(DIR_INC) -o $@ $<

$(DIR_OBJ)/%_c.o: $(DIR_SRC)/%.c
	@mkdir -p $(dir $@)
	@echo 'CC       $(patsubst $(DIR_SRC)/%,%,$<)'
	@$(CC) -c $(CFLAGS) $(ARCHFLAGS) -I$(DIR_INC)/arch/$(ARCH) -I$(DIR_INC) -o $@ $<

$(DIR_OBJ)/%_S.o: $(DIR_SRC)/%.S
	@mkdir -p $(dir $@)
	@echo 'ASM      $(patsubst $(DIR_SRC)/%,%,$<)'
	@$(CC) -c $(CFLAGS) $(ARCHFLAGS) -o $@ $<

clean:
	@echo '-------- Cleaning'
	@$(MAKE) clean-for-real

clean-for-real:
	@-echo "RM       Deleted $$(rm -Rv $(DIR_OBJ)/* 2> /dev/null | wc -l) files"

clean-deep:
	@echo '-------- Cleaning deeply'
	@$(MAKE) clean-deep-for-real

clean-deep-for-real:
	@-echo "RM       Deleted $$(rm -Rv $(DIR_BIN)/* 2> /dev/null | wc -l) files"

image-begin:
	@echo '-------- Floppy image'
	@$(DIR_RES)/image_begin.sh $(DIR_RES)/kernel5.cfg $(DIR_BIN) $(DIR_RES)
	@echo 'MOUNT    Mounted floppy image'

image-end:
	@echo '-------- Floppy image'
	@$(DIR_RES)/image_end.sh $(DIR_RES)/kernel5.cfg $(DIR_BIN)
	@echo 'UNMOUNT  Unmounted floppy image'

image-entry:
	@echo '-------- Floppy image'
	@$(DIR_RES)/image_entry.sh $(DIR_RES)/kernel5.cfg $(DIR_BIN) $(ARCH)

version:
	@echo '-------- Version information'
	@$(shell $(SH) version.sh)
	@echo 'VERSION  Updated version information'

dirs:
	@mkdir -p $(DIR_BIN)
	@mkdir -p $(DIR_SRC)
	@mkdir -p $(DIR_INC)
	@mkdir -p $(DIR_OBJ)
	@mkdir -p $(DIR_RES)
	@mkdir -p $(DIR_EXT)

test:
	@bochs -f kernel5.cfg -q

todo:
	@find src inc -type f -iregex "^.*\.\(c\|h\|S\)" -exec grep -Hn TODO \{\} \; 2> /dev/null

fixme:
	@find src inc -type f -iregex "^.*\.\(c\|h\|S\)" -exec grep -Hn FIXME \{\} \; 2> /dev/null

lines:
	@find src inc -type f -iregex "^.*\.\(c\|h\|S\)" | xargs wc | tail -n 1 2> /dev/null
