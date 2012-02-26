#ifndef ACORAL_SPIN_LOCK_H
#define ACORAL_SPIN_LOCK_H
#include <autocfg.h>
#include <type.h>
typedef struct {volatile acoral_32 lock;}acoral_spinlock_t;
#ifdef CFG_CMP
#define acoral_spin_init(v) HAL_SPIN_INIT(v)
#define acoral_spin_init_lock(v) HAL_SPIN_INIT_LOCK(v)
#define acoral_spin_is_locked(v) HAL_SPIN_IS_LOCKED(v)
#define acoral_spin_try_lock(v) HAL_SPIN_TRY_LOCK(v)
#define acoral_spin_lock(v) HAL_SPIN_LOCK(v)
#define acoral_spin_unlock(v) HAL_SPIN_UNLOCK(v)
#else
#define acoral_spin_init(v)
#define acoral_spin_init_lock(v)
#define acoral_spin_is_locked(v) 
#define acoral_spin_try_lock(v) 
#define acoral_spin_lock(v)
#define acoral_spin_unlock(v)
#endif

#define acoral_spin_lock_intr_save(v) (sr=spin_lock_intr_save(v))
#define acoral_spin_unlock_intr_restore(v) spin_unlock_intr_restore(sr,v)
#define acoral_spin_lock_intr(v) spin_lock_intr(v)
#define acoral_spin_unlock_intr(v) spin_unlock_intr(v)
#define acoral_spin_lock_stamp(v) spin_lock_stamp(v)
#define acoral_spin_unlock_stamp(v) spin_unlock_stamp(v)

void spin_lock_intr(acoral_spinlock_t *v);
void spin_unlock_intr(acoral_spinlock_t *v);
void spin_lock_stamp(acoral_spinlock_t *v);
void spin_unlock_stamp(acoral_spinlock_t *v);

#endif
