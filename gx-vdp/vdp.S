/* gx-vdp/vdp.S, Copyright (C) 2013 Alex Iadicicco */

#include <gx-vdp/names.h>

#define VDPM_MASK    0x12780f8f
#define VDPM_ALWAYS  0x04440000 /* includes display enable */

	.text

	.globl gx_vdp_mode

	.globl gx_set_base_window
	.globl gx_set_base_scrolla
	.globl gx_set_base_scrollb
	.globl gx_set_base_sprite

	.globl gx_copy_to_vram
	.globl gx_copy_to_cram
	.globl gx_copy_to_vsram

gx_vdp_mode:
	move.l	4(%sp), %d0
	andi.l	#VDPM_MASK, %d0
	ori.l	#VDPM_ALWAYS, %d0
	move.l	%d0, -(%sp)

	move.w	#0x8000, %d0   /* MSR1 */
	or.b	(%sp), %d0
	add.l	#1, %sp
	move.w	%d0, VDP_CTL

	move.w	#0x8100, %d0   /* MSR2 */
	or.b	(%sp), %d0
	add.l	#1, %sp
	move.w	%d0, VDP_CTL

	move.w	#0x8b00, %d0   /* MSR3 */
	or.b	(%sp), %d0
	add.l	#1, %sp
	move.w	%d0, VDP_CTL

	move.w	#0x8c00, %d0   /* MSR4 */
	or.b	(%sp), %d0
	add.l	#1, %sp
	move.w	%d0, VDP_CTL

	rts


gx_set_base_window:

	move.l	4(%sp), %d0
	and.l	#0xf800, %d0
	move.w	#10, %d1
	lsr.l	%d1, %d0
	or.w	#0x8300, %d0
	move.w	%d0, VDP_CTL

	rts

gx_set_base_scrolla:

	move.l	4(%sp), %d0
	and.l	#0xe000, %d0
	move.w	#10, %d1
	lsr.l	%d1, %d0
	or.w	#0x8200, %d0
	move.w	%d0, VDP_CTL

	rts

gx_set_base_scrollb:

	move.l	4(%sp), %d0
	and.l	#0xe000, %d0
	move.w	#13, %d1
	lsr.l	%d1, %d0
	or.w	#0x8400, %d0
	move.w	%d0, VDP_CTL

	rts

gx_set_base_sprite:

	move.l	4(%sp), %d0
	and.l	#0xfe00, %d0
	move.w	#9, %d1
	lsr.l	%d1, %d0
	or.w	#0x8500, %d0
	move.w	%d0, VDP_CTL

	rts


gx_copy_to_cram:
	move.l	#0xc0000000, %d0
	bra	finish_copy

gx_copy_to_vsram:
	move.l	#0x40000010, %d0
	bra	finish_copy

gx_copy_to_vram:
	move.l	#0x40000000, %d0

finish_copy:
	move.w	#0x8f02, VDP_CTL  /* set VDP_INCR to 2 */

	move.l	%d2, -4(%sp)

	move.l	4(%sp), %d1
	move.l	#16, %d2
	lsl.l	%d2, %d1
	andi.l	#0x3fff0000, %d1
	or.l	%d1, %d0

	move.l	4(%sp), %d1
	move.l	#14, %d2
	lsr.l	%d2, %d1
	andi.l	#0x3, %d1
	or.l	%d1, %d0

	move.l	-4(%sp), %d2

	move.l	%d0, VDP_CTL

	move.l	8(%sp), %a0
	move.l	12(%sp), %a1
	add.l	%a0, %a1
8:	move.w	(%a0)+, VDP_DATA
	cmp.l	%a1, %a0
	blo	8b

	rts
