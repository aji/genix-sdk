/* gx-core/int.h, Copyright (C) 2014 Alex Iadicicco */

#ifndef __INC_GX_CORE_INT_H__
#define __INC_GX_CORE_INT_H__   1

#define gx_vec_fn __attribute__((interrupt))

extern void (*gx_vint_vec)(void);

#endif
