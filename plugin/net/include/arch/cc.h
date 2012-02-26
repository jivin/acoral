#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

#include <acoral.h>

/* 定义与编译器无关的数据类型 */
typedef acoral_u8    u8_t;
typedef acoral_8     s8_t;
typedef acoral_u16   u16_t;
typedef acoral_16    s16_t;
typedef acoral_u32   u32_t;
typedef acoral_32    s32_t;
typedef acoral_u32   mem_ptr_t;

/* 结构体 */
#ifdef __GNU_C__ 

#define PACK_STRUCT_FIELD(x)  x 
#define PACK_STRUCT_STRUCT  __attribute__((__packed__))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

#else 

#define PACK_STRUCT_FIELD(x) __packed x 
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_BEGIN 	 __packed
#define PACK_STRUCT_END

#endif

/* 平台调试输出和断言 */
#if 1


#define IP_REASSEMBLY  0

#define lwip_printf   acoral_debug
#define LWIP_NOASSERT
#define LWIP_ERROR(m,e,h)
//#define LWIP_DEBUG  

#define LWIP_PLATFORM_DIAG(x)	 { lwip_printf x; }
#define LWIP_PLATFORM_ASSERT(x)  { lwip_printf("Assertion \"%s\" failed at line %d in %s\n", \
                                     x, __LINE__, __FILE__); while(1); }
#else // 1

#define LWIP_PLATFORM_DIAG(x)
#define LWIP_PLATFORM_ASSERT(x)  { while(1); }

#endif // 1

#define strlen  acoral_str_len
#define strcpy  acoral_str_cpy
#define strcmp  acoral_str_cmp
#define memcpy  acoral_memcpy
#define memset  acoral_memset

/* 临界区的保护 */
#define	SYS_ARCH_DECL_PROTECT(x)  acoral_sr cpu_sr;
#define	SYS_ARCH_PROTECT(x)       HAL_ENTER_CRITICAL();
#define	SYS_ARCH_UNPROTECT(x)     HAL_EXIT_CRITICAL();

#endif /* __ARCH_CC_H__ */
