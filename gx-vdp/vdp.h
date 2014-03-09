/* gx-vdp/vdp.h, Copyright (C) 2013 Alex Iadicicco */

#ifndef __INC_GX_VDP_VDP_H__
#define __INC_GX_VDP_VDP_H__   1

#include <gx-core/types.h>
#include <gx-vdp/names.h>

#define VDP_DATA16  ((volatile u16*)VDP_DATA)
#define VDP_CTL16   ((volatile u16*)VDP_CTL)
#define VDP_DATA32  ((volatile u32*)VDP_DATA)
#define VDP_CTL32   ((volatile u32*)VDP_CTL)

#define VDP_PUTREG(r, v)  (*VDP_CTL16 = ((r)|((u8)(v))))

/* 0x11223344, for MSR1, MSR2, etc. Sane and/or required settings are
   applied by default */

#define VDPM_HINT    0x10000000
#define VDPM_VINT    0x00200000
#define VDPM_V28C    0
#define VDPM_V30C    0x00080000
#define VDPM_EXINT   0x00000800
#define VDPM_VS_2C   0x00000400
#define VDPM_H32C    0
#define VDPM_H40C    0x00000081
#define VDPM_STE     0x00000008
/* only use 1 of the following: */
#define VDPM_HS_F    0
#define VDPM_HS_1C   0x00000200
#define VDPM_HS_1L   0x00000300
/* only use 1 of the following: */
#define VDPM_LSM_OFF 0
#define VDPM_LSM_ON  0x00000002
#define VDPM_LMS_DBL 0x00000006

#define gx_in_vblank() ((*VDP_CTL16 & 8) > 0)

extern void gx_vdp_mode(u32 mode);

extern void gx_set_base_window(u32 addr);
extern void gx_set_base_scrolla(u32 addr);
extern void gx_set_base_scrollb(u32 addr);
extern void gx_set_base_sprite(u32 addr);
extern void gx_set_base_hscroll(u32 addr);

/* When I figure out DMA, I will have DMA versions of these as well */
extern void gx_copy_to_vram(u32 to, const void *from, u32 len);
extern void gx_copy_to_cram(u32 to, const void *from, u32 len);
extern void gx_copy_to_vsram(u32 to, const void *from, u32 len);

/* some helpful data structures */

struct gx_vdp_pattern {
	unsigned prio:1;
	unsigned palette:2;
	unsigned vflip:1;
	unsigned hflip:1;
	unsigned name:11;
};

struct gx_vdp_sprite {
	u16 y; /* 10 bits */
	unsigned hsize:6; /* 2 bits */
	unsigned vsize:2; /* 2 bits */
	u8 link; /* 7 bits */
	struct gx_vdp_pattern pat;
	u16 x; /* 10 bits */
};

#define gx_vdp_pattern_addr(n) ((n) << 5)
#define gx_vdp_palette_addr(pal, n) ((((pal)<<5)&0x1e0) | (((n)<<1)&0x1e))
#define gx_vdp_sprite_offs(n) ((n) << 3)

static inline void gx_set_palette(int pal, int n, u16 color)
{
	gx_copy_to_cram(gx_vdp_palette_addr(pal, n), &color, 2);
}

static inline void gx_set_sprite(u16 base, int n, struct gx_vdp_sprite *sp)
{
	gx_copy_to_vram(base + gx_vdp_sprite_offs(n), sp, sizeof(*sp));
}

static inline void gx_copy_to_pattern(u16 pat, const void *from, u32 len)
{
	gx_copy_to_vram(gx_vdp_pattern_addr(pat), from, len);
}

static inline void gx_copy_to_palette(u8 pal, u8 ent, const void *from, u32 len)
{
	gx_copy_to_cram(gx_vdp_palette_addr(pal, ent), from, len);
}

#define gx_array_to_pattern(pat, array) \
	gx_copy_to_pattern(pat, array, sizeof(array))
#define gx_array_to_palette(pal, ent, array) \
	gx_copy_to_palette(pal, ent, array, sizeof(array))

/* and some nifty macros for fastfastfast vdp access */

#define gx_start_vram(addr) ((*VDP_CTL32 = 0x40000000 \
	| (((addr) & 0x3fff) << 16) | (((addr) & 0xc000) >> 14)), 0)
#define gx_start_cram(addr) ((*VDP_CTL32 = 0xc0000000 \
	| (((addr) & 0x3fff) << 16) | (((addr) & 0xc000) >> 14)), 0)
#define gx_start_vsram(addr) ((*VDP_CTL32 = 0x40000010 \
	| (((addr) & 0x3fff) << 16) | (((addr) & 0xc000) >> 14)), 0)

#define gx_step_size(n) (VDP_PUTREG(VDP_INCR, n), 0)

#define gx_put_word(data) (*VDP_DATA16 = (data), 0)
#define gx_put_long(data) (*VDP_DATA32 = (data), 0)

#endif
