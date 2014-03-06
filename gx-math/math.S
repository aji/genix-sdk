/* gx-math/math.S, Copyright (C) 2014 Alex Iadicicco */

	.globl	gx_sin
	.globl	gx_cos

#include "sin_10_16.s"

perform_lookup:
	move.l	#sin_10_16_table, %a1
	andi.l	#0x3ff, %d0
	lsl.l	#1, %d0
	move.l	(%a1,%d0), %d0
	rts

gx_sin:
	move.l	4(%sp), %d0
	bra	perform_lookup

gx_cos:
	move.l	4(%sp), %d0
	addi.l	#0x100, %d0
	bra	perform_lookup
