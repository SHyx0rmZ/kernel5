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

.section .smpinit
.code16

    ljmp $0, $smp_start

.global smp_initializer_status
.global smp_initializer_stack
.global smp_initializer_context
.global smp_initializer_kernel

smp_initializer_status:
.byte 0
smp_initializer_stack:
.quad 0
smp_initializer_context:
.quad 0
smp_initializer_kernel:
.quad 0

.global smp_start
smp_start:

    /* disable interrupts, create stack and load GDT */
    cli
    movb $1, (smp_initializer_status)
    lgdt (smp_initializer_gdt_descriptor)

    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0

    /* transfer control to known 32 bit segment */
    ljmp $8, $smp_switch_to_i386

.local smp_switch_to_i386
smp_switch_to_i386:

.code32

    /* load segment registers */
    movw $0, %dx
    movw $16, %cx
    movw %dx, %fs
    movw %dx, %gs
    movw %cx, %ds
    movw %cx, %es
    movw %cx, %ss

    movl (smp_initializer_stack), %esp

    call smp_jmp_to_kernel

.local smp_idle
smp_idle:

    cli
    hlt
    jmp smp_idle
