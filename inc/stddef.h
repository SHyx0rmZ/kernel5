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

#ifndef _STDDEF_H_
#define _STDDEF_H_

#ifndef __cplusplus
#define NULL (void *)0
#else
#define NULL 0
#endif

/* these are necessary for syscalls */
#define SYSCALL uintarch_t reserved __attribute__((unused))
#define EMPTY 0
#define CALL_AS_NON_SYSCALL EMPTY

/* these are for optimizations */
#define LIKELY(x) __builtin_expect((x), 1)
#define UNLIKELY(x) __builtin_expect((x), 0)

/* shortcuts */
#ifdef __LP64__
#define ARCHDEP(x,y) x
#else
#define ARCHDEP(x,y) y
#endif

#define PRINT_DEBUG

#endif
