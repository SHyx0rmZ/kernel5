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

#include <stdint.h>
#include <stdbool.h>

#include "cpuid.h"

extern bool cpuid_initialized, cpuid_supported;
extern uint32_t cpuid_max_standard, cpuid_max_extended;

void cpuid_check_support(void)
{
    __asm__ __volatile__ (
        "pushf \n"
        "pop %%rax \n"
        "mov %%rax, %%rcx \n"
        "xor $0x2000000, %%rax \n"
        "push %%rax \n"
        "popf \n"
        "pushf \n"
        "pop %%rax \n"
        "xor %%rcx, %%rax \n"
        : "=a" (cpuid_supported)
        : : "rcx"
    );

    if (cpuid_supported)
    {
        cpuid_max_standard = cpuid(0x00000000).eax;
        cpuid_max_extended = cpuid(0x80000000).eax;
    }
}
