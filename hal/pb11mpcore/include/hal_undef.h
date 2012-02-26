#ifndef HAL_UNDEF_H
#define HAL_UNDEF_H
#define HAL_ATOMIC_READ(v) hal_atomic_read(v)
#define HAL_ATOMIC_SET(i,v) hal_atomic_set(i,v)
#define HAL_ATOMIC_INC(v)  hal_atomic_add(1,v)
#define HAL_ATOMIC_DEC(v)  hal_atomic_sub(1,v)
#define HAL_ATOMIC_ADD(i,v) hal_atomic_add(i,v)
#define HAL_ATOMIC_SUB(i,v) hal_atomic_sub(i,v)
#endif
