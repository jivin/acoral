#ifndef HAL_ATOMIC_H
#define HAL_ATOMIC_H
#include "type.h"
#include "atomic.h"
#define HAL_ATOMIC_READ(v) hal_atomic_read(v)
#define HAL_ATOMIC_SET(i,v) hal_atomic_set(i,v)
#define HAL_ATOMIC_INC(v)  hal_atomic_add(1,v)
#define HAL_ATOMIC_DEC(v)  hal_atomic_sub(1,v)
#define HAL_ATOMIC_ADD(i,v) hal_atomic_add(i,v)
#define HAL_ATOMIC_SUB(i,v) hal_atomic_sub(i,v)

static inline void hal_atomic_set(acoral_atomic_t *v, acoral_u32 i)
{
	acoral_u32 tmp;

	__asm__ __volatile__("@ atomic_set\n"
"1:	ldrex	%0, [%1]\n"
"	strex	%0, %2, [%1]\n"
"	teq	%0, #0\n"
"	bne	1b"
	: "=&r" (tmp)
	: "r" (&v->val), "r" (i)
	: "cc");
}

static inline acoral_32 hal_atomic_add(acoral_u32 i, acoral_atomic_t *v)
{
	acoral_u32 tmp;
	acoral_u32 result;

	__asm__ __volatile__("@ atomic_add_return\n"
"1:	ldrex	%0, [%2]\n"
"	add	%0, %0, %3\n"
"	strex	%1, %0, [%2]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (result), "=&r" (tmp)
	: "r" (&v->val), "Ir" (i)
	: "cc");

	return result;
}

static inline acoral_32 hal_atomic_sub(acoral_u32 i, acoral_atomic_t *v)
{
	acoral_u32 tmp;
	acoral_u32 result;

	__asm__ __volatile__("@ atomic_sub_return\n"
"1:	ldrex	%0, [%2]\n"
"	sub	%0, %0, %3\n"
"	strex	%1, %0, [%2]\n"
"	teq	%1, #0\n"
"	bne	1b"
	: "=&r" (result), "=&r" (tmp)
	: "r" (&v->val), "Ir" (i)
	: "cc");

	return result;
}

#endif
