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

#include "memory.h"
#include "console.h"

#define MEMORY_OVERHEAD (sizeof(memory_container_t) + sizeof(memory_area_t))

extern void kernel_area_begin(void);
extern void kernel_area_end(void);

typedef struct memory_container
{
    memory_area_t *area;
    bool freeable;
    struct memory_container *next;
    struct memory_container *prev;
} memory_container_t;

static memory_container_t *list_free = NULL;
static memory_container_t *list_used = NULL;

/* print memory information */
void memory_information()
{
    memory_container_t *i, *j;

    i = list_free;

    while (LIKELY(i != NULL))
    {
        j = i->next;

        printf("free | %p - %p%" ARCHDEP("46", "30") "P | %s\n", i->area->address, i->area->address + i->area->size, ' ', ((i->freeable == true) ? "freeable" : "not freeable"));

        i = j;
    }

    i = list_used;

    while (LIKELY(i != NULL))
    {
        j = i->next;

        printf("used | %p - %p%" ARCHDEP("46", "30") "P | %s\n", i->area->address, i->area->address + i->area->size, ' ', ((i->freeable == true) ? "freeable" : "not freeable"));

        i = j;
    }
}

/* sort the memory_list by size, descending */
void memory_sort(memory_container_t **start)
{
    memory_container_t *i = *start, *j, *c;

    /* selection sort (?) */
    while(LIKELY(i != NULL))
    {
        j = i->next;
        c = i;

        while (LIKELY(j != NULL))
        {
            if (j->area->address > c->area->address)
            {
                c = j;
            }

            j = j->next;
        }

        if (LIKELY(c != i))
        {
            if (LIKELY(c->prev != NULL))
            {
                c->prev->next = i;
            }

            if (LIKELY(c->next != NULL))
            {
                c->next->prev = i;
            }

            if (LIKELY(i->prev != NULL))
            {
                i->prev->next = c;
            }

            if (LIKELY(i->next != NULL))
            {
                i->next->prev = c;
            }

            j = i->prev;
            i->prev = c->prev;
            c->prev = j;
            j = i->next;
            i->next = c->next;
            c->next = j;

            if (UNLIKELY(i == *start))
            {
                *start = c;
            }

            i = j;
        }
        else
        {
            i = i->next;
        }
    }
}

/* add a memory container after an other */
__inline__ void memory_link_prev_next(memory_container_t *prev, memory_container_t *next)
{
    if (LIKELY(prev->next != NULL))
    {
        prev->next->prev = next;
    }

    next->next = prev->next;
    prev->next = next;
    next->prev = prev;
}

/* add a memory container before an other */
__inline__ void memory_link_next_prev(memory_container_t *next, memory_container_t *prev)
{
    if (LIKELY(next->prev != NULL))
    {
        next->prev->next = prev;
    }
    else
    {
        if (LIKELY(list_free == next))
        {
            list_free = prev;
        }
        else if (LIKELY(list_used == next))
        {
            list_used = prev;
        }
    }

    prev->prev = next->prev;
    next->prev = prev;
    prev->next = next;
}

/* move memory container from one memory list to an other */
void memory_move_from_to(memory_container_t *container, memory_container_t **from, memory_container_t **to)
{
    /* is first element */
    if ((LIKELY(from != NULL)) && (UNLIKELY(*from == container)))
    {
        *from = container->next;
    }

    if (LIKELY(container->prev != NULL))
    {
        container->prev->next = container->next;
    }

    if (LIKELY(container->next != NULL))
    {
        container->next->prev = container->prev;
    }

    memory_container_t *iterator = *to;

    /* move to correct position */
    while ((LIKELY(iterator != NULL)) && (LIKELY(iterator->next != NULL)) && (iterator->area->address > container->area->address))
    {
        iterator = iterator->next;
    }

    /* insert element */
    if (UNLIKELY(iterator == NULL))
    {
        *to = container;

        container->next = NULL;
        container->prev = NULL;
    }
    else
    {
        if (LIKELY(iterator->area->address > container->area->address))
        {
            memory_link_prev_next(iterator, container);
        }
        else
        {
            memory_link_next_prev(iterator, container);

            if (UNLIKELY(container->prev == NULL))
            {
                *to = container;
            }
        }
    }
}

