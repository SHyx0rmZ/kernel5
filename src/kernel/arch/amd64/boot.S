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

.section .multiboot

.section .text
.code32

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

    /* fill in PDP */
    movl $(LOADER_PML4_ADDRESS + 0x200f), %eax
    stosl
    movl $0x03ff, %ecx
    xorl %eax, %eax
    rep stosl

    /* fill in PD (map multiples of 2 MiB) */
    movl $0x008b, %esi
    xorl %edx, %edx
    movl $(LOADER_MAP_SIZE + 0x1fffff), %eax
    movl $0x200000, %ecx
    divl %ecx
    mov %eax, %ecx

.local loader_map_loop
loader_map_loop:
    mov %esi, %eax
    stosl
    xor %eax, %eax
    stosl
    addl $0x00200000, %esi
    subl $1, %ecx
    jnz loader_map_loop

    /* activate PAE and PGE */
    movl %cr4, %eax
    orl $0x00a0, %eax
    movl %eax, %cr4

    /* load page tables */
    movl $LOADER_PML4_ADDRESS, %eax
    movl %eax, %cr3

    /* activate long mode, long mode still inactive */
    movl $0xc0000080, %ecx
    rdmsr
    orl $0x0100, %eax
    wrmsr

    /* activate paging, long mode now activate */
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0

    /* transfer control to known 64 bit segment */
    jmp $24, $loader_switch_to_amd64

.local loader_switch_to_amd64
loader_switch_to_amd64:

.code64
    /* reload gdt (descriptor changed) */
    lgdt (loader_gdt_descriptor)

    /* reload page tables (address size changed) */
    movq $LOADER_PML4_ADDRESS, %rax
    movq %rax, %cr3

.global loader_jmp_to_kernel
loader_jmp_to_kernel:

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
