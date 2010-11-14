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
#include <string.h>
#include <version.h>

#include "gdt.h"
#include "idt.h"
#include "multiboot.h"
#include "memory.h"
#include "console.h"
#include "smp.h"
#include "pic.h"

/* the kernels main function, this gets called from boot.S */
void kernel_entry(multiboot_info_t *info)
{
    /* initialize and load the GDT */
    gdt_init();
    gdt_load();

    idt_init();
    idt_load();

    pic_init();

    memset((void *)0xb8000, 0, 160 * 25);

    printf("ASXSoft %[Nuke%] - kernel5 - %[build %u%] from %[%s%]\n", 9, 9, __BUILD__, 9, __COMPILED__);
    printf("================================================================================");

    if((info->flags & (1 << 6)) == 0)
    {
        printf("%[No memory map supplied!%]", 12);

        while (1);
    }

    memory_init((multiboot_memory_t *)(uintptr_t)info->mmap_addr, info->mmap_length);

    smp_init();

    printf("%[Kernel up and running...%]", 10);

    __asm__ (
        "sti \n"
    );

    /* idle */
    while (1)
    {
        __asm__ (
            "hlt \n"
        );
    }
}
