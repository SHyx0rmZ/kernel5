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

void memory_move_from_to(memory_container_t *container, memory_container_t **from, memory_container_t **to)
{
    if (container->prev == NULL)
    {
        *from = container->next;
    }
    else
    {
        container->prev->next = container->next;
    }

    if (container->next != NULL)
    {
        container->next->prev = container->prev;
    }

    memory_container_t *iterator = *to;

    while ((iterator != NULL) && (iterator->next != NULL) && (iterator->area->address > container->area->address))
    {
        iterator = iterator->next;
    }

    if(iterator == NULL)
    {
        *to = container;

        container->next = NULL;
        container->prev = NULL;
    }
    else
    {
        container->prev = iterator;
        container->next = iterator->next;
        iterator->next = container;
    }
}

/* FIXME: this will return invalid values if align is not a power of 2 or 0 */
memory_area_t *memory_alloc(size_t size, uintptr_t limit, uintptr_t align)
{
    memory_container_t *container = list_free;

    /* iterate through free containers */
    while (container != NULL)
    {
        /* check for address limit */
        if ((limit == 0) || ((container->area->address + size) <= limit))
        {
            /* check for size */
            if (container->area->size >= size)
            {
                /* check for alignment */
                if ((align == 0) || ((((container->area->address + align - 1) & (~(align - 1))) + size) <= (container->area->address + container->area->size)))
                {
                    /* everything's fine, go ahead */
                    break;
                }
            }
        }

        container = container->next;
    }

    /* no container available */
    if(container == NULL)
    {
        return NULL;
    }

    memory_container_t *selected = container;

    if(align == 0)
    {
        /* hey, we can fit another container's description in here */
        /* go ahead and split the damn thing into two */
        if ((size + sizeof(memory_container_t) + sizeof(memory_area_t)) <= container->area->size)
        {
            selected = (memory_container_t *)container->area->address;
            selected->area = (memory_area_t *)(container->area->address + sizeof(memory_container_t));

            selected->area->address = container->area->address + sizeof(memory_container_t) + sizeof(memory_area_t);
            selected->area->size = size;

            selected->prev = container->prev;
            selected->next = container;
            container->prev = selected;

            container->area->address += size + sizeof(memory_container_t) + sizeof(memory_area_t);
            container->area->size -= size + sizeof(memory_container_t) + sizeof(memory_area_t);
        }
    }
    else
    {
        /* FIXME: space potentially wasted */
        /* we need to align the memory */
        uintptr_t address = ((container->area->address + align - 1) & (~(align - 1)));

        selected->area->size -= address - selected->area->address;
        selected->area->address = address;
    }

    memory_move_from_to(selected, &list_free, &list_used);

    return selected->area;
}

void memory_free(memory_area_t *area)
{
    memory_container_t *container = list_used;

    while (container != NULL)
    {
        if (container->area == area)
        {
            break;
        }

        container = container->next;
    }

    if ((container == NULL) || (container->freeable == false))
    {
        while (1)
        {
            __asm__ (
                "cli \n"
                "hlt \n"
            );            
        }
    }

    memory_move_from_to(container, &list_used, &list_free);

    if((container->prev != NULL) && ((uintptr_t)(container + sizeof(memory_container_t) + sizeof(memory_area_t) + container->area->size) == container->prev->area->address))
    {
        container->prev->area->address = (uintptr_t)container;
        container->prev->area->size += sizeof(memory_container_t) + sizeof(memory_area_t) + container->area->size;
        container->prev->next = container->next;

        if(container->next != NULL)
        {
            container->next->prev = container->prev;
        }
    }
}

void memory_init(multiboot_memory_t *memory, uint32_t length)
{
    multiboot_memory_t *memory_end = memory + length;

    while (memory < memory_end)
    {
        memory_container_t *container = NULL;
        memory_area_t *area = NULL;

        if (memory->type == 1)
        {
            if(memory->address == 0x0000)
            {
                memory->address = 0x1000;
            }

            container = (memory_container_t *)(uintptr_t)memory->address;
            area = (memory_area_t *)(uintptr_t)(memory->address + sizeof(memory_container_t));

            container->area = area;
            container->freeable = true;
            container->next = container;
            container->prev = container;
            area->address = memory->address + sizeof(memory_container_t) + sizeof(memory_area_t);
            area->size = memory->length - sizeof(memory_container_t) - sizeof(memory_area_t);

            memory_move_from_to(container, NULL, &list_free);
        }
        else
        {
            container = (memory_container_t *)memory_alloc(sizeof(memory_container_t), 0, 0)->address;
            area = (memory_area_t *)memory_alloc(sizeof(memory_area_t), 0, 0)->address;

            if ((container == NULL) || (area == NULL))
            {
                while (1)
                {
                    __asm__ (
                        "cli \n"
                        "hlt \n"
                    );
                }
            }

            container->area = area;
            container->freeable = false;
            container->next = container;
            container->prev = container;
            area->address = memory->address;
            area->size = memory->size;

            memory_move_from_to(container, NULL, &list_used);
        }

        memory = (multiboot_memory_t *)((uintptr_t)memory + memory->size + 4);
    }
}