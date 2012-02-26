#ifndef ACORAL_COMM_H
#define ACORAL_COMM_H
#include<type.h>
typedef acoral_char * va_list;

#define _INTSIZEOF(n) \
((sizeof(n)+sizeof(acoral_32)-1)&~(sizeof(acoral_32) - 1) )

#define va_start(ap,v) ( ap = (va_list)&v + _INTSIZEOF(v) )

#define va_arg(ap,t) \
( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )

#define va_end(ap) ( ap = (va_list)0 )

#define offsetof(TYPE, MEMBER) ((acoral_u32) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) ((type *)((acoral_8 *)ptr - offsetof(type,member)))
#endif

