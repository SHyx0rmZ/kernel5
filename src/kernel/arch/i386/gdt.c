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
#include "tss.h"

/* initialize the GDT */
void gdt_init(void)
{
    /* null segment */
    gdt_set_entry(0, 0, 0, 0, 0);
    /* 32 bit code, ring 0 */
    gdt_set_entry(1, GDT_BASE_NULL, GDT_LIMIT_FULL, GDT_PRESENT | GDT_RING_0 | GDT_SEGMENT | GDT_EXECUTABLE | GDT_READABLE, GDT_GRANULAR | GDT_PROTECTED_MODE);
    /* 32 bit data, ring 0 */
    gdt_set_entry(2, GDT_BASE_NULL, GDT_LIMIT_FULL, GDT_PRESENT | GDT_RING_0 | GDT_SEGMENT | GDT_WRITABLE | GDT_DIRECTION_UP, GDT_GRANULAR | GDT_PROTECTED_MODE);
    /* 32 bit code, ring 3 */
    gdt_set_entry(3, GDT_BASE_NULL, GDT_LIMIT_FULL, GDT_PRESENT | GDT_RING_3 | GDT_SEGMENT | GDT_EXECUTABLE | GDT_READABLE, GDT_GRANULAR | GDT_PROTECTED_MODE);
    /* 32 bit data, ring 3 */
    gdt_set_entry(4, GDT_BASE_NULL, GDT_LIMIT_FULL, GDT_PRESENT | GDT_RING_3 | GDT_SEGMENT | GDT_WRITABLE | GDT_DIRECTION_UP, GDT_GRANULAR | GDT_PROTECTED_MODE);
    /* 32 bit tss */
    gdt_set_entry(5, (uintptr_t)&tss, sizeof(tss_t) - 1, GDT_PRESENT | GDT_RING_0 | GDT_TSS, GDT_NULL);
}
