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

#include "state.h"
#include "console.h"
#include "io.h"

cpu_state_t *isr_handler(cpu_state_t *cpu)
{
    if(cpu->vector < 0x20)
    {
        /* Exception */
        printf("\n%[Encountered exception!%]\nVector: #% 2u", 12, cpu->vector);

        while(1)
        {
            __asm__ (
                "cli \n"
                "hlt \n"
            );
        }
    }
    else if(cpu->vector < 0x30)
    {
        /* IRQ */
        printf("\n%[Encountered IRQ!%]\nVector: #%u", 12, cpu->vector);

        if(cpu->vector >= 0x28)
        {
            outb(0xA0, 0x20);
        }

        outb(0x20, 0x20);
    }
    else
    {
        printf("\nVector: #% 2u", cpu->vector);
    }

    __asm__ (
        "sti"
    );

    return cpu;

    /*
     * ss (may be invalid)
     * rsp/esp (may be invalid)
     * rflags/eflags
     * cs
     * rip/eip
     * error
     */
}