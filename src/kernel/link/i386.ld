/*
 *  ASXSoft Nuke - Operating System - kernel5 - Codename: 理コ込
 *  Copyright (C) 2010 Patrick Pokatilo
 *
 *  This file is part of Nuke (理コ込).
 *
 *  Nuke (理コ込) is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Nuke (理コ込) is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Nuke (理コ込).  If not, see <http://www.gnu.org/licenses/>.
 */

OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
ENTRY("loader_entry")

SECTIONS
{
    . = 0x00100000;
    . = ALIGN(16);
    start_kernel = .;
    .multiboot :
    {
        . = ALIGN(16);
        *(.multiboot)
    }
    .text :
    {
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
    }
    . = ALIGN(16);
    end_kernel = .;
}
