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

    jmp smp_start

.code32

.global smp_jmp_to_kernel
smp_jmp_to_kernel:

    movl (smp_initializer_context), %eax
    movl %eax, %cr3

    movl (smp_initializer_kernel), %eax
    call *%eax

.global smp_initializer_gdt
smp_initializer_gdt:
.int 0x00000000
.int 0x00000000
.int 0x0000ffff
.int 0x00cf9a00
.int 0x0000ffff
.int 0x00cf9200

.global smp_initializer_gdt_descriptor
smp_initializer_gdt_descriptor:
.short smp_initializer_gdt_descriptor - smp_initializer_gdt - 1
.int smp_initializer_gdt
