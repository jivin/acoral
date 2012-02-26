#ifndef ACORAL_CMP_H
#define ACORAL_CMP_H
#include "type.h"
#include "thread.h"
void acoral_cmp_init(void);
void acoral_prepare_cpus(void);
void acoral_start_cpus(void);
void acoral_follow_cpu_start();
void idle_follow(void *args);
void acoral_moveto_thread(acoral_thread_t *thread,acoral_u8 cpu);
void acoral_moveto_thread_by_id(acoral_id thread_id,acoral_u8 cpu);
void acoral_jump_cpu(acoral_u8 cpu);
void acoral_movein_thread(acoral_thread_t *thread);
void acoral_movein_thread_by_id(acoral_id thread_id);
#endif
