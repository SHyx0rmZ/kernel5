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

#include <stdbool.h>
#include <string.h>

#include "paging.h"
#include "memory.h"
#include "console.h"
#include "cpuid.h"

static uintptr_t *invalidator = (uintptr_t *)0xffffffffffffeff8ULL;
static bool nx = false;
static uintptr_t virtual_mask = 0x00000000ffffffffULL, physical_mask = 0x00000000ffffffffULL;

void paging_init(void)
{
    void paging_check_area(memory_area_t area)
    {
        if (area.size == 0)
        {
            printf("%[Ran out of memory while initializing paging!%]\n", 12);
            while(1);
        }
    }

    nx = (cpuid_extended(0x80000001).edx >> 20) & 1;
    physical_mask = (0x000fffffffffffffULL >> (76 - (cpuid_extended(0x80000008).eax & 0xff))) << 12;
    virtual_mask = (0xffffffffffffffffULL >> (76 - ((cpuid_extended(0x80000008).eax >> 8) & 0xff))) << 12;

    uintptr_t cr3;

    __asm__ __volatile__ (
        "mov %%cr3, %0"
        : "=a" (cr3)
    );

    uintptr_t *pml4 = (uintptr_t *)(cr3 & physical_mask);

    memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0x01000000, 0x1000);

    paging_check_area(area);

    pml4[511] = area.address & physical_mask;
    pml4[511] |= PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_USER_BIT;

    memset((void *)(pml4[511] & physical_mask), 0, 0x1000);

    uintptr_t *pdp = (uintptr_t *)(pml4[511] & physical_mask);

    area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0x01000000, 0x1000);

    paging_check_area(area);

    pdp[511] = area.address & physical_mask;
    pdp[511] |= PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_USER_BIT;

    memset((void *)(pdp[511] & physical_mask), 0, 0x1000);

    uintptr_t *pd = (uintptr_t *)(pdp[511] & physical_mask);

    area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0x01000000, 0x1000);

    paging_check_area(area);

    pd[511] = area.address & physical_mask;
    pd[511] |= PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_USER_BIT;

    memset((void *)(pd[511] & physical_mask), 0, 0x1000);

    uintptr_t *pt = (uintptr_t *)(pd[511] & physical_mask);
    pt[510] = (uintptr_t)pt;
    pt[510] |= PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;
    pt[511] = (uintptr_t)NULL;
    pt[511] |= PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;
}

paging_context_t *paging_context(void)
{
    void paging_check_area(memory_area_t area)
    {
        if (area.size == 0)
        {
            printf("%[Ran out of memory while mapping!%]", 12);
            while(1);
        }
    }

    memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, sizeof(paging_context_t), 0, 0);

    paging_check_area(area);

    paging_context_t *context = (paging_context_t *)area.address;

    area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0, 0x1000);

    paging_check_area(area);

    context->cr3 = area.address & physical_mask;

    *invalidator = (uintptr_t)(context->cr3 & physical_mask) | PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)0xfffffffffffff000ULL));

    memset((void *)0xfffffffffffff000ULL, 0, 0x1000);

    uintptr_t *pml4 = (uintptr_t *)0xfffffffffffff000ULL;

    area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0, 0x1000);

    paging_check_area(area);

    pml4[511] = area.address & physical_mask;
    pml4[511] |= PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_USER_BIT;

    *invalidator = (uintptr_t)(pml4[511] & physical_mask) | PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)0xfffffffffffff000ULL));

    memset((void *)0xfffffffffffff000ULL, 0, 0x1000);

    uintptr_t *pdp = (uintptr_t *)0xfffffffffffff000ULL;

    area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0, 0x1000);

    paging_check_area(area);

    pdp[511] = area.address & physical_mask;
    pdp[511] |= PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_USER_BIT;

    *invalidator = (uintptr_t)(pdp[511] & physical_mask) | PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)0xfffffffffffff000ULL));

    memset((void *)0xfffffffffffff000ULL, 0, 0x1000);

    uintptr_t *pd = (uintptr_t *)0xfffffffffffff000ULL;

    area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0, 0x1000);

    paging_check_area(area);

    pd[511] = area.address & physical_mask;
    pd[511] |= PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_USER_BIT;

    *invalidator = (uintptr_t)(pd[511] & physical_mask) | PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)0xfffffffffffff000ULL));

    memset((void *)0xfffffffffffff000ULL, 0, 0x1000);

    uintptr_t *pt = (uintptr_t *)0xfffffffffffff000ULL;
    pt[510] = (uintptr_t)(*invalidator & physical_mask);
    pt[510] |= PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;
    pt[511] = (uintptr_t)NULL;
    pt[511] |= PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;

    return context;
}

uintptr_t paging_map(paging_context_t *context, uintptr_t virtual_address, uintptr_t physical_address, uint32_t flags)
{
    void paging_check_area(memory_area_t area)
    {
        if (area.size == 0)
        {
            printf("%[Ran out of memory while mapping!%]", 12);
            while(1);
        }
    }

    uint16_t pml4i = (virtual_address >> 39) & 0x1ff;
    uint16_t pdpi = (virtual_address >> 30) & 0x1ff;
    uint16_t pdi = (virtual_address >> 21) & 0x1ff;
    uint16_t pti = (virtual_address >> 12) & 0x1ff;

    if ((pml4i == 511) && (pdpi == 511) && (pdi == 511) && (pti >= 510))
    {
        printf("%[Tried to map reserved address!%]", 12);
        while(1);
    }

    *invalidator = (uintptr_t)(context->cr3 & physical_mask) | PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)0xfffffffffffff000ULL));

    uintptr_t *pml4 = (uintptr_t *)0xfffffffffffff000ULL;

    if (!(pml4[pml4i] & 1))
    {
        memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0, 0x1000);

        paging_check_area(area);

        memset((void *)area.address, 0, 0x1000);

        pml4[pml4i] = area.address & physical_mask;
        pml4[pml4i] |= PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_USER_BIT;
    }

    *invalidator = (uintptr_t)(pml4[pml4i] & physical_mask) | PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)0xfffffffffffff000ULL));

    uintptr_t *pdp = (uintptr_t *)0xfffffffffffff000ULL;

    if (!(pdp[pdpi] & 1))
    {
        memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0, 0x1000);

        paging_check_area(area);

        memset((void *)area.address, 0, 0x1000);

        pdp[pdpi] = area.address & physical_mask;
        pdp[pdpi] |= PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_USER_BIT;
    }

    *invalidator = (uintptr_t)(pdp[pdpi] & physical_mask) | PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)0xfffffffffffff000ULL));

    uintptr_t *pd = (uintptr_t *)0xfffffffffffff000ULL;

    if (!(pd[pdi] & 1))
    {
        memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0, 0x1000);

        paging_check_area(area);

        memset((void *)area.address, 0, 0x1000);

        pd[pdi] = area.address & physical_mask;
        pd[pdi] |= PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_USER_BIT;
    }

    *invalidator = (uintptr_t)(pd[pdi] & physical_mask) | PAGE_PRESENT_BIT | PAGE_CACHEDISABLE_BIT;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)0xfffffffffffff000ULL));

    uintptr_t *pt = (uintptr_t *)0xfffffffffffff000ULL;

    pt[pti] = (uintptr_t)(physical_address & physical_mask);
    pt[pti] |= flags & 0x0f7f;

    *invalidator = (uintptr_t)NULL;

    __asm__ __volatile__ ("invlpg %0" : : "m" (*(uint8_t *)virtual_address));

    return physical_address & physical_mask;
}
