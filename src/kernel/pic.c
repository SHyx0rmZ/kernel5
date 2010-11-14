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

#include "pic.h"
#include "io.h"

void pic_init()
{
    /* tell the PIC it will get initialised */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);

    /* map master to 0x20 and slave to 0x28 */
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    
    /* tell them about each other */
    outb(0x21, 0x04);
    outb(0xA1, 0x02);

    /* necessary for 80x86 */
    outb(0x21, 0x01);
    outb(0xA1, 0x01);

    /* mask nothing */
    outb(0x21, 0x00);
    outb(0xA1, 0x00);

    /* this doesn't really belong here, but I don't care for now */
    /* this pretty much generates an IRQ with 249.98 Hz */
    uint16_t timer_divisor = 4773;

    /* tell the PIT it will get initialised and send data */
    outb(0x43, 0x34);
    outb(0x40, (timer_divisor & 0xFF));
    outb(0x40, (timer_divisor >> 8));
}
