#ifndef ACORAL_IPI_H
#define ACORAL_IPI_H
#include<type.h>
#include<spinlock.h>

#define ACORAL_IPI_THREAD_BIT 4
#define ACORAL_IPI_THREAD_SIZE 4
#define ACORAL_IPI_THREAD_MASK(x) (x&0xf0)


#define ACORAL_IPI_THREAD_KILL (1<<ACORAL_IPI_THREAD_BIT)
#define ACORAL_IPI_THREAD_RESUME (1<<ACORAL_IPI_THREAD_BIT+1)
#define ACORAL_IPI_THREAD_SUSPEND (1<<ACORAL_IPI_THREAD_BIT+2)
#define ACORAL_IPI_THREAD_CHG_PRIO (1<<ACORAL_IPI_THREAD_BIT+3)
#define ACORAL_IPI_THREAD_MOVETO (1<<ACORAL_IPI_THREAD_BIT+4)
#define ACORAL_IPI_THREAD_MOVEIN (1<<ACORAL_IPI_THREAD_BIT+5)
#define ACORAL_IPI_THREAD_MOVE2_TAIL (1<<ACORAL_IPI_THREAD_BIT+6)

typedef struct{
	acoral_spinlock_t lock;
	acoral_u32 cmd;
	acoral_id thread_id;
	void *data;
}acoral_ipi_cmd_t;

void acoral_ipi_init(void);
void acoral_ipi_cmd_init(void);
void acoral_ipi_cmd_send(acoral_u32 cpu,acoral_u32 cmd,acoral_id thread_id,void *args);
void acoral_ipi_cmd_entry(acoral_vector vector);
void acoral_ipi_sched(void);

#endif
