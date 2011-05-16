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

#include "gdt.h"

static gdt_entry_t gdt[GDT_MAX_ENTRIES];
static gdt_pointer_t gdt_pointer;

/* sets an entry in the GDT */
void gdt_set_entry(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags)
{
    gdt[index].limit_low = limit & 0xFFFF;
    gdt[index].flags_and_limit_high = ((flags << 4) & 0xF0) | ((limit >> 16) & 0xF);
    gdt[index].base_low = base & 0xFFFF;
    gdt[index].base_middle = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;
    gdt[index].access = access;
}

/* loads the GDT */
void gdt_load(void)
{
    gdt_pointer.limit = (GDT_MAX_ENTRIES * sizeof(gdt_entry_t) - 1);
    gdt_pointer.base = (uintptr_t)gdt;

    struct {
        uint32_t offset;
        uint16_t segment;
    } jump_address;

    jump_address.segment = 0x08;

    __asm__ __volatile__ (
        "lgdt %0 \n"
        "mov %1, %%ds \n"
        "mov %1, %%es \n"
        "mov %1, %%ss \n"
        "mov %2, %%fs \n"
        "mov %2, %%gs \n"
        "movl $gdt_jump, %3 \n"
        "ljmpl *%3 \n"
        "gdt_jump: \n"
        :: "m" (gdt_pointer), "r"(0x10), "r"(0), "m"(jump_address) : "memory"
    );
}
