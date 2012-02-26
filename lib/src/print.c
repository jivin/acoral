#include<print.h>
#include<string.h>
#include<driver.h>
#include<lsched.h>
#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/acoral_32 */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */
static acoral_32 do_div(acoral_64 *n,acoral_32 base)		
{								
	acoral_32 __res;				
	__res = ((acoral_u32)*n) % (acoral_u32)base;
	*n = ((acoral_u32)*n) / (acoral_u32)base;	
	return __res;					
}


acoral_32 skip_atoi(const acoral_char **s)
    {/*change the string "s" to digit*/
            acoral_32 i=0;
 
            while (IS_DIGIT(**s))
                    i = i*10 + *((*s)++) - '0';
            return i;
} 

const acoral_char small_digits[] = "0123456789abcdefghijklmnopqrstuvwxyz";
const acoral_char large_digits[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
acoral_char * number(acoral_char * buf, acoral_char * end, acoral_64 num, acoral_32 base, acoral_32 size, acoral_32 precision, acoral_32 type)
{
	acoral_char c,sign,tmp[66];
	const acoral_char *digits;
	acoral_32 i;

	digits = (type & LARGE) ? large_digits : small_digits;
	if (type & LEFT)
		type &= ~ZEROPAD;
	if (base < 2 || base > 36)
		return 0;
	c = (type & ZEROPAD) ? '0' : ' ';
	sign = 0;
	if (type & SIGN) {
		if (num < 0) {
			sign = '-';
			num = -num;
			size--;
		} else if (type & PLUS) {
			sign = '+';
			size--;
		} else if (type & SPACE) {
			sign = ' ';
			size--;
		}
	}
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}
	i = 0;
	if (num == 0)
		tmp[i++]='0';
	else while (num != 0)
		tmp[i++] = digits[do_div(&num,base)];
	if (i > precision)
		precision = i;
	size -= precision;
	if (!(type&(ZEROPAD+LEFT))) {
		while(size-->0) {
			if (buf <= end)
				*buf = ' ';
			++buf;
		}
	}
	if (sign) {
		if (buf <= end)
			*buf = sign;
		++buf;
	}
	if (type & SPECIAL) {
		if (base==8) {
			if (buf <= end)
				*buf = '0';
			++buf;
		} else if (base==16) {
			if (buf <= end)
				*buf = '0';
			++buf;
			if (buf <= end)
				*buf = digits[33];
			++buf;
		}
	}
	if (!(type & LEFT)) {
		while (size-- > 0) {
			if (buf <= end)
				*buf = c;
			++buf;
		}
	}
	while (i < precision--) {
		if (buf <= end)
			*buf = '0';
		++buf;
	}
	while (i-- > 0) {
		if (buf <= end)
			*buf = tmp[i];
		++buf;
	}
	while (size-- > 0) {
		if (buf <= end)
			*buf = ' ';
		++buf;
	}
	return buf;
}

