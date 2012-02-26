#ifndef HAL_SPINLOCK_H
#define HAL_SPINLOCK_H
#include"spinlock.h"
#define LOCK "lock ; "

#define HAL_SPIN_INIT(v) ((v)->lock=1)
#define HAL_SPIN_INIT_LOCK(v) ((v)->lock=0)
#define HAL_SPIN_IS_LOCKED(v) ((v)->lock<=0)
#define HAL_SPIN_TRYLOCK(v) hal_spin_trylock(v)
#define HAL_SPIN_LOCK(v) hal_spin_lock(v)
#define HAL_SPIN_UNLOCK(v) hal_spin_unlock(v)

static inline void hal_spin_lock(acoral_spinlock_t *lock)
{
	__asm__ __volatile__(
		"\n1:\t" \
			"lock ; decb %0\n\t" \
			"jns 3f\n" \
		"2:\t" \
			"rep;nop\n\t" \
			"cmpb $0,%0\n\t" \
			"jle 2b\n\t" \
			"jmp 1b\n" \
		"3:\n\t" \
		:"=m" (lock->lock) : : "memory"
	);
}

static inline void hal_spin_unlock(acoral_spinlock_t *lock)
{
	acoral_8 oldval=1;
	__asm__ __volatile__(
		"xchgb %b0,%1"
		:"=q" (oldval), "=m" (lock->lock)
		:"0" (oldval) : "memory");
}

static inline int hal_spin_trylock(acoral_spinlock_t *lock)
{
	acoral_8 oldval;
	__asm__ __volatile__(
		"xchgb %b0,%1"
		:"=q" (oldval), "=m" (lock->lock)
		:"0" (0) : "memory");
	return oldval > 0;
}

#endif
