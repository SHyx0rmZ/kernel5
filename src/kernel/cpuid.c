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

bool cpuid_initialized = false, cpuid_supported = false;
uint32_t cpuid_max_standard = 0x00000000, cpuid_max_extended = 0x80000000;

extern void cpuid_check_support(void);

cpuid_result_t cpuid(uint32_t function)
{
    cpuid_result_t result = { 0, 0, 0, 0 };

    if (cpuid_initialized == false)
    {
        cpuid_check_support();

        cpuid_initialized = true;
    }

    if (cpuid_supported == false)
    {
        return result;
    }

    if (function < 0x80000000)
    {
        if (function > cpuid_max_standard)
        {
            return result;
        }
    }
    else
    {
        if (function > cpuid_max_extended)
        {
            return result;
        }
    }

    __asm__ (
        "cpuid"
        : "=a" (result.eax), "=c" (result.ecx), "=d" (result.edx), "=b" (result.ebx) : "a" (function)
    );

    return result;
}

cpuid_result_t cpuid_standard(uint32_t function)
{
    return cpuid(function);
}

cpuid_result_t cpuid_extended(uint32_t function)
{
    if (function < 0x80000000)
    {
        function += 0x80000000;
    }

    return cpuid(function);
}
