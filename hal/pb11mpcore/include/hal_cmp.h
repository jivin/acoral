#ifndef HAL_CMP_H
#define HAL_CMP_H
#define HAL_CMD_IPI 2
#define HAL_CPU_IS_ACTIVE(cpu) hal_cpu_is_active(cpu)

#define HAL_PREPARE_CPUS() hal_prepare_cpus()
#define HAL_START_CPU(cpu) hal_start_cpu(cpu)

#define HAL_CMP_ACK() hal_cmp_ack()
#define HAL_WAIT_ACK() hal_wait_ack()

#define HAL_CORE_CPU_INIT() hal_core_cpu_init()
#define HAL_FOLLOW_CPU_INIT() hal_follow_cpu_init() 

#endif
