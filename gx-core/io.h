/* gx-core/io.h, Copyright (C) 2014 Alex Iadicicco */

#ifndef __INC_GX_CORE_IO_H__
#define __INC_GX_CORE_IO_H__

#define IO_CTL_ONE ((volatile u8*) 0xa10003)
#define IO_CTL_TWO ((volatile u8*) 0xa10005)

#define CTL_RAW_UP     0x01
#define CTL_RAW_DOWN   0x02
#define CTL_RAW_LEFT   0x04
#define CTL_RAW_RIGHT  0x08
#define CTL_RAW_B      0x10
#define CTL_RAW_C      0x20
#define CTL_RAW_A      0x10
#define CTL_RAW_START  0x20

#define CTL_UP       0x0001
#define CTL_DOWN     0x0002
#define CTL_LEFT     0x0004
#define CTL_RIGHT    0x0008
#define CTL_B        0x0010
#define CTL_C        0x0020
#define CTL_A        0x1000
#define CTL_START    0x2000

static inline u8 gx_read_io_low(volatile u8 *port)
{
	*port |= 0x40;
	return ~*port;
}

static inline u8 gx_read_io_high(volatile u8 *port)
{
	*port &= ~0x40;
	return ~*port;
}

static inline u16 gx_read_io(volatile u8 *port)
{
	return gx_read_io_low(port) | gx_read_io_high(port) << 8;
}

#endif
