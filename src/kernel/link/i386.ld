/*
 *  ASXSoft Nuke - Operating System - kernel5 - Codename: 悪意
 *  Copyright (C) 2010 - 2011 Patrick Pokatilo
 *
 *  This file is part of Nuke (悪意).
 *
 *  Nuke (悪意) is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Nuke (悪意) is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Nuke (悪意).  If not, see <http://www.gnu.org/licenses/>.
 */

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY("loader_entry")

SECTIONS
{
    . = 0x00100000;
    . = ALIGN(16);
    kernel_area_begin = .;
    .text :
    {
        . = ALIGN(16);
        *(.multiboot)
        . = ALIGN(16);
        *(.text)
    }
    .data :
    {
        . = ALIGN(16);
        start_ctors = .;
        *(.ctors)
        end_ctors = .;

        . = ALIGN(16);
        start_dtors = .;
        *(.dtors)
        end_dtors = .;

        . = ALIGN(16);
        *(.data)
        *(.rodata)
    }
    .bss :
    {
        . = ALIGN(16);
        *(.bss)
        *(.loader)
    }
    . = ALIGN(16);
    kernel_area_end = .;
}