/* split memory container at address */
memory_container_t *memory_split(memory_container_t *container, uintptr_t address, size_t size)
{
    memory_container_t *new;

    if (UNLIKELY(container == NULL))
    {
        return NULL;
    }

    /* [ size ][ ... ] */
    if (LIKELY(address == container->area->address))
    {
        new = (memory_container_t *)(address + size);
        new->area = (memory_area_t *)(address + size + sizeof(memory_container_t));

        new->area->address = address + size + MEMORY_OVERHEAD;
        new->area->size = container->area->size - size - MEMORY_OVERHEAD;
        new->freeable = true;
        container->area->size = size;

        memory_link_next_prev(container, new);

        return container;
    }
    /* [ ... ][ size ] */
    else
    {
        new = (memory_container_t *)(address - MEMORY_OVERHEAD);
        new->area = (memory_area_t *)(address - sizeof(memory_area_t));

        new->area->address = address;
        new->area->size = container->area->size - address + container->area->address;
        new->freeable = true;
        container->area->size = address - container->area->address - MEMORY_OVERHEAD;

        memory_link_next_prev(container, new);

        return new;
    }

    return NULL;
}

memory_area_t *memory_alloc(SYSCALL, size_t size, uintptr_t limit, uintptr_t align)
{
    memory_container_t *container = list_free;

    if (LIKELY(align == 0))
    {
        /* iterate through free containers */
        while (LIKELY(container != NULL))
        {
            /* check for address limit */
            if ((LIKELY(limit == 0)) || (UNLIKELY((container->area->address + size) <= limit)))
            {
                /* check for size */
                if (LIKELY(container->area->size >= (size + MEMORY_OVERHEAD)))
                {
                    break;
                }
            }

            container = container->next;
        }

        if (UNLIKELY(container == NULL))
        {
            return NULL;
        }

        memory_container_t *selected = memory_split(container, container->area->address, size);

        memory_move_from_to(selected, &list_free, &list_used);

        return selected->area;
    }
    else
    {
        uintptr_t mod;
        uintptr_t aligned_address;
        uintptr_t skipped_bytes;

        /* iterate through free containers */
        while (LIKELY(container != NULL))
        {
            if (LIKELY((align & (align - 1)) == 0))
            {
                mod = align - 1;
                aligned_address = (container->area->address + mod) & (~mod);
                skipped_bytes = mod - ((container->area->address + mod) & mod);
            }
            else
            {
                mod = container->area->address + align - 1;
                aligned_address = (mod - (mod % align));
                skipped_bytes = (align - 1 - (mod % align));
            }

            /* check for address limit */
            if ((LIKELY(limit == 0)) || (UNLIKELY((aligned_address + size) <= limit)))
            {
                /* check for size and alignment */
                if (UNLIKELY(container->area->size == size))
                {
                    if (LIKELY(aligned_address == container->area->address))
                    {
                        memory_move_from_to(container, &list_free, &list_used);

                        return container->area;
                    }
                }
                else if (LIKELY(container->area->size > size))
                {
                    /* one end of the requested area is located on an edge of the free area: split */
                    if ((UNLIKELY(aligned_address == container->area->address)) || (UNLIKELY((aligned_address + size) == (container->area->address + container->area->size))))
                    {
                        if (LIKELY((container->area->size - skipped_bytes - MEMORY_OVERHEAD) >= size))
                        {
                            memory_container_t *selected = memory_split(container, aligned_address, size);

                            memory_move_from_to(selected, &list_free, &list_used);

                            return selected->area;
                        }
                    }
                    /* neither end of the requested area is located on an edge of the free area: split twice */
                    else
                    {
                        if ((LIKELY(skipped_bytes >= MEMORY_OVERHEAD)) && (LIKELY((container->area->size - skipped_bytes - (2 * MEMORY_OVERHEAD)) >= size)))
                        {
                            memory_container_t *selected = memory_split(container, aligned_address, size);

                            if (UNLIKELY(selected != memory_split(selected, aligned_address, size)))
                            {
                                return NULL;
                            }

                            memory_move_from_to(selected, &list_free, &list_used);

                            return selected->area;
                        }
                    }
                }
            }

            container = container->next;
        }

        if (UNLIKELY(container == NULL))
        {
            return NULL;
        }
    }

    return NULL;
}

