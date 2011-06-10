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

#include "smp.h"
#include "console.h"
#include "apic.h"
#include "memory.h"
#include "cpu.h"

static cpu_t *smp_processors = NULL;
static uint32_t smp_processor_count = 0;

extern void smp_initializer_begin(void);
extern void smp_initializer_end(void);
extern void kernel_entry_smp(void);

/* find the floating structure */
smp_floating_t *smp_find_floating(uintptr_t start, size_t size)
{
    smp_floating_t *floating = (smp_floating_t *)start;

    while ((uintptr_t)floating < (start + size))
    {
        /* check for the signature */
        if (floating->signature == 0x5f504d5f)
        {
            uint8_t checksum = 0;
            uint8_t *pointer = (uint8_t *)floating;
            uint8_t i;

            /* calculate checksum */
            for (i = 0; i < sizeof(smp_floating_t); i++)
            {
                checksum += pointer[i];
            }

            /* valid */
            if (checksum == 0)
            {
                return floating;
            }
        }

        floating++;
    }

    return NULL;
}

void smp_init(paging_context_t *context)
{
    smp_floating_t *floating;

    /* search for floating structure */
    floating = smp_find_floating(0, 0x400);

    if (floating == NULL)
    {
        floating = smp_find_floating(0x9fc00, 0x400);
    }

    if (floating == NULL)
    {
        floating = smp_find_floating(0xe0000, 0x20000);
    }

    if ((floating == NULL) || (floating->config_table == 0) || (floating->features[0] != 0))
    {
        /* TODO: Hang / continue with one cpu */

        printf("SMP NOT SUPPORTED");

        while(1);
    }

    smp_config_table_t *config = (smp_config_table_t *)(uintptr_t)floating->config_table;

    if (config->signature != 0x504d4350)
    {
        printf("SMP SIGNATURE INVALID");

        while(1);
    }

    paging_map(context, config->local_apic, config->local_apic, PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_CACHEDISABLE_BIT);

    local_apic_init(config->local_apic);

    {
        smp_config_entry_t *iterator = (smp_config_entry_t *)((uintptr_t)config + sizeof(smp_config_table_t));
        uint16_t entries;

        for (entries = 0; entries < config->entry_count; entries++)
        {
            if (iterator->entry_type == 0)
            {
                smp_config_processor_t *cpu = (smp_config_processor_t *)iterator;

                if (cpu->flags & SMP_FLAG_ACTIVE)
                {
                    smp_processor_count++;
                }

                cpu++;

                iterator = (smp_config_entry_t *)cpu;
            }
            else if (iterator->entry_type <= 4)
            {
                iterator++;
            }
            else
            {
                printf("%[Unknown SMP type '%u'!%]", 12, iterator->entry_type);
                while(1);
            }
        }

        memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, sizeof(cpu_t) * smp_processor_count, 0, 0);

        if (area.size == 0)
        {
            printf("%[Ran out of memory while initializing SMP!%]", 12);
            while(1);
        }

        smp_processors = (cpu_t *)area.address;

        memset(smp_processors, 0, area.size);
    }

    {
        smp_config_entry_t *iterator = (smp_config_entry_t *)((uintptr_t)config + sizeof(smp_config_table_t));
        uint16_t entries;
        uint16_t cpu_index = 0;
        uint16_t cpu_id = 0;

        for (entries = 0; entries < config->entry_count; entries++)
        {
            if (iterator->entry_type == 0)
            {
                smp_config_processor_t *cpu = (smp_config_processor_t *)iterator;

                if (cpu->flags & SMP_FLAG_ACTIVE)
                {
                    printf("CPU #%hu %[enabled%]\n", cpu_id, 10);

                    smp_processors[cpu_index].proc_id = cpu_id;
                    smp_processors[cpu_index].apic_id = cpu->local_apic_id;
                    smp_processors[cpu_index].boot = (cpu->flags & SMP_FLAG_BOOT) ? true : false;
                    smp_processors[cpu_index].up = (cpu->flags & SMP_FLAG_BOOT) ? true : false;
                    smp_processors[cpu_index].context = context;
                    smp_processors[cpu_index].thread = NULL;

                    cpu_index++;
                }
                else
                {
                    printf("CPU #%hu %[disabled%]\n", cpu_id, 12);
                }

                cpu_id++;
                cpu++;

                iterator = (smp_config_entry_t *)cpu;
            }
            else if (iterator->entry_type == 1)
            {
                smp_config_bus_t *bus = (smp_config_bus_t *)iterator;

                printf("Bus #%03hhu is %c%c%c%c%c%c\n", bus->id, bus->type[0], bus->type[1], bus->type[2], bus->type[3], bus->type[4], bus->type[5]);

                iterator++;
            }
            else if (iterator->entry_type == 2)
            {
                smp_config_io_apic_t *io = (smp_config_io_apic_t *)iterator;

                printf("IO APIC #%03hhu (version %03hhu) with flags %#0.2hhx located at %p\n", io->id, io->version, io->flags, io->address);

                if (io->flags & 1)
                {
                    paging_map(context, io->address, io->address, PAGE_PRESENT_BIT | PAGE_WRITE_BIT | PAGE_CACHEDISABLE_BIT);
                }

                iterator++;
            }
            else if (iterator->entry_type == 3)
            {
                smp_config_io_assignment_t *assign = (smp_config_io_assignment_t *)iterator;

                printf("IO Assignment of type '%s' with flag %#0.4hx from %03hhu->%03hhu to %03hhu->%03hhu\n", (assign->type & 2 ? (assign->type & 1 ? "ExtINT" : "SMI") : (assign->type & 1) ? "NMI" : "INT"), assign->flag, assign->src_id, assign->src_irq, assign->dest_id, assign->dest_int);

                iterator++;
            }
            else if (iterator->entry_type == 4)
            {
                smp_config_local_assignment_t *assign = (smp_config_local_assignment_t *)iterator;

                printf("Local Assignment of type '%s' with flag %#0.4hx from %03hhu->%03hhu to %03hhu->%03hhu\n", (assign->type & 2 ? (assign->type & 1 ? "ExtINT" : "SMI") : (assign->type & 1) ? "NMI" : "INT"), assign->flag, assign->src_id, assign->src_irq, assign->dest_id, assign->dest_int);

                iterator++;
            }
            else
            {
                printf("%[Unkown SMP type '%u'!%]\n", 12, iterator->entry_type);
                while (1);
            }
        }

        printf("%[%hu%] cpu%s found\n", 9, cpu_id, ((cpu_id == 1) ? "" : "s"));
    }

    if (smp_processor_count == 1)
    {
        return;
    }

    __asm__ __volatile__ ("sti");
    __asm__ __volatile__ ("hlt");

    local_apic_write(0x310, 0);
    local_apic_write(0x300, 0x0cc500);

    __asm__ __volatile__ ("hlt");

    local_apic_write(0x310, 0);
    local_apic_write(0x300, 0x0c8500);

    __asm__ __volatile__ ("hlt");

    volatile smp_startup_data_t *startup_data = (smp_startup_data_t *)SMP_STARTUP_ADDRESS;

    {
        uint32_t i;

        for (i = 0; i < smp_processor_count; i++)
        {
            if (smp_processors[i].boot == true)
            {
                continue;
            }

            memcpy((void *)startup_data, smp_initializer_begin, smp_initializer_end - smp_initializer_begin);

            startup_data->status = 0;
            startup_data->context = (uintptr_t)smp_processors[i].context->cr3;
            startup_data->kernel = (uintptr_t)kernel_entry_smp;

            memory_area_t area = memory_alloc(CALL_AS_NON_SYSCALL, 0x1000, 0, 0);

            if (area.size == 0)
            {
                printf("%[Ran out of memory while initializing SMP!%]", 12);
                while(1);
            }

            startup_data->stack = area.address + 0x1000;

            local_apic_write(0x310, smp_processors[i].apic_id << 24);
            local_apic_write(0x300, 0x0600 | (SMP_STARTUP_VECTOR & 0xff));

            __asm__ __volatile__ ("hlt");

            if (!(startup_data->status & SMP_STARTING))
            {
                local_apic_write(0x310, smp_processors[i].apic_id << 24);
                local_apic_write(0x300, 0x0600 | (SMP_STARTUP_VECTOR & 0xff));

                __asm__ __volatile__ ("hlt");
            }

            printf("%u - %u\n", i, startup_data->status);

            while (!(startup_data->status & SMP_RUNNING));

            smp_processors[i].up = true;
        }
    }

    __asm__ __volatile__ ("cli");
}
