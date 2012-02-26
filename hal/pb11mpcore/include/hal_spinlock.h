#ifndef HAL_SPINLOCK_H
#define HAL_SPINLOCK_H
#include<type.h>
#include<spinlock.h>
#define HAL_MB() __asm__ __volatile__("": : :"memory")
#define HAL_SPIN_INIT(v) ((v)->lock=0)
#define HAL_SPIN_INIT_LOCK(v) ((v)->lock=1)
#define HAL_SPIN_IS_LOCKED(x)	((x)->lock != 0)
#define HAL_SPIN_TRYLOCK(v) hal_spin_trylock(v) 
#define HAL_SPIN_LOCK(v) hal_spin_lock(v)
#define HAL_SPIN_UNLOCK(v) hal_spin_unlock(v)
static inline void hal_spin_lock(acoral_spinlock_t *lock)
{
	acoral_u32 tmp;
	__asm__ __volatile__(
"1:	ldrex	%0, [%1]\n"
"	teq	%0, #0\n"
"	strexeq	%0, %2, [%1]\n"
"	teqeq	%0, #0\n"
"	bne	1b"
	: "=&r" (tmp)
	: "r" (&lock->lock), "r" (1)
	: "cc");

	HAL_MB();
}

static inline acoral_32 hal_spin_trylock(acoral_spinlock_t *lock)
{
	acoral_u32 tmp;

	__asm__ __volatile__(
"	ldrex	%0, [%1]\n"
"	teq	%0, #0\n"
"	strexeq	%0, %2, [%1]"
	: "=&r" (tmp)
	: "r" (&lock->lock), "r" (1)
	: "cc");

	if (tmp == 0) {
		HAL_MB();
		return 1;
	} else {
		return 0;
	}
}

static inline void hal_spin_unlock(acoral_spinlock_t *lock)
{
	__asm__ __volatile__(
"	str	%1, [%0]\n"
	:
	: "r" (&lock->lock), "r" (0)
	: "cc");
}
#endif
