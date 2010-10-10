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

TAR_KER = $(DIR_BIN)/kernel

MAKE = make -s
CC = gcc
LD = ld
ASM = gcc
SH = bash

LDFLAGS= -n
CFLAGS = -g -Wall -Wextra -Werror -std=gnu99 -ffreestanding -nostdlib -nostartfiles -nodefaultlibs -fno-leading-underscore -O2
ARCHFLAGS_AMD64 = -m64
ARCHFLAGS_I386 = -m32
ARCH_AMD64 = amd64
ARCH_I386 = i386

.PHONY: all amd64 i386 clean clean-for-real clean-deep clean-deep-for-real image-begin image-entry image-end version dirs

all: dirs clean-deep
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
	@$(MAKE) "ARCH=$(ARCH_AMD64)" "ARCHFLAGS=$(ARCHFLAGS_AMD64)" $(patsubst $(DIR_SRC)/%.c,$(DIR_OBJ)/%_c.o,$(patsubst $(DIR_SRC)/%.S,$(DIR_OBJ)/%_S.o,$(shell find $(DIR_SRC)/kernel \( -iregex ".*\.c" -or -iregex ".*\.S" \) -and -not -iregex "$(DIR_SRC)/kernel/arch/.*") $(shell find $(DIR_SRC)/kernel/arch/$(ARCH_AMD64) -iregex ".*\.c" -or -iregex ".*\.S")))
	@$(MAKE) "ARCH=$(ARCH_AMD64)" "ARCHFLAGS=$(ARCHFLAGS_AMD64)" $(TAR_KER)
	@$(MAKE) "ARCH=$(ARCH_AMD64)" image-entry
	@$(MAKE) "ARCH=$(ARCH_I386)" "ARCHFLAGS=$(ARCHFLAGS_I386)" i386

i386: dirs clean
	@echo '-------- Architecture i386'
	@$(MAKE) "ARCH=$(ARCH_I386)" "ARCHFLAGS=$(ARCHFLAGS_I386)" $(patsubst $(DIR_SRC)/%.c,$(DIR_OBJ)/%_c.o,$(patsubst $(DIR_SRC)/%.S,$(DIR_OBJ)/%_S.o,$(shell find $(DIR_SRC)/kernel \( -iregex ".*\.c" -or -iregex ".*\.S" \) -and -not -iregex "$(DIR_SRC)/kernel/arch/.*") $(shell find $(DIR_SRC)/kernel/arch/$(ARCH_I386) -iregex ".*\.c" -or -iregex ".*\.S")))
	@$(MAKE) "ARCH=$(ARCH_I386)" "ARCHFLAGS=$(ARCHFLAGS_I386)" $(TAR_KER)
	@$(MAKE) "ARCH=$(ARCH_I386)" image-entry

$(TAR_KER): $(patsubst $(DIR_SRC)/%.c,$(DIR_OBJ)/%_c.o,$(patsubst $(DIR_SRC)/%.S,$(DIR_OBJ)/%_S.o,$(shell find $(DIR_SRC)/kernel \( -iregex ".*\.c" -or -iregex ".*\.S" \) -and -not -iregex "$(DIR_SRC)/kernel/arch/.*") $(shell find $(DIR_SRC)/kernel/arch/$(ARCH) -iregex ".*\.c" -or -iregex ".*\.S")))
	@mkdir -p $(dir $@)
	@echo 'CC       $(patsubst $(DIR_BIN)/%,%,$@_$(ARCH))'
	@$(LD) $(LDFLAGS) -o $@_$(ARCH) $^ -T$(DIR_SRC)/kernel/link/$(ARCH).ld

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
	@echo 'MOUNT'
	@echo 'drive x:\nfile="$(DIR_BIN)/floppy.img" cylinders=80 heads=2 sectors=18 filter' > ~/.mtoolsrc
	@cp $(DIR_RES)/base.img.bz2 $(DIR_BIN)/floppy.img.bz2
	@bunzip2 $(DIR_BIN)/floppy.img.bz2
	@mkdir -p $(DIR_BIN)/boot/grub
	@cp $(DIR_RES)/menu.lst $(DIR_BIN)/boot/grub/menu.lst

image-end:
	@echo '-------- Floppy image'
	@echo 'UNMOUNT'
	@mcopy -s $(DIR_BIN)/boot/grub/menu.lst x:/boot/grub/menu.lst
	@rm ~/.mtoolsrc

image-entry:
	@echo '-------- Floppy image'
	@echo 'GRUBADD  kernel_$(ARCH)'
	@echo >> $(DIR_BIN)/boot/grub/menu.lst
	@echo 'title Nuke, kernel5, $(ARCH)' >> $(DIR_BIN)/boot/grub/menu.lst
	@echo '\troot (fd0)' >> $(DIR_BIN)/boot/grub/menu.lst
	@echo '\tkernel /kernel_$(ARCH)' >> $(DIR_BIN)/boot/grub/menu.lst
	@echo '\tboot' >> $(DIR_BIN)/boot/grub/menu.lst
	@mcopy $(DIR_BIN)/kernel_$(ARCH) x:/kernel_$(ARCH)

version:
	@echo '-------- Version information'
	@echo '         Updating version information'
	@$(shell $(SH) version.sh)

dirs:
	@mkdir -p $(DIR_BIN)
	@mkdir -p $(DIR_SRC)
	@mkdir -p $(DIR_INC)
	@mkdir -p $(DIR_OBJ)
	@mkdir -p $(DIR_RES)
