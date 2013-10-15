/* video/vdp.c, Copyright (C) 2013 Alex Iadicicco */

#include "vdp.h"

struct vdp_bg_ent vdp_bg[VDP_BG_SIZE];

int vdp_init(void)
{
	/* Crack open your favorite Genesis VDP manual to decode these */
	VDP_PUTREG(VDP_MSR1,  0x14); /* 0x10 = Hint en */
	VDP_PUTREG(VDP_MSR2,  0x74); /* 0x40 = display, 0x20 = Vint en */
	VDP_PUTREG(VDP_PNTSA, 0xff);
	VDP_PUTREG(VDP_PNTWN, 0x00);
	VDP_PUTREG(VDP_PNTSB, 0xff);
	VDP_PUTREG(VDP_BGCOL, 0x00);
	VDP_PUTREG(VDP_HINT,  0x00);
	VDP_PUTREG(VDP_MSR3,  0x00);
	VDP_PUTREG(VDP_MSR4,  0x00);
	VDP_PUTREG(VDP_HSDT,  0x00);
	VDP_PUTREG(VDP_INCR,  0x02);

	/* TODO: load a default set of sprites or something */

	return 0;
}

int vdp_copy(u8 type, u16 to, u16 *from, u16 len)
{
	*VDP_CTL32 = (type << 30) | ((type << 2) & 0xf0)
		| ((to << 16) & 0x3f000000) | (to >> 14);
	for (len>>=1; len; len--)
		*VDP_DATA16 = *from++;
	return 0;
}

/* these aren't working. not sure why */

int vdp_dma(u8 type, u16 to, u32 from, u16 len)
{
	VDP_PUTREG(VDP_DMALCL, (u8)(len >> 0));
	VDP_PUTREG(VDP_DMALCH, (u8)(len >> 8));

	VDP_PUTREG(VDP_DMASAL, (u8)(from >>  0));
	VDP_PUTREG(VDP_DMASAM, (u8)(from >>  8));
	VDP_PUTREG(VDP_DMASAH, (u8)(from >> 16));

	VDP_PUTREG(VDP_INCR, 0x02);

	*VDP_CTL16 = (((u8)(type)) << 14) | (to & 0x3fff);
	*VDP_CTL16 = 0x80 | ((type & 0x4) << 2) | (to >> 14);

	return 0;
}

int vdp_fill(u16 to, u16 word, u16 len)
{
	VDP_PUTREG(VDP_DMALCL, len & 0xff);
	VDP_PUTREG(VDP_DMALCH, (len >> 8));

	VDP_PUTREG(VDP_DMASAH, 0x80);

	VDP_PUTREG(VDP_INCR, 0x02);

	*VDP_CTL16 = 0x4000 | (to & 0x3fff);
	*VDP_CTL16 = 0x80 | (to >> 14);

	*VDP_DATA16 = word;

	return 0;
}
