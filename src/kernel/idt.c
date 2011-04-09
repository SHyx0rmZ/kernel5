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

#include <stddef.h>

#include "idt.h"
#include "isrs.h"

idt_entry_t idt[IDT_MAX_ENTRIES];
static idt_pointer_t idt_pointer;

/* initialize the IDT */
void idt_init(void)
{
    uint16_t i;

    /* install dummy handlers */
    for(i = 0; LIKELY(i < IDT_MAX_ENTRIES); i++)
    {
        idt_set_entry(i, (uintptr_t)isr_stub_null, 0x08, 0x8e01);
    }

    /* install exception handlers */
    idt_set_entry(1, (uintptr_t)isr_stub_1, 0x08, 0x8e01);
    idt_set_entry(2, (uintptr_t)isr_stub_2, 0x08, 0x8e01);
    idt_set_entry(3, (uintptr_t)isr_stub_3, 0x08, 0x8e01);
    idt_set_entry(4, (uintptr_t)isr_stub_4, 0x08, 0x8e01);
    idt_set_entry(5, (uintptr_t)isr_stub_5, 0x08, 0x8e01);
    idt_set_entry(6, (uintptr_t)isr_stub_6, 0x08, 0x8e01);
    idt_set_entry(7, (uintptr_t)isr_stub_7, 0x08, 0x8e01);
    idt_set_entry(8, (uintptr_t)isr_stub_8, 0x08, 0x8e01);
    idt_set_entry(10, (uintptr_t)isr_stub_10, 0x08, 0x8e01);
    idt_set_entry(11, (uintptr_t)isr_stub_11, 0x08, 0x8e01);
    idt_set_entry(12, (uintptr_t)isr_stub_12, 0x08, 0x8e01);
    idt_set_entry(13, (uintptr_t)isr_stub_13, 0x08, 0x8e01);
    idt_set_entry(14, (uintptr_t)isr_stub_14, 0x08, 0x8e01);
    idt_set_entry(16, (uintptr_t)isr_stub_16, 0x08, 0x8e01);
    idt_set_entry(17, (uintptr_t)isr_stub_17, 0x08, 0x8e01);
    idt_set_entry(18, (uintptr_t)isr_stub_18, 0x08, 0x8e01);
    idt_set_entry(19, (uintptr_t)isr_stub_19, 0x08, 0x8e01);

    /* install IRQ handlers */
    idt_set_entry(32, (uintptr_t)isr_stub_32, 0x08, 0x8e01);
    idt_set_entry(33, (uintptr_t)isr_stub_33, 0x08, 0x8e01);
    idt_set_entry(34, (uintptr_t)isr_stub_34, 0x08, 0x8e01);
    idt_set_entry(35, (uintptr_t)isr_stub_35, 0x08, 0x8e01);
    idt_set_entry(36, (uintptr_t)isr_stub_36, 0x08, 0x8e01);
    idt_set_entry(37, (uintptr_t)isr_stub_37, 0x08, 0x8e01);
    idt_set_entry(38, (uintptr_t)isr_stub_38, 0x08, 0x8e01);
    idt_set_entry(39, (uintptr_t)isr_stub_39, 0x08, 0x8e01);
    idt_set_entry(40, (uintptr_t)isr_stub_40, 0x08, 0x8e01);
    idt_set_entry(41, (uintptr_t)isr_stub_41, 0x08, 0x8e01);
    idt_set_entry(42, (uintptr_t)isr_stub_42, 0x08, 0x8e01);
    idt_set_entry(43, (uintptr_t)isr_stub_43, 0x08, 0x8e01);
    idt_set_entry(44, (uintptr_t)isr_stub_44, 0x08, 0x8e01);
    idt_set_entry(45, (uintptr_t)isr_stub_45, 0x08, 0x8e01);
    idt_set_entry(46, (uintptr_t)isr_stub_46, 0x08, 0x8e01);
    idt_set_entry(47, (uintptr_t)isr_stub_47, 0x08, 0x8e01);

    /* install syscall handler */
    idt_set_entry(81, (uintptr_t)isr_stub_system, 0x08, 0xef07);
}

void idt_load(void)
{
    idt_pointer.limit = (IDT_MAX_ENTRIES * sizeof(idt_entry_t) - 1);
    idt_pointer.base = (uintptr_t)idt;

    __asm__ ("lidt %0" : : "m" (idt_pointer));
}
