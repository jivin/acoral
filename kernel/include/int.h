#ifndef ACORAL_INT_H
#define ACORAL_INT_H
#include"type.h"
#define ACORAL_COMM_INTR 1
#define ACORAL_EXPERT_INTR 2
#define ACORAL_RT_INTR 3
typedef struct {
	acoral_u32 index;
	acoral_u8  state;
	acoral_u8  type;
	void (*isr)(acoral_vector);
	void (*enter)(acoral_vector);
	void (*exit)(acoral_vector);
	void (*mask)(acoral_vector);
	void (*unmask)(acoral_vector);
}acoral_intr_ctr_t;
#define acoral_intr_enable() HAL_INTR_ENABLE()
#define acoral_intr_disable() HAL_INTR_DISABLE()
#define acoral_intr_nesting HAL_GET_INTR_NESTING()
#define acoral_intr_nesting_inc() HAL_INTR_NESTING_INC()
#define acoral_intr_nesting_dec() HAL_INTR_NESTING_DEC()
void acoral_intr_init(void);
acoral_32 acoral_intr_attach(acoral_vector vector,void (*isr)(acoral_vector));
acoral_32 acoral_intr_detach(acoral_vector vector);
void acoral_intr_entry(acoral_vector vector);
void acoral_intr_unmask(acoral_vector vector);
void acoral_intr_mask(acoral_vector vector);
void acoral_default_isr(acoral_vector vector);
void acoral_intr_exit();
extern acoral_intr_ctr_t intr_table[];
#endif
