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

#ifndef _MEMORY_H_
#define _MEMORY_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "multiboot.h"

typedef struct memory_area
{
    uintptr_t address;
    size_t size;
} memory_area_t;

memory_area_t memory_foreach(memory_area_t last_element);
memory_area_t memory_alloc(SYSCALL, size_t size, uintptr_t limit, uintptr_t align);
void memory_free(SYSCALL, memory_area_t area);
void memory_init(multiboot_memory_t *memory, uint32_t length);

#endif
