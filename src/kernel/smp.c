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

#include <stddef.h>

#include "smp.h"
#include "console.h"

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

void smp_init(void)
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

    smp_config_entry_t *iterator = (smp_config_entry_t *)((uintptr_t)config + sizeof(smp_config_table_t));
    uint16_t entries;
    uint16_t cpus = 0;

    for (entries = 0; entries < config->entry_count; entries++)
    {
        if (iterator->entry_type == 0)
        {
            smp_config_processor_t *cpu = (smp_config_processor_t *)iterator;

            if (cpu->flags & SMP_FLAG_ACTIVE)
            {
                printf("CPU #%hu %[enabled%]\n", entries, 10);
            }
            else
            {
                printf("CPU #%hu %[disabled%]\n", entries, 12);
            }

            if (cpu->flags & SMP_FLAG_BOOT)
            {
                printf("Bootprocessor properties:\n");
                printf("APIC ID      : %03hhu\n", cpu->local_apic_id);
                printf("APIC Version : %03hhu\n", cpu->local_apic_version);
                printf("Feature flags: %#0.8x\n", cpu->feature_flags);
                printf("Local APIC   : %#0p\n", config->local_apic);
            }

            cpu++;
            cpus++;

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

            printf("IO APIC #%03hhu (Version %03hhu) with flags %#0.2hhx located at %p\n", io->id, io->version, io->flags, io->address);

            iterator++;
        }
        else if (iterator->entry_type == 3)
        {
            smp_config_io_assignment_t *assign = (smp_config_io_assignment_t *)iterator;

            printf("IO Assignment of Type '%s' with flag %#0.4hx from %03hhu->%03hhu to %03hhu->%03hhu\n", (assign->type & 2 ? (assign->type & 1 ? "ExtINT" : "SMI") : (assign->type & 1) ? "NMI" : "INT"), assign->flag, assign->src_id, assign->src_irq, assign->dest_id, assign->dest_int);

            iterator++;
        }
        else if (iterator->entry_type == 4)
        {
            smp_config_local_assignment_t *assign = (smp_config_local_assignment_t *)iterator;

            printf("Local Assignment of Type '%s' with flag %#0.4hx from %03hhu->%03hhu to %03hhu->%03hhu\n", (assign->type & 2 ? (assign->type & 1 ? "ExtINT" : "SMI") : (assign->type & 1) ? "NMI" : "INT"), assign->flag, assign->src_id, assign->src_irq, assign->dest_id, assign->dest_int);

            iterator++;
        }
        else
        {
            printf("%[%u%]\n", 12, iterator->entry_type);
            while (1);
        } 
    }

    printf("%[%hu%] cpu%s found\n", 9, cpus, ((cpus == 1) ? "" : "s"));
}
