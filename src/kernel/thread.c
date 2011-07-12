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

#include <stddef.h>
#include <string.h>

#include "thread.h"
#include "memory.h"
#include "lock.h"

#include "help/list.h"

HELP_DLLIST(process_t , processes);
HELP_DLLIST(thread_t , threads);

static uint32_t thread_pid = 0;
static lock_t lock_thread = 0;

thread_t *thread_create(uintptr_t entry, process_t *parent, paging_context_t *context)
{
    lock(&lock_thread);

    process_t *process = parent;
    thread_t *thread = NULL;
    
    HELP_DLNODE(processes) process_node = NULL;
    HELP_DLNODE(threads) thread_node = NULL;

    {
        memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, sizeof(thread_t) + sizeof(HELP_DLNODE(threads)), 0, 0);

        if (area.size == 0)
        {
            return NULL;
        }

        thread = (thread_t *)area.address;
        thread_node = (HELP_DLNODE(threads))(area.address + sizeof(thread_t));
        thread_node->data = thread;
    }

    {
        memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, THREAD_STACKSIZE, 0, 0x1000);

        if (area.size == 0)
        {
            memory_free(CALL_AS_NON_SYSCALL, (memory_area_t){ .address = (uintptr_t)thread, .size = sizeof(thread_t) + sizeof(HELP_DLNODE(threads)) });

            return NULL;
        }

        thread->stack = area.address;
        thread->stacksize = THREAD_STACKSIZE;
    }

    {
        memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, sizeof(cpu_state_t), 0, 0);

        if (area.size == 0)
        {
            memory_free(CALL_AS_NON_SYSCALL, (memory_area_t){ .address = thread->stack, .size = THREAD_STACKSIZE });
            memory_free(CALL_AS_NON_SYSCALL, (memory_area_t){ .address = (uintptr_t)thread, .size = sizeof(thread_t) + sizeof(HELP_DLNODE(threads)) });

            return NULL;
        }

        thread->cpu = (cpu_state_t *)area.address;
    }

    if (parent == NULL)
    {
        memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, sizeof(process_t) + sizeof(HELP_DLNODE(processes)), 0, 0);

        if (area.size == 0)
        {
            memory_free(CALL_AS_NON_SYSCALL, (memory_area_t){ .address = (uintptr_t)thread->cpu, .size = sizeof(cpu_state_t) });
            memory_free(CALL_AS_NON_SYSCALL, (memory_area_t){ .address = thread->stack, .size = THREAD_STACKSIZE });
            memory_free(CALL_AS_NON_SYSCALL, (memory_area_t){ .address = (uintptr_t)thread, .size = sizeof(thread_t) + sizeof(HELP_DLNODE(threads)) });

            return NULL;
        }

        process = (process_t *)area.address;
        process_node = (HELP_DLNODE(processes))(area.address + sizeof(process_t));
        process_node->data = process;

        process->id = thread_pid++;
        process->threads = 0;

        HELP_DLLIST_ADD(processes, process_node);
    }

    thread->parent = process;
    process->threads++;

    unlock(&lock_thread);

    memset(thread->cpu, 0, sizeof(cpu_state_t));

    thread->cpu->ARCHDEP(rflags, eflags) = 0x202;
    thread->cpu->ARCHDEP(rip, eip) = entry;
    thread->cpu->ARCHDEP(rsp, esp) = thread->stack + thread->stacksize;
    thread->cpu->cs = 0x1b;
    thread->cpu->ds = 0x23;
    thread->cpu->ss = 0x23;
    thread->context = context;

    lock(&lock_thread);

    HELP_DLLIST_ADD(threads, thread_node);

    unlock(&lock_thread);

    return thread;
}

void thread_destroy(thread_t *thread, bool free_context)
{
    lock(&lock_thread);

    HELP_DLLIST_FIND(threads, thread, thread_node);

    if (thread_node != NULL)
    {
        HELP_DLLIST_REMOVE(threads, thread_node);
    }

    thread->parent->threads--;

    if (thread->parent->threads == 0)
    {
        HELP_DLLIST_FIND(processes, thread->parent, process_node);

        if (process_node != NULL)
        {
            HELP_DLLIST_REMOVE(processes, process_node);
        }

        memory_free(CALL_AS_NON_SYSCALL, (memory_area_t){ .address = (uintptr_t)thread->parent, .size = sizeof(process_t) + sizeof(HELP_DLNODE(processes)) });
    }

    unlock(&lock_thread);

    memory_free(CALL_AS_NON_SYSCALL, (memory_area_t){ .address = (uintptr_t)thread->cpu, .size = sizeof(cpu_state_t) });
    memory_free(CALL_AS_NON_SYSCALL, (memory_area_t){ .address = thread->stack, .size = THREAD_STACKSIZE });

    if (free_context == true)
    {
        /*
         * FIXME
         * Leak for now, as cleaning up requires quite an effort
         */
    }

    memory_free(CALL_AS_NON_SYSCALL, (memory_area_t){ .address = (uintptr_t)thread, .size = sizeof(thread_t) + sizeof(HELP_DLNODE(threads)) });
}

/*
 * TODO
 * Schedule next thread (thread_node->next)
 */

thread_t *thread_schedule(thread_t *current)
{
    lock(&lock_thread);

    HELP_DLLIST_FIND(threads, current, thread_node);

    if (thread_node != NULL)
    {
        if (thread_node->next != NULL)
        {
        }
    }

    unlock(&lock_thread);
}
