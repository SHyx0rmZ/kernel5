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

#ifndef _SMP_H_
#define _SMP_H_

#include <stdint.h>

/* functions */
void smp_init();

/* structs */
typedef struct smp_floating
{
    uint32_t signature;
    uint32_t config_table;
    uint8_t length;
    uint8_t version;
    uint8_t checksum;
    uint8_t features[5];
} __attribute__((packed)) smp_floating_t;

typedef struct smp_config_table
{
    uint32_t signature;
    uint16_t length;
    uint8_t version;
    uint8_t checksum;
    uint8_t oem_id[8];
    uint8_t product_id[12];
    uint32_t oem_table;
    uint16_t oem_table_size;
    uint16_t entry_count;
    uint32_t local_apic;
    uint16_t ext_table_length;
    uint8_t ext_table_checksum;
    uint8_t reserved;
} __attribute__((packed)) smp_config_table_t;

typedef struct smp_config_processor
{
    uint8_t entry_type;
    uint8_t local_apic_id;
    uint8_t local_apic_version;
    uint8_t flags;
    uint8_t signature[4];
    uint32_t feature_flags;
    uint32_t feature_reserved[2];
} __attribute__((packed)) smp_config_processor_t;

typedef struct smp_config_bus
{
    uint8_t entry_type;
    uint8_t id;
    uint8_t type[6];
} __attribute__((packed)) smp_config_bus_t;

typedef struct smp_config_io_apic
{
    uint8_t entry_type;
    uint8_t id;
    uint8_t version;
    uint8_t flags;
    uint32_t address;
} __attribute__((packed)) smp_config_io_apic_t;

typedef struct smp_config_io_assignment
{
    uint8_t entry_type;
    uint8_t type;
    uint16_t flag;
    uint8_t src_id;
    uint8_t src_irq;
    uint8_t dest_id;
    uint8_t dest_int;
} __attribute__((packed)) smp_config_io_assignment_t;

typedef struct smp_config_local_assignment
{
    uint8_t entry_type;
    uint8_t type;
    uint16_t flag;
    uint8_t src_id;
    uint8_t src_irq;
    uint8_t dest_id;
    uint8_t dest_int;
} __attribute__((packed)) smp_config_local_assignment_t;

typedef struct smp_config_entry
{
    uint8_t entry_type;
    uint8_t reserved[7];
} __attribute__((packed)) smp_config_entry_t;

#define SMP_FLAG_ACTIVE 1
#define SMP_FLAG_BOOT   2

#endif
