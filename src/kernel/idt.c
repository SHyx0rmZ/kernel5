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

#include "idt.h"

idt_entry_t idt[IDT_MAX_ENTRIES];
static idt_pointer_t idt_pointer;

void isr_handler_null(void)
{
}

/* initialize the IDT */
void idt_init(void)
{
    uint16_t i;

    for(i = 0; i < IDT_MAX_ENTRIES; i++)
    {
        idt_set_entry(i, (uintptr_t)isr_handler_null, 0x08, 0x8E00);
    }
}

void idt_load(void)
{
    idt_pointer.limit = (IDT_MAX_ENTRIES * sizeof(idt_entry_t) - 1);
    idt_pointer.base = (uintptr_t)idt;

    __asm__ ("lidt %0" : : "m" (idt_pointer));
}
