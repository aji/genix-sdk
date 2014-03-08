/* gx-core/int.S, Copyright (C) 2014 Alex Iadicicco */

	.section .text

	.globl	__gx_vint_handler

__gx_vint_handler:
	ori.w	#0x0700, %sr
	move.l	(gx_vint_vec), %a0
	jsr	(%a0)
	andi.w	#0xf3ff, %sr
	rte

nothing:
	rts


	.section .data

	.globl	gx_vint_vec

gx_vint_vec:
	.long	nothing
