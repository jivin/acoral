#ifndef ACORAL_ATOMIC_H
#define ACORAL_ATOMIC_H
#include "type.h"
/*
 * Make sure gcc doesn't try to be clever and move things around
 * on us. We need to use _exactly_ the address the user gave us,
 * not some alias that contains the same information.
 */
typedef struct { volatile acoral_32 val; } acoral_atomic_t;
#define acoral_atomic_init(v) 
#define acoral_atomic_read(v)  HAL_ATOMIC_READ(v)
#define acoral_atomic_inc(v)   HAL_ATOMIC_INC(v)
#define acoral_atomic_dec(v)   HAL_ATOMIC_DEC(v)
#define acoral_atomic_set(i,v) HAL_ATOMIC_SET(i,v)
#define acoral_atomic_sub(i,v) HAL_ATOMIC_SUB(i,v)
#define acoral_atomic_add(i,v) HAL_ATOMIC_ADD(i,v)

#define acoral_atomic_set_and_test(i,v) (acoral_atomic_set(i,v)==0)
#define acoral_atomic_sub_and_test(i,v) (acoral_atomic_sub(i,v)==0)
#define acoral_atomic_add_and_test(i,v) (acoral_atomic_add(i,v)==0)
#endif