acoral_32 vsnprintf(acoral_char *buf, acoral_u32 size, const acoral_char *fmt, va_list args)
{
	acoral_32 len;
	acoral_u64 num;
	acoral_32 i, base;
	acoral_char *str, *end, c;
	const acoral_char *s;

	acoral_32 flags;		/* flags to number() */

	acoral_32 field_width;	/* width of output field */
	acoral_32 precision;		/* min. # of digits for acoral_32egers; max
				   number of acoral_chars for from string */
	acoral_32 qualifier;		/* 'h', 'l', or 'L' for acoral_32eger fields */
				/* 'z' support added 23/7/1999 S.H.    */
				/* 'z' changed to 'Z' --davidm 1/25/99 */

	str = buf;
	end = buf + size - 1;

	if (end < buf - 1) {
		end = ((void *) -1);
		size = end - buf + 1;
	}

	for (; *fmt ; ++fmt) {
		if (*fmt != '%') {
			if (str <= end)
				*str = *fmt;
			++str;
			continue;
		}

		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case '-': flags |= LEFT; goto repeat;
				case '+': flags |= PLUS; goto repeat;
				case ' ': flags |= SPACE; goto repeat;
				case '#': flags |= SPECIAL; goto repeat;
				case '0': flags |= ZEROPAD; goto repeat;
			}

		/* get field width */
		field_width = -1;
		if (IS_DIGIT(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, acoral_32);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			++fmt;	
			if (IS_DIGIT(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == '*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, acoral_32);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt =='Z') {
			qualifier = *fmt;
			++fmt;
			if (qualifier == 'l' && *fmt == 'l') {
				qualifier = 'L';
				++fmt;
			}
		}

		/* default base */
		base = 10;

		switch (*fmt) {
			case 'c':
				if (!(flags & LEFT)) {
					while (--field_width > 0) {
						if (str <= end)
							*str = ' ';
						++str;
					}
				}
				c = (acoral_u8) va_arg(args, acoral_32);
				if (str <= end)
					*str = c;
				++str;
				while (--field_width > 0) {
					if (str <= end)
						*str = ' ';
					++str;
				}
				continue;

			case 's':
				s = va_arg(args, acoral_char *);
				if (!s)
					s = "<NULL>";

				len = acoral_str_nlen(s, precision);

				if (!(flags & LEFT)) {
					while (len < field_width--) {
						if (str <= end)
							*str = ' ';
						++str;
					}
				}
				for (i = 0; i < len; ++i) {
					if (str <= end)
						*str = *s;
					++str; ++s;
				}
				while (len < field_width--) {
					if (str <= end)
						*str = ' ';
					++str;
				}
				continue;

			case 'p':
				if (field_width == -1) {
					field_width = 2*sizeof(void *);
					flags |= ZEROPAD;
				}
				str = number(str, end,
						(acoral_u32) va_arg(args, void *),
						16, field_width, precision, flags);
				continue;


			case 'n':
				/* FIXME:
				* What does C99 say about the overflow case here? */
				if (qualifier == 'l') {
					acoral_32 * ip = va_arg(args, acoral_32 *);
					*ip = (str - buf);
				} else if (qualifier == 'Z') {
					acoral_u32 * ip = va_arg(args, acoral_u32 *);
					*ip = (str - buf);
				} else {
					acoral_32 * ip = va_arg(args, acoral_32 *);
					*ip = (str - buf);
				}
				continue;

			case '%':
				if (str <= end)
					*str = '%';
				++str;
				continue;

				/* acoral_32eger number formats - set up the flags and "break" */
			case 'o':
				base = 8;
				break;

			case 'X':
				flags |= LARGE;
			case 'x':
				base = 16;
				break;

			case 'd':
			case 'i':
				flags |= SIGN;
			case 'u':
				break;

			default:
				if (str <= end)
					*str = '%';
				++str;
				if (*fmt) {
					if (str <= end)
						*str = *fmt;
					++str;
				} else {
					--fmt;
				}
				continue;
		}
		if (qualifier == 'L')
			num = va_arg(args, acoral_64);
		else if (qualifier == 'l') {
			num = va_arg(args, acoral_u32);
			if (flags & SIGN)
				num = (acoral_32) num;
		} else if (qualifier == 'Z') {
			num = va_arg(args, acoral_u32);
		} else if (qualifier == 'h') {
			num = (acoral_u16) va_arg(args, acoral_32);
			if (flags & SIGN)
				num = (acoral_16) num;
		} else {
			num = va_arg(args, acoral_u32);
			if (flags & SIGN)
				num = (acoral_32) num;
		}
		str = number(str, end, num, base,
				field_width, precision, flags);
	}
	if (str <= end)
		*str = '\0';
	else if (size > 0)
		/* don't write out a null byte if the buf size is zero */
		*end = '\0';
	/* the trailing null byte doesn't count towards the total
	* ++str;
	*/
	return str-buf;
}

acoral_32 acoral_sprint(acoral_char * buf, acoral_u32 size, const acoral_char *fmt, ...)
{
	acoral_32 i;
	va_list args;
	va_start(args, fmt);
	i=vsnprintf(buf,size,fmt,args);
	va_end(args);
	return i;
}

acoral_32 acoral_print(const acoral_char *fmt, ...)
{
	va_list args;
	acoral_32 printed_len;
	static acoral_char print_buf[256];
	va_start(args, fmt);
	printed_len =vsnprintf(print_buf, sizeof(print_buf), fmt, args);
	va_end(args);
	return acoral_dev_write(acoral_cur_thread->console_id,(void *)print_buf,printed_len,0,0);
}

acoral_32 acoral_prints(const acoral_char *str)
{
	acoral_32 printed_len;
	printed_len=acoral_str_len(str);  
	return acoral_dev_write(acoral_cur_thread->console_id,(void *)str,printed_len,0,0);
}

acoral_char acoral_getchar(){
	acoral_char c;
	acoral_dev_read(acoral_cur_thread->console_id,&c,1,0,0);
	return c;
}

acoral_char acoral_putchar(acoral_char c){
	acoral_dev_write(acoral_cur_thread->console_id,&c,1,0,0);
	return c;
}

static acoral_id console_id=ACORAL_DEV_ERR_ID;
//======================
acoral_32 acoral_debug(const acoral_char *fmt, ...)
{
	va_list args;
	acoral_32 printed_len;
	static acoral_char print_buf[1024];
	va_start(args, fmt);
	printed_len =vsnprintf(print_buf, sizeof(print_buf), fmt, args);
	va_end(args);
	if(console_id==ACORAL_DEV_ERR_ID){
		console_id=acoral_dev_open("console");
		if(console_id==ACORAL_DEV_ERR_ID)
			return;
	}
	return acoral_dev_write(console_id,(void *)print_buf,printed_len,0,0);
}

acoral_32 acoral_debugs(const acoral_char *str)
{
	acoral_32 printed_len;
	printed_len=acoral_str_len(str);  
	if(console_id==ACORAL_DEV_ERR_ID){
		console_id=acoral_dev_open("console");
		if(console_id==ACORAL_DEV_ERR_ID)
			return;
	}
	return acoral_dev_write(console_id,(void *)str,printed_len,0,0);
}
