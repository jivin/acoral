#include <hal.h>
#include <spinlock.h>
#include <lsched.h>
acoral_sr spin_lock_intr_save(acoral_spinlock_t *v){
	acoral_sr sr;
	sr=HAL_INTR_DISABLE_SAVE();
	acoral_spin_lock(v);
	return sr;
}

void spin_unlock_intr_restore(acoral_sr sr,acoral_spinlock_t *v){
	acoral_spin_unlock(v);
	HAL_INTR_RESTORE(sr);
}

void spin_lock_intr(acoral_spinlock_t *v){
	HAL_INTR_DISABLE();
	acoral_spin_lock(v);
}

void spin_unlock_intr(acoral_spinlock_t *v){
	acoral_spin_unlock(v);
	HAL_INTR_ENABLE();
}

void spin_lock_stamp(acoral_spinlock_t *v){
	acoral_sched_lock();
	acoral_spin_lock(v);
}

void spin_unlock_stamp(acoral_spinlock_t *v){
	acoral_spin_unlock(v);
	acoral_sched_unlock();
}

