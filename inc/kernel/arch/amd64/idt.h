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

#ifndef _IDT_H_ARCH_
#define _IDT_H_ARCH_

#include <stdint.h>

#include "../../idt.h"

/* structs */
typedef struct idt_entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint16_t flags;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed)) idt_entry_t;

#endif
