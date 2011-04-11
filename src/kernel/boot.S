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

/* config area start */

#define LOADER_USE_BSS 1

.set LOADER_STACK_SIZE, 0x1000
.set LOADER_MAP_SIZE, 0x01000000 /* don't use values above 0x40000000 */
.set LOADER_STACK_ADDRESS, 0x300000
.set LOADER_PML4_ADDRESS, 0x300000

/* config area end */

.section .multiboot

.global LOADER_STACK_SIZE
.global LOADER_MAP_SIZE
.global LOADER_PML4_ADDRESS

#define LOADER_MB_MAGIC 0x1badb002
#define LOADER_MB_FLAGS 0x0003
#define LOADER_MB_CHECKSUM -(LOADER_MB_MAGIC + LOADER_MB_FLAGS)

.align 4

/* multiboot header */
.int LOADER_MB_MAGIC
.int LOADER_MB_FLAGS
.int LOADER_MB_CHECKSUM

/* unused */
.int 0
.int 0
.int 0
.int 0
.int 0

/* graphics mode 1, 80x25 chars, automatically select depth */
.int 1
.int 80
.int 25
.int 0

.ifgt LOADER_USE_BSS
.section .loader, "aw", @nobits

/* provide extra space for stack and page tables if LOADER_USE_BSS != 0 */

.align 0x1000
.space LOADER_STACK_SIZE
.set LOADER_STACK_ADDRESS, .
.set LOADER_PML4_ADDRESS, .
.space 0xb000 /* its not yet over */
.endif

#if LOADER_MAP_SIZE < LOADER_STACK_ADDRESS
.set LOADER_MAP_SIZE, LOADER_STACK_ADDRESS
#endif

.section .text
.code32

/* entrypoint */
.global loader_entry
loader_entry:
    /* disable interrupts, create stack and load GDT */
    cli
    movl $LOADER_STACK_ADDRESS, %esp
    lgdt (loader_gdt_descriptor)

    /* transfer control to known 32 bit segment */
    ljmp $8, $loader_switch_to_i386

.local loader_switch_to_i386
loader_switch_to_i386:
    /* load segment registers */
    movw $0, %dx
    movw $16, %cx
    movw %dx, %fs
    movw %dx, %gs
    movw %cx, %ds
    movw %cx, %es
    movw %cx, %ss

    call loader_init_paging

    call loader_jmp_to_kernel

.local loader_idle
loader_idle:
    cli
    hlt
    jmp loader_idle
