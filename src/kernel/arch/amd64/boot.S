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

.section .multiboot

.section .text
.code32

/* TODO: check requirements */

.global loader_init_paging
loader_init_paging:
    /* create page tables */
    movl $LOADER_PML4_ADDRESS, %edi

    /* fill in PML4 */
    movl $(LOADER_PML4_ADDRESS + 0x100f), %eax
    stosl
    movl $0x03ff, %ecx
    xorl %eax, %eax
    rep stosl

    call loader_map_fill

    /* fill in PDP */
    movl $(LOADER_PML4_ADDRESS + 0x200f), %eax
    stosl
    movl $0x03ff, %ecx
    xorl %eax, %eax
    rep stosl

    call loader_map_fill

    /* NOTE: the above two structures don't calculate the number of entries */
    /* this limits LOADER_MAP_SIZE to 0x40000000 (1 GiB), which is enough */

    /* calculate number of entries to fill in PD */
    xorl %edx, %edx
    movl $(LOADER_MAP_SIZE + 0x1fffff), %eax
    movl $0x200000, %ecx
    divl %ecx

    movl %eax, %ecx
    movl $(LOADER_PML4_ADDRESS + 0x300f), %esi

.local loader_map_loop_2mb
loader_map_loop_2mb:

    /* fill in PD */
    movl %esi, %eax
    stosl
    xorl %eax, %eax
    stosl
    addl $0x1000, %esi
    subl $1, %ecx
    jnz loader_map_loop_2mb

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
    xor %eax, %eax
    stosl
    addl $0x1000, %esi
    subl $1, %ecx
    jnz loader_map_loop_4kb

    call loader_map_fill

    /* activate PAE and PGE */
    movl %cr4, %eax
    orl $0x00a0, %eax
    movl %eax, %cr4

    /* load page tables */
    movl $LOADER_PML4_ADDRESS, %eax
    movl %eax, %cr3

    /* NOTE: paging is activated in loader_jmp_to_kernel */
    /* anything else would give us a headache, due to stack misalignment */

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
    divl %ecx

    /* and fill them with zeroes */
    movl %eax, %ecx
    xorl %eax, %eax
    rep stosl

    ret

.global loader_jmp_to_kernel
loader_jmp_to_kernel:

    /* activate long mode, long mode still inactive */
    movl $0xc0000080, %ecx
    rdmsr
    orl $0x0100, %eax
    wrmsr

    /* activate paging, long mode now activate */
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0

    /* 32 bit mapping? */
    /* transfer control to known 64 bit segment */
    ljmp $24, $loader_switch_to_amd64

.local loader_switch_to_amd64
loader_switch_to_amd64:

.code64
    /* reload gdt (descriptor changed) */
    lgdt (loader_gdt_descriptor)

    /* reload page tables (address size changed) */
    movq $LOADER_PML4_ADDRESS, %rax
    movq %rax, %cr3

    /* call void kernel_entry(multiboot_info_t *) */
    movq %rbx, %rdi
    call kernel_entry

    ret

.section .data

.align 1

.global loader_gdt
loader_gdt:
.int 0x00000000 /* null */
.int 0x00000000
.int 0x0000ffff /* 32-bit code */
.int 0x00cf9a00
.int 0x0000ffff /* 32-bit data */
.int 0x00cf9200
.int 0x00000000 /* 64-bit code */
.int 0x00a09a00

.global loader_gdt_descriptor
loader_gdt_descriptor:
.short loader_gdt_descriptor - loader_gdt - 1
.quad loader_gdt
