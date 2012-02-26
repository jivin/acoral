#ifndef HAL_UNDEF_H
#define HAL_UNDEF_H

#undef HAL_SCHED_BRIDGE()
#undef HAL_INTR_EXIT_BRIDGE()
#undef HAL_INTR_NESTING_INIT()   
#undef HAL_GET_INTR_NESTING()    
#undef HAL_INTR_NESTING_DEC()    
#undef HAL_INTR_NESTING_INC()   
#undef HAL_START_OS(stack)  

#define HAL_INTR_NESTING_INIT()   
#define HAL_GET_INTR_NESTING()    0
#define HAL_INTR_NESTING_DEC()    
#define HAL_INTR_NESTING_INC()   

#endif
