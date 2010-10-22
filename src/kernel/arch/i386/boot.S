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

.section .text
.code32

.global loader_init_paging
loader_init_paging:
    /* create page tables */
    movl $LOADER_PML4_ADDRESS, %edi

    /* calculate number of entries to fill in PD */
    xorl %edx, %edx
    movl $(LOADER_MAP_SIZE + 0x03fffff), %eax
    movl $0x400000, %ecx
    divl %ecx

    movl %eax, %ecx
    movl $(LOADER_PML4_ADDRESS + 0x100f), %esi

.local loader_map_loop_4mb
loader_map_loop_4mb:

    /* fill in PD */
    movl %esi, %eax
    stosl
    addl $0x1000, %esi
    subl $1, %ecx
    jnz loader_map_loop_4mb

    call loader_map_fill

    /* calculate number of entries to fill in PT */
    xorl %edx, %edx
    movl $(LOADER_MAP_SIZE + 0x0fff), %eax
    movl $0x1000, %ecx
    divl %ecx

    movl %eax, %ecx
    movl $0x000b, %esi

.local loader_map_loop_4kb
loader_map_loop_4kb:

    /* fill in PT */
    mov %esi, %eax
    stosl
    addl $0x1000, %esi
    subl $1, %ecx
    jnz loader_map_loop_4kb

    call loader_map_fill

    /* activate PGE */
    movl %cr4, %eax
    orl $0x0080, %eax
    movl %eax, %cr4

    /* load page tables */
    movl $LOADER_PML4_ADDRESS, %eax
    movl %eax, %cr3

    /* activate paging */
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0
    
    /* fill in PT */
    ret

.local loader_map_fill
loader_map_fill:

    /* calculate the number of entries not written in this table */
    xorl %edx, %edx
    movl %edi, %eax
    addl $0x0fff, %eax
    andl $0xfffff000, %eax
    subl %edi, %eax
    movl $4, %ecx
    div %ecx
    movl %eax, %ecx

    /* and fill them with zeroes */
    xorl %eax, %eax
    rep stosl

    ret

.global loader_jmp_to_kernel
loader_jmp_to_kernel:

    /* call void kernel_entry(multiboot_info_t *) */
    push %ebx
    call kernel_entry

    ret

.section .data

.align 1

.global loader_gdt
loader_gdt:
.int 0x00000000 /* null */
.int 0x00000000
.int 0x0000ffff /* 32 bit code */
.int 0x00cf9a00
.int 0x0000ffff /* 32 bit data */
.int 0x00cf9200

.global loader_gdt_descriptor
loader_gdt_descriptor:
.short loader_gdt_descriptor - loader_gdt - 1
.int loader_gdt
