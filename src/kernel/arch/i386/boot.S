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
	# create page tables
	movl $LOADER_PML4_ADDRESS, %edi

	# fill in PD
	
	# fill in PT
	ret

.global loader_jmp_to_kernel
loader_jmp_to_kernel:
	# call void kernel_entry(multiboot_info_t *)
	push %ebx
	call kernel_entry
	ret

.section .data

.align 1

.global loader_gdt
loader_gdt:
.int 0x00000000 # null
.int 0x00000000
.int 0x0000ffff # 32 bit code
.int 0x00cf9a00
.int 0x0000ffff # 32 bit data
.int 0x00cf9200

.global loader_gdt_descriptor
loader_gdt_descriptor:
.short loader_gdt_descriptor - loader_gdt - 1
.int loader_gdt
