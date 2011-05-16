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

#include "lock.h"

void lock(lock_t *lock)
{
    __asm__ __volatile__ (
        "movb $1, %%cl \n"
        "lock_loop: \n"
        "xorb %%al, %%al \n"
        "lock cmpxchgb %%cl, (%0) \n"
        "jnz lock_loop \n" : : "D" (lock) : "eax", "ecx"
    );
}

void unlock(lock_t *lock)
{
    *lock = 0;
}

bool locked(lock_t *lock)
{
    return (*lock == 0) ? false : true;
}

void lock_wait(lock_t *lock)
{
    /* TODO: thread_yield() */
    while (locked(lock))
    {
        __asm__ __volatile__ ("nop");
    }
}

void locked_increment(lock_t *lock)
{
    __asm__ __volatile__ ("lock incl (%0)" : : "r" (lock));
}

void locked_decrement(lock_t *lock)
{
    __asm__ __volatile__ ("lock decl (%0)" : : "r" (lock));
}
