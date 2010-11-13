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

#include "io.h"

uint8_t inb(uint16_t port)
{
    uint8_t value;

    __asm__ __volatile__ (
        "inb %%dx, %%al"
        : "=a"(value)
        : "d"(port)
    );

    return value;
}

void outb(uint16_t port, uint8_t value)
{
    __asm__ __volatile__ (
        "outb %%al, %%dx"
        : : "a"(value), "d"(port)
    );
}
