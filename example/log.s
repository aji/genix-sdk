	.text
	.globl __gx_log
__gx_log:
	move.l	4(%sp), %a0
	lea.l	8(%sp), %a1
	move.b	#0, (0xa05000)
	rts
