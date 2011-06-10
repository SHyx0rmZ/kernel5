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
    memory_area_t stack = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0x200000 , 0x1000);

    if (stack.size == 0)
    {
        printf("%[Ran out of memory while setting up TSS!%]", 12);
        while(1);
    }

    tss.ist7 = stack.address + stack.size;

    /* get stack space for IST0 */
    stack = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0x200000, 0x1000);

    if (stack.size == 0)
    {
        printf("%[Ran out of memory while setting up TSS!%]", 12);
        while(1);
    }

    tss.ist1 = stack.address + stack.size;

    /* mark end of IO-map */
    tss.io_end = 0xff;
}
