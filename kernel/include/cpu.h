#ifndef ACORAL_CPU_H
#define ACORAL_CPU_H
#include "autocfg.h"
#include "type.h"
#include "hal_port.h"
#ifndef CFG_CMP
#define CFG_MAX_CPU 1
#define acoral_current_cpu 0
#define acoral_idlest_cpu 0
#else
#define acoral_current_cpu HAL_GET_CURRENT_CPU()
#define acoral_idlest_cpu acoral_get_idlest_cpu()
#endif
acoral_u32 acoral_get_idle_maskcpu(acoral_u32 cpu_mask);
#endif
