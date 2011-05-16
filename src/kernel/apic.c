/*
 *  ASXSoft Nuke - Operating System - kernel5 - Codename: 理コ込
 *  Copyright (C) 2011 Patrick Pokatilo
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

#include <stddef.h>

#include "apic.h"
#include "paging.h"

static uint32_t apic_local_apic_base = (uint32_t)(uintptr_t)NULL;

void local_apic_init(uint32_t local_apic_base)
{
    apic_local_apic_base = local_apic_base;

    local_apic_write(0xf0, (local_apic_read(0xf0) & 0x0200) | 0x015f);
}

void local_apic_write(uint32_t offset, uint32_t value)
{
    uint32_t *ptr = (uint32_t *)(uintptr_t)(apic_local_apic_base + offset);

    *ptr = value;
}

uint32_t local_apic_read(uint32_t offset)
{
    uint32_t *ptr = (uint32_t *)(uintptr_t)(apic_local_apic_base + offset);

    return *ptr;
}
