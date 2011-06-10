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

.section .smpinit
.code16

    jmp smp_start

.code32

.global smp_jmp_to_kernel
smp_jmp_to_kernel:

    movl %cr4, %eax
    orl $0x00a0, %eax
    movl %eax, %cr4

    movl (smp_initializer_context), %eax
    movl %eax, %cr3

    movl $0xc0000080, %ecx
    rdmsr
    orl $0x0100, %eax
    wrmsr

    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0

    ljmp $24, $smp_switch_to_amd64

.local smp_switch_to_amd64
smp_switch_to_amd64:

.code64

    lgdt (smp_initializer_gdt_descriptor)

    movq (smp_initializer_context), %rax
    movq %rax, %cr3

    movq (smp_initializer_stack), %rsp
    movq (smp_initializer_kernel), %rax
    call *%rax

.global smp_initializer_gdt
smp_initializer_gdt:
.int 0x00000000
.int 0x00000000
.int 0x0000ffff
.int 0x00cf9a00
.int 0x0000ffff
.int 0x00cf9200
.int 0x00000000
.int 0x00a09a00

.global smp_initializer_gdt_descriptor
smp_initializer_gdt_descriptor:
.short smp_initializer_gdt_descriptor - smp_initializer_gdt - 1
.quad smp_initializer_gdt
