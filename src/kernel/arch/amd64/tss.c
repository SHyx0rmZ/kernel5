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

#include <string.h>

#include "tss.h"
#include "memory.h"
#include "console.h"

/* initialize the TSS */
void tss_init(void)
{
    /* clear all fields */
    memset(&tss, 0, sizeof(tss_t));

    /* get stack space for IST7 */
    memory_area_t *stack = memory_alloc(0x1000, 0x200000 , 0);

    tss.ist7 = stack->address + stack->size;

    /* get stack space for IST0 */
    stack = memory_alloc(0x1000, 0x200000, 0);

    tss.ist1 = stack->address + stack->size;

    /* mark end of IO-map */
    tss.io_end = 0xff;
}
