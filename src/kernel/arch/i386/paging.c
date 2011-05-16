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

#include <string.h>

#include "paging.h"
#include "memory.h"
#include "console.h"

static uintptr_t *invalidator = (uintptr_t *)0xffffeffc;

void paging_init(void)
{
    uintptr_t cr3;

    __asm__ __volatile__ (
        "mov %%cr3, %0"
        : "=a" (cr3)
    );

    uintptr_t *pd = (uintptr_t *)(cr3 & ~0x0fff);

    memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0x01000000, 0x1000);

    if (area.size == 0)
    {
        printf("%[Ran out of memory while initializing paging!%]\n", 12);
        while(1);
    }

    pd[1023] = area.address & ~0x0fff;
    pd[1023] |= PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_USER_BIT ;

    memset((void *)(pd[1023] & ~0x0fff), 0, 0x1000);

    uintptr_t *pt = (uintptr_t *)(pd[1023] & ~0x0fff);
    pt[1022] = (uintptr_t)pt;
    pt[1022] |= PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;
    pt[1023] = (uintptr_t)NULL;
    pt[1023] |= PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;
}

paging_context_t *paging_context(void)
{
    memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, sizeof(paging_context_t), 0, 0);

    if (area.size == 0)
    {
        printf("%[Ran out of memory while mapping!%]", 12);
        while(1);
    }

    paging_context_t *context = (paging_context_t *)area.address;

    area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0, 0x1000);

    if (area.size == 0)
    {
        printf("%[Ran out of memory while mapping!%]", 12);
        while(1);
    }

    context->cr3 = area.address;

    *invalidator = (uintptr_t)(context->cr3 & ~0x0fff) | PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)0xfffff000));

    memset((void *)0xfffff000, 0, 0x1000);

    uintptr_t *pd = (uintptr_t *)0xfffff000;

    area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0, 0x1000);

    if (area.size == 0)
    {
        printf("%[Ran out of memory while mapping!%]", 12);
        while(1);
    }

    pd[1023] = area.address;
    pd[1023] |= PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_USER_BIT;

    *invalidator = (uintptr_t)(pd[1023] & ~0x0fff) | 0x0f;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)0xfffff000));

    memset((void *)0xfffff000, 0, 0x1000);

    uintptr_t *pt = (uintptr_t *)0xfffff000;
    pt[1022] = (uintptr_t)(*invalidator & ~0x0fff);
    pt[1022] |= PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;
    pt[1023] = (uintptr_t)NULL;
    pt[1023] |= PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;

    unlock(&lock_paging);

    return context;
}

uintptr_t paging_map(paging_context_t *context, uintptr_t virtual_address, uintptr_t physical_address, uint32_t flags)
{
    uint16_t pdi = (virtual_address >> 22) & 0x3ff;
    uint16_t pti = (virtual_address >> 12) & 0x3ff;

    /* if ((((pdi == 0) && (pti == 0)) || ((pdi == 1023) && (pti >= 1022))) & ((flags ) == 0)) */
    if ((pdi == 1023) && (pti >= 1022))
    {
        printf("%[Tried to map reserved address!%]", 12);
        while(1);
    }

    *invalidator = (uintptr_t)(context->cr3 & ~0x0fff) | PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)0xfffff000));

    uintptr_t *pd = (uintptr_t *)0xfffff000;

    if (!(pd[pdi] & 1))
    {
        memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0, 0x1000);

        if (area.size == 0)
        {
            printf("%[Ran out of memory while mapping!%]", 12);
            while(1);
        }

        pd[pdi] = (uintptr_t)area.address;

        memset((void *)pd[pdi], 0, 0x1000);

        pd[pdi] |= PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_USER_BIT;
    }

    *invalidator = (uintptr_t)(pd[pdi] & ~0x0fff) | PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)0xfffff000));

    uintptr_t *pt = (uintptr_t *)0xfffff000;

    pt[pti] = (uintptr_t)(physical_address & ~0x0fff);
    pt[pti] |= flags & 0x0f7f;

    *invalidator = (uintptr_t)NULL;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)virtual_address));

    return physical_address & ~0x0fff;
}
