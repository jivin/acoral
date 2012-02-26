#ifndef HAL_ATOMIC_H
#define HAL_ATOMIC_H
#include "type.h"
#include "atomic.h"
#define LOCK "lock ; "

static __inline__ int hal_atomic_add(int i, acoral_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		LOCK "addl %2,%0; sete %1"
		:"=m" (v->val), "=qm" (c)
		:"ir" (i), "m" (v->val) : "memory");
	return c;
}

static __inline__ int hal_atomic_sub(int i, acoral_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		LOCK "subl %2,%0; sete %1"
		:"=m" (v->val), "=qm" (c)
		:"ir" (i), "m" (v->val) : "memory");
	return c;
}


static __inline__ int hal_atomic_inc(acoral_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		LOCK "incl %0; sete %1"
		:"=m" (v->val), "=qm" (c)
		:"m" (v->val) : "memory");
	return c != 0;
}

static __inline__ int hal_atomic_dec(acoral_atomic_t *v)
{
	unsigned char c;

	__asm__ __volatile__(
		LOCK "decl %0; sete %1"
		:"=m" (v->val), "=qm" (c)
		:"m" (v->val) : "memory");
	return c != 0;
}

#endif
