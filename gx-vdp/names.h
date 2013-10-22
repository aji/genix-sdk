/* gx-vdp/names.h, Copyright (C) 2013 Alex Iadicicco */

#ifndef __INC_GX_VDP_NAMES_H__
#define __INC_GX_VDP_NAMES_H__   1

/* The VDP used in the Sega Mega Drive is an enhanced version of the
   TMS9918 from Texas Instruments */

#define VDP_DATA       0xc00000
#define VDP_CTL        0xc00004

#define VDP_MSR1         0x8000 /* mode set reg 1 */
#define VDP_MSR2         0x8100 /* mode set reg 2 */
#define VDP_PNTSA        0x8200 /* pat name table base for scroll a */
#define VDP_PNTWN        0x8300 /* pat name table base for window */
#define VDP_PNTSB        0x8400 /* pat name table base for scroll b */
#define VDP_SAT          0x8500 /* sprite attr table base */
#define VDP_BGCOL        0x8700 /* background color */
#define VDP_HINT         0x8a00 /* h interrupt register */
#define VDP_MSR3         0x8b00 /* mode set reg 3 */
#define VDP_MSR4         0x8c00 /* mode set reg 4 */
#define VDP_HSDT         0x8d00 /* h scroll data table base addr */
#define VDP_INCR         0x8f00 /* auto increment data */
#define VDP_SSZ          0x9000 /* scroll size */
#define VDP_WINH         0x9100 /* window h position */
#define VDP_WINV         0x9200 /* window v position */
#define VDP_DMALCL       0x9300 /* dma length counter low */
#define VDP_DMALCH       0x9400 /* dma length counter high */
#define VDP_DMASAL       0x9500 /* dma source address low */
#define VDP_DMASAM       0x9600 /* dma source address mid */
#define VDP_DMASAH       0x9700 /* dma source address high */

#define VDPS_FIFOEMPTY   0x0200 /* FIFO empty */
#define VDPS_FIFOFULL    0x0100 /* FIFO full */
#define VDPS_VINTPEND    0x0080 /* vertical interrupt pending */
#define VDPS_SPRITEOFL   0x0040 /* sprite overflow on this scan line */
#define VDPS_SPRITECOLL  0x0020 /* sprite collision */
#define VDPS_ODDFRAME    0x0010 /* odd frame */
#define VDPS_VBLANK      0x0008 /* vertical blanking */
#define VDPS_HBLANK      0x0004 /* horizontal blanking */
#define VDPS_DMAINP      0x0002 /* DMA in progress */
#define VDPS_PALMODE     0x0001 /* PAL mode flag */

#endif
