#ifndef ACORAL_PRINT_H
#define ACORAL_PRINT_H
#include<type.h>
#include<autocfg.h>
acoral_32 dummy_print(const acoral_char *fmt, ...);
acoral_32 acoral_print(const acoral_char *fmt, ...);
acoral_32 acoral_sprint(acoral_char * buf, acoral_u32 size, const acoral_char *fmt, ...);
acoral_32 acoral_prints(const acoral_char * buf);
#define acoral_printerr acoral_print

#ifndef CFG_LIB_EXT
#define acoral_prints(str)
#define acoral_print dummy_print
#endif

#ifdef CFG_TEST
#define acoral_printk(str) acoral_prints(str)
#else
#define acoral_printk(str)
#endif

#ifdef CFG_DEBUG
#undef acoral_printk(str)
#define acoral_printk(str) acoral_prints(str)
#define acoral_printdbg(str) acoral_prints(str)
#else
#undef acoral_printdbg(str)
#define acoral_printdbg(str)
#endif
#endif