void memory_free(SYSCALL, memory_area_t *area)
{
    memory_container_t *container = list_used;

    /* search for allocated area */
    while (LIKELY(container != NULL))
    {
        if ((UNLIKELY(container->area->address == area->address)) && (LIKELY(container->area->size == area->size)))
        {
            break;
        }

        container = container->next;
    }

    /* not allocated or not freeable */
    if ((UNLIKELY(container == NULL)) || (UNLIKELY(container->freeable == false)))
    {
        /* TODO kill task */
        printf("%[Attempted to free invalid area!%]", 12);

        while (1)
        {
            __asm__ (
                "cli \n"
                "hlt \n"
            );            
        }
    }

    memory_move_from_to(container, &list_used, &list_free);

    /* try to merge with next container */
    if ((LIKELY(container->next != NULL)) && (LIKELY(((uintptr_t)container + MEMORY_OVERHEAD) == container->area->address)) && (UNLIKELY((container->next->area->address + container->next->area->size) == (uintptr_t)container)))
    {
        container->next->area->size += MEMORY_OVERHEAD + container->area->size;
        container->next->prev = container->prev;

        if (LIKELY(container->prev != NULL))
        {
            container->prev->next = container->next;
        }

        container->area->size = container->next->area->size;
        container = container->next;
    }

    /* try to merge with previous container */
    if ((LIKELY(container->prev != NULL)) && (LIKELY(((uintptr_t)container->prev + MEMORY_OVERHEAD) == container->prev->area->address)) && (UNLIKELY((container->area->address + container->area->size) == (uintptr_t)container->prev)))
    {
        container->area->size += MEMORY_OVERHEAD + container->prev->area->size;

        if (LIKELY(container->prev->prev != NULL))
        {
            container->prev->prev->next = container;
        }

        container->prev = container->prev->prev;
    }
}

