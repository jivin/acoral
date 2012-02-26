#ifndef HAL_UNDEF_H
#define HAL_UNDEF_H
#define HAL_ATOMIC_INIT(v) hal_atomic_init(v)
#define HAL_ATOMIC_READ(v) ((v)->val)
#define HAL_ATOMIC_SET(i,v) (((v)->val) = (i))
#define HAL_ATOMIC_INC(v)  hal_atomic_inc(v)
#define HAL_ATOMIC_DEC(v)  hal_atomic_dec(v)
#define HAL_ATOMIC_ADD(i,v) hal_atomic_add(i,v)
#define HAL_ATOMIC_SUB(i,v) hal_atomic_sub(i,v)
#endif
