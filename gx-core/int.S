/* gx-core/int.S, Copyright (C) 2014 Alex Iadicicco */

	.section .text

	.globl	__gx_vint_handler

__gx_vint_handler:
	move.l	(gx_vint_vec), %a0
	jmp	(%a0)

nothing:
	rte


	.section .data

	.globl	gx_vint_vec

gx_vint_vec:
	.long	nothing
