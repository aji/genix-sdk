	.section .vectbl
	.long	0                  /* initial stack pointer */
	.long	_start             /* entry point */
	.long	0                  /* bus error */
	.long	0                  /* address error */

	.long	0                  /* illegal instr */
	.long	0                  /* zero divide */
	.long	0                  /* chk instr */
	.long	0                  /* trapv instr */

	.long	0                  /* privilege viol */
	.long	0                  /* trace */
	.long	0                  /* line1010 emu */
	.long	0                  /* line1111 emu */

	.long	0, 0, 0
	.long	0                  /* uninit. vector */

	.long	0, 0, 0, 0

	.long	0, 0, 0, 0

	.long	0                  /* spurious interrupt */
	.long	0                  /* level 1 autovec */
	.long	0                  /* level 2 autovec */
	.long	0                  /* level 3 autovec */
	.long	0                  /* level 4 autovec */
	.long	0                  /* level 5 autovec */
	.long	__gx_vint_handler  /* level 6 autovec */
	.long	0                  /* level 7 autovec */

	.section .text
	.globl _start
_start:
	/* tmss, taken from the wikibooks page */
	move.b	(0xa10001), %d0
	andi.b	#0xf, %d0
	beq.b	v_0
	move.b	'S', 0xa14000
	move.b	'E', 0xa14001
	move.b	'G', 0xa14002
	move.b	'A', 0xa14003
v_0:
	andi.w	#0xf0ff, %sr
	ori.w	#0x0300, %sr

	/* set up stack */
	move.l	#0x01000000, %sp

	/* copy .data to RAM */
	lea	__data_load_beg, %a0
	lea	__data_load_end, %a1
	lea	__data_rel, %a2

1:	cmp	%a1, %a0
	bge	2f
	move.l	(%a0)+, (%a2)+
	jmp	1b
2:

	/* clear .bss */
	lea	__bss_beg, %a0
	lea	__bss_end, %a1

1:	cmp	%a1, %a0
	bge	2f
	move.l	#0, (%a0)+
	jmp	1b
2:

	/* jump to the main program */
	jsr	main

__die:
	nop
	jmp	1b

__ignore:
	rte
