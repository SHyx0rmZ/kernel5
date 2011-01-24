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
#include "console.h"
#include "memory.h"

void syscall_invalid()
{
    printf("\n%[Invalid syscall! System halted!%]", 12);

    while (1)
    {
        __asm__ (
            "cli \n"
            "hlt \n"
        );
    }
}

memory_area_t *syscall_memory_alloc(size_t size, uintptr_t limit, uintptr_t align)
{
    memory_area_t *result = NULL;

    __asm__ (
            "push %1 \n"
            "push %2 \n"
            "push %3 \n"
            "push %4 \n"
            "int $81 \n"
            "pop %4 \n"
            "pop %3 \n"
            "pop %2 \n"
            "pop %1 \n"
            : "=a"(result) : "c"(align), "d"(limit), "S"(size), "D"((uintarch_t)1)
    );

    return result;
}

void syscall_memory_free(memory_area_t *area)
{
    __asm__ (
            "push %0 \n"
            "push %1 \n"
            "int $81 \n"
            "pop %1 \n"
            "pop %0 \n"
            : : "S"(area), "D"((uintarch_t)2)
    );
}

const uintptr_t syscall_table[] = {
    (uintptr_t)syscall_invalid,
    (uintptr_t)memory_alloc,
    (uintptr_t)memory_free,
};

const uintptr_t syscall_table_end;
