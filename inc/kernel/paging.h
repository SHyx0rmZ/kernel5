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

#ifndef _PAGING_H_
#define _PAGING_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct paging_context
{
    uintptr_t cr3;
} paging_context_t;

void paging_init(void);
paging_context_t *paging_context(void);
void paging_switch(paging_context_t *context);
uintptr_t paging_map(paging_context_t *context, uintptr_t virtual_address, uintptr_t physical_address, uint32_t flags);

#endif
