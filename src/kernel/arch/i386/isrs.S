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

.macro isr_stub nr
    .global isr_stub_\nr
    isr_stub_\nr:
        pushl $0
        pushl $\nr
        jmp isr_stub_common
.endm

.macro isr_stub_error nr
    .global isr_stub_\nr
    isr_stub_\nr:
        pushl $\nr
        jmp isr_stub_common
.endm

.macro isr_named_stub name, nr
    .global isr_stub_\name
    isr_stub_\name:
        pushl $0
        pushl $\nr
        jmp isr_stub_common
.endm

isr_stub 0
isr_stub 1
isr_stub 2
isr_stub 3
isr_stub 4
isr_stub 5
isr_stub 6
isr_stub 7
isr_stub_error 8
isr_stub_error 10
isr_stub_error 11
isr_stub_error 12
isr_stub_error 13
isr_stub_error 14
isr_stub 16
isr_stub_error 17
isr_stub 18
isr_stub 19

isr_stub 32
isr_stub 33
isr_stub 34
isr_stub 35
isr_stub 36
isr_stub 37
isr_stub 38
isr_stub 39
isr_stub 40
isr_stub 41
isr_stub 42
isr_stub 43
isr_stub 44
isr_stub 45
isr_stub 46
isr_stub 47

isr_named_stub null, 0xdead
isr_named_stub system, 81

.global isr_stub_common
isr_stub_common:
    pushl %eax
    pushl %ecx
    pushl %edx
    pushl %ebx
    pushl %esi
    pushl %edi
    pushl %ebp

    pushl %esp
    call isr_handler
    movl %eax, %esp

    popl %ebp
    popl %edi
    popl %esi
    popl %ebx
    popl %edx
    popl %ecx
    popl %eax

    add $8, %esp
    iret
