#include "vdp.h"

const u16 palette[] = {
	0x000, 0x000, 0xeee, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,

	0x000, 0x000, 0xe40, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,

	0x000, 0x000, 0x40e, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,

	0x000, 0x20c, 0xeee, 0x000, 0x000, 0x000, 0x000, 0x000,
	0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000, 0x000,
};

#include "font-bold.xbm"

/*
x = 1, . = 0, 12345678 = bits
 
v= ........ ........ ........ 12345678
 * ...x.... ...x.... .......x .......x
   -------- -------- -------- --------
   56781234 5678.... 12345678 12345678
 & ...x.... .x...... ...x.... .x......
>> ....5678 12345678 ....1234 56781234
 & ......x. ....x... ......x. ....x...
   -------- -------- -------- --------
   ...8..7. .6..5... ...4..3. .2..1...
 + .xxx.xx. .x...... .xxx.xx. .x......
 & x...x... x...x... x...x... x...x...
   -------- -------- -------- --------
   8...7... 6...5... 4...3... 2...1...
>> ...8...7 ...6...5 ...4...3 ...2...1
 * ........ ........ ........ ....xxxx
   -------- -------- -------- --------
   88887777 66665555 44443333 22221111 :D
*/

u32 unpack(u32 v)
{
	v = v * 0x10100101;
	v = (v & 0x10401040) | ((v >> 4) & 0x02080208);
	v = (v + 0x76407640) & 0x88888888;
	return v >> 3;
}

#define CTL_UP    0x01
#define CTL_DOWN  0x02
#define CTL_LEFT  0x04
#define CTL_RIGHT 0x08
#define CTL_B     0x10
#define CTL_C     0x20
#define CTL_A     0x40
#define CTL_START 0x80

u8 read_ctl(volatile u8 *ctl)
{
	u8 a;
	a = *ctl;
	*ctl &= ~0x40;
	a = (a & 0x3f) | ((*ctl & 0x30) << 2);
	*ctl |= 0x40;
	return a;
}

void wr(u16 x, u16 y, unsigned palette, char *s)
{
	x = x + y * 32;

	for (; *s; s++, x++) {
		vdp_bg[x].palette = palette;
		vdp_bg[x].name = *s;
	}
}

extern void __gx_log(const char *fmt, ...);

#ifdef GX_LOGGING
# define gx_log(a...) __gx_log(a)
#else
# define gx_log(a...)
#endif

int main(void)
{
	const char *alpha = "0123456789abcdef";
	s32 i, x, p;
	u8 *ctl = (u8*)0xa10003;
	u8 *sctl = (u8*)0xa10009;
	u8 now, was = 0;
	u8 pos, neg;

	gx_log("3 == *d, f == *x, *s\n", 3, 0xf, "hello");

	vdp_init();

	/* palette */
	/*
	*VDP_CTL32 = 0xc0000000;
	for (i=0; i<sizeof(palette)/2; i++)
		*VDP_DATA16 = *(palette + i) << 1;
	*/
	vdp_copy(VDP_CRAM, 0, (u16*)palette, sizeof(palette));

	/* basic characters */
	*VDP_CTL32 = 0x40000000;
	for (i=0; i<sizeof(font_bits); i++) {
		x = (i&0xff80) | ((i<<4)&0x70) | ((i>>3)&0x0f);
		x = font_bits[x];
		x = unpack(x) + 0x11111111;
		*VDP_DATA32 = x;
	}

	for (i=0; i<VDP_BG_SIZE; i++) {
		vdp_bg[i].priority = 0;
		vdp_bg[i].palette = 0;
		vdp_bg[i].vflip = 0;
		vdp_bg[i].hflip = 0;
		vdp_bg[i].name = 0;
	}

	for (;;) {
		while (!vdp_is_vblank());

		now = read_ctl(ctl);
		pos = ~now & was;
		neg = now & ~was;
		was = now;

		vdp_copy(VDP_VRAM, 0xe000, (u16*)vdp_bg, sizeof(vdp_bg));

		while (vdp_is_vblank());

		wr(2, 10, 0, "Hello, world!");
	}

	return 0;
}
