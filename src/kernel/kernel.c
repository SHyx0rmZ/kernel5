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

#include <stdint.h>

#include "gdt.h"
#include "multiboot.h"
#include "memory.h"
#include "console.h"

/* the kernels main function, this gets called from boot.S */
void kernel_entry(multiboot_info_t *info)
{
    /* initialize and load the GDT */
    gdt_init();
    gdt_load();

    short *video;
    char *text = "Kernel up and running...";
    char *c;
    short color = 0x0a00;

    if((info->flags & (1 << 6)) == 0)
    {
        text = "No memory map available. Halting...";
        color = 0x0c00;
    }

    memory_init((multiboot_memory_t *)(uintptr_t)info->mmap_addr, info->mmap_length);

    /* print message to screen */
    for (c = text, video = (short *)0xb8000; *c; video++, c++)
    {
        *video = color | *c;
    }

    /* idle */
    while (1)
    {
        __asm__ (
            "cli \n"
            "hlt \n"
        );
    }
}
