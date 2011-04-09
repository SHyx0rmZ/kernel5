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

#include "stdint.h"
#include "stddef.h"
#include "memory.h"
#include "syscalls.h"

memory_area_t syscall_memory_alloc(size_t size, uintptr_t limit, uintptr_t align)
{
    memory_area_t result;

    __asm__ (
            "int $81 \n"
            : "=a"(result.address), "=d"(result.size) : "c"(align), "d"(limit), "S"(size), "D"((uintarch_t)1)
    );

    return result;
}
