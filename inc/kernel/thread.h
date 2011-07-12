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

#ifndef _THREAD_H_
#define _THREAD_H_

#include <stdint.h>
#include <stdbool.h>

#include "state.h"
#include "paging.h"

#define THREAD_STACKSIZE 0x1000

typedef struct process process_t;
typedef struct thread thread_t;

struct process
{
    uint32_t id;
    uint16_t threads;
};

struct thread
{
    process_t *parent;
    paging_context_t *context;
    cpu_state_t *cpu;
    uintptr_t stack;
    uint32_t stacksize;
};

thread_t *thread_create(uintptr_t entry, process_t *parent, paging_context_t *context);
void thread_destroy(thread_t *thread, bool free_context);
thread_t *thread_schedule(thread_t *current);

#endif
