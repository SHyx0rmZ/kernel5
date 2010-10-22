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

#ifndef _GDT_H_
#define _GDT_H_

/* functions */
void gdt_set_entry(uint8_t index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void gdt_load();
void gdt_init();

/* structs */
typedef struct
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t flags_and_limit_high;
    uint8_t base_high;
} __attribute__((packed)) gdt_entry_t;

typedef struct
{
    uint16_t limit;
    uintptr_t base;
} __attribute__((packed)) gdt_pointer_t;

/* defines */
#define GDT_BASE_NULL       0
#define GDT_LIMIT_FULL      0xffffffff

#define GDT_PRESENT         0x80
#define GDT_RING_3          0x60
#define GDT_RING_2          0x40
#define GDT_RING_1          0x20
#define GDT_RING_0          0x00
#define GDT_SEGMENT         0x10
#define GDT_EXECUTABLE      0x08
#define GDT_DIRECTION_DOWN  0x04
#define GDT_DIRECTION_UP    0x00
#define GDT_CONFORMING      0x04
#define GDT_READABLE        0x02
#define GDT_WRITABLE        0x02
#define GDT_ACCESSED        0x01

#define GDT_GRANULAR        0x08
#define GDT_PROTECTED_MODE  0x04
#define GDT_REAL_MODE       0x00
#define GDT_LONG_MODE       0x02
#define GDT_AVAILABLE       0x01

#endif
