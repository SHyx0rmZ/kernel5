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

#ifndef _IDT_H_
#define _IDT_H_

/* functions */
void idt_set_entry(uint8_t index, uintptr_t offset, uint16_t selector, uint16_t flags);
void idt_load(void);
void idt_init(void);

/* structs */
typedef struct idt_pointer
{
    uint16_t limit;
    uintptr_t base;
} __attribute__((packed)) idt_pointer_t;

/* defines */
#define IDT_MAX_ENTRIES 256

#endif
