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
.code64

.macro isr_stub nr
    .global isr_stub_\nr
    isr_stub_\nr:
    pushq $0
    pushq $\nr
    jmp isr_stub_common
.endm

.macro isr_stub_error nr
    .global isr_stub_\nr
    isr_stub_\nr:
    pushq $\nr
    jmp isr_stub_common
.endm

.macro isr_named_stub name,nr
    .global isr_stub_\name
    isr_stub_\name:
    pushq $0
    pushq $\nr
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

.global isr_stub_common
isr_stub_common:
    pushq %rax
    pushq %rcx
    pushq %rdx
    pushq %rbx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    movq %rsp, %rdi
    call isr_handler
    movq %rax, %rsp

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rbx
    popq %rdx
    popq %rcx
    popq %rax

    add $16, %rsp
    iretq

.global isr_stub_system
isr_stub_system:
    pushq %rbx

    .extern syscall_table
    leaq syscall_table(,%rdi,8), %rbx

    .extern syscall_table_end
    cmpq $syscall_table_end, %rbx
    jae isr_invalid_system

isr_do_system:
    call *(%rbx)

    popq %rbx
    iretq

isr_invalid_system:
    .extern syscall_invalid
    movq $syscall_invalid, %rbx
    jmp isr_do_system