void memory_init(multiboot_memory_t *memory, uint32_t length)
{
    void memory_check_area(memory_area_t *area)
    {
        /* can't allocate memory structures */
        if (UNLIKELY(area == NULL))
        {
            printf("%[No memory available!%]", 12);

            while (1)
            {
                __asm__ (
                    "cli \n"
                    "hlt \n"
                );
            }
        }
    }

    memory_container_t *container = NULL;
    memory_area_t *area = NULL;
    multiboot_memory_t *memory_end = (multiboot_memory_t *)((uintptr_t)memory + (uintptr_t)length);

    while (LIKELY(memory < memory_end))
    {
        /* resolve any conflicts with the kernel's position in RAM */
        if (UNLIKELY(memory->address >= (uintptr_t)kernel_area_begin))
        {
            if (LIKELY(memory->address < (uintptr_t)kernel_area_end))
            {
                /* kernel contains whole memory block: skip */
                if (UNLIKELY((memory->address + memory->length) <= (uintptr_t)kernel_area_end))
                {
                    memory = (multiboot_memory_t *)((uintptr_t)memory + memory->size + 4);

                    continue;
                }
                /* kernel contains memory block's beginning: move memory block's beginning */
                else
                {
                    memory->length -= (uintptr_t)kernel_area_end - memory->address;
                    memory->address += (uintptr_t)kernel_area_end - memory->address;
                }
            }
        }
        else
        {
            if (UNLIKELY((memory->address + memory->length) > (uintptr_t)kernel_area_begin))
            {
                /* memory block contains whole kernel: split */
                if (UNLIKELY((memory->address + memory->length) > (uintptr_t)kernel_area_end))
                {
                    /* second memory block begins at kernel's end */
                    area = (memory_area_t *)memory_alloc(CALL_AS_NON_SYSCALL, MEMORY_OVERHEAD, 0, 0);

                    memory_check_area(area);

                    container = (memory_container_t *)area->address;
                    area = (memory_area_t *)(area->address + sizeof(memory_container_t));

                    container->area = area;
                    container->next = NULL;
                    container->prev = NULL;
                    area->address = (uintptr_t)kernel_area_end;
                    area->size = memory->address + memory->length - (uintptr_t)kernel_area_end;

                    if (LIKELY(memory->type == 1))
                    {
                        container->freeable = true;

                        memory_move_from_to(container, NULL, &list_free);
                    }
                    else
                    {
                        /* TODO: panic? */
                        container->freeable = false;

                        memory_move_from_to(container, NULL, &list_used);
                    }

                    /* first memory block is truncated */
                    memory->length -= memory->address + memory->length - (uintptr_t)kernel_area_begin;
                }
                /* kernel contains memory block's end: truncate */
                else
                {
                    memory->length -= memory->address + memory->length - (uintptr_t)kernel_area_begin;
                }
            }
        }

        if (UNLIKELY(memory->type == 1))
        {
            /* skip first 4 KiB */
            if (UNLIKELY(memory->address == 0x0000))
            {
                memory->address = 0x1000;
                memory->length -= 0x1000;
            }

            container = (memory_container_t *)(uintptr_t)memory->address;
            area = (memory_area_t *)(uintptr_t)(memory->address + sizeof(memory_container_t));

            /* add memory to free list */
            container->area = area;
            container->freeable = true;
            container->next = NULL;
            container->prev = NULL;
            area->address = memory->address + MEMORY_OVERHEAD;
            area->size = memory->length - MEMORY_OVERHEAD;

            memory_move_from_to(container, NULL, &list_free);
        }
        else
        {
            area = (memory_area_t *)memory_alloc(CALL_AS_NON_SYSCALL, MEMORY_OVERHEAD, 0, 0);

            memory_check_area(area);

            container = (memory_container_t *)area->address;
            area = (memory_area_t *)(area->address + sizeof(memory_container_t));

            /* add memory to used list */
            container->area = area;
            container->freeable = false;
            container->next = NULL;
            container->prev = NULL;
            area->address = memory->address;
            area->size = memory->length;

            memory_move_from_to(container, NULL, &list_used);
        }

        memory = (multiboot_memory_t *)((uintptr_t)memory + memory->size + 4);
    }

    /* add used memory block for kernel's position in RAM */
    area = (memory_area_t *)memory_alloc(CALL_AS_NON_SYSCALL, MEMORY_OVERHEAD, 0, 0);

    memory_check_area(area);

    container = (memory_container_t *)area->address;
    area = (memory_area_t *)(area->address + sizeof(memory_container_t));

    container->area = area;
    container->freeable = false;
    container->next = NULL;
    container->prev = NULL;
    area->address = (uintptr_t)kernel_area_begin;
    area->size = (uintptr_t)kernel_area_end - (uintptr_t)kernel_area_begin;

    memory_move_from_to(container, NULL, &list_used);

    memory_container_t *used, *free;

    /* some used memory blocks may overlap with free memory blocks, resize them */
    for (used = list_used; LIKELY(used != NULL); used = used->next)
    {
        for (free = list_free; LIKELY(free != NULL); free = free->next)
        {
            if (free->area->address >= used->area->address)
            {
                if (free->area->address < (used->area->address + used->area->size))
                {
                    /* used memory block contains whole free memory block: skip */
                    if ((free->area->address + free->area->size) <= (used->area->address + used->area->size))
                    {
                        continue;
                    }
                    /* used memory block contains free memory block's beginning: move free memory block's beginning */
                    else
                    {
                        free->area->address += used->area->address + used->area->size - free->area->address;
                        free->area->size -= used->area->address + used->area->size - free->area->address;
                    }
                }
            }
            else
            {
                if ((free->area->address + free->area->size) > used->area->address)
                {
                    /* free memory block contains whole used memory block: split */
                    if ((free->area->address + free->area->size) > (used->area->address + used->area->size))
                    {
                        /* second free memory block begins at used memory block's end */
                        area = (memory_area_t *)memory_alloc(CALL_AS_NON_SYSCALL, MEMORY_OVERHEAD, 0, 0);

                        memory_check_area(area);

                        container = (memory_container_t *)area->address;
                        area = (memory_area_t *)(area->address + sizeof(memory_container_t));

                        container->area = area;
                        container->next = NULL;
                        container->prev = NULL;
                        container->freeable = true;
                        area->address = used->area->address + used->area->size;
                        area->size = free->area->address + free->area->size - used->area->address - used->area->size;

                        memory_move_from_to(container, NULL, &list_free);

                        /* first free memory block is truncated */
                        free->area->size -= free->area->address + free->area->size - used->area->address;
                    }
                    /* used memory block contains free memory block's end: truncate */
                    else
                    {
                        free->area->size -= free->area->address + free->area->size - used->area->address;
                    }
                }
            }
        }
    }

    /* finally, sort the lists */
    memory_sort(&list_free);
    memory_sort(&list_used);
}
