#ifndef HAL_IPI_H
#define HAL_IPI_H
#define HAL_IPI_SEND(cpu,vector)                   hal_ipi_send(cpu,vector)
#define HAL_IPI_SEND_CPUS(cpulist,vector)          hal_ipi_send(cpulist,vector)
#define HAL_IPI_SEND_ALL(vector)                   hal_ipi_send_all(vector)
#endif
