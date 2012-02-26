#include<string.h>

acoral_u8  acoral_tolower(acoral_u8 c){
      if(IS_LOWER(c))
        return c;
      else if(IS_UPPER(c))
        return (acoral_u8)(c+32);
      else
        return FALSE; 
            
}
acoral_u8  acoral_toupper(acoral_u8 c){
     if(IS_LOWER(c))
        return (acoral_u8)(c-32);
     else if(IS_UPPER(c))
        return c;
     else
        return FALSE; 
}

acoral_char *acoral_str_upr(acoral_char *str){
	acoral_u32 i=0;
	acoral_char *s=str;
	while(s[i]!='\0'){
		if(s[i]>='a'&&s[i]<='z')
			s[i]-=32;
		i++;
	} 
	return str;
}

acoral_char *acoral_str_lwr(acoral_char *s){
	acoral_u32 i=0;
	while(s[i]!='\0'){
		if(s[i]>='A'&&s[i]<='Z')
			s[i]+=32;
		i++;
	} 
	return s;
}

acoral_u32 acoral_str_len(const acoral_char *str)  
{  
  acoral_u32  i=0;  
  const acoral_char *p=str;
  while(*p++)  
      i++;  
  return  i;  
}

acoral_char *acoral_str_cpy(acoral_char *to, const acoral_char *from)
{
    acoral_char *rev = to;
    while(*to++ = *from++);
    return rev;
}

acoral_u32 acoral_str_cmp(const acoral_char * des,const  acoral_char * src)
{
    acoral_u32 c;
    acoral_u32 len1;
    acoral_u32 len2;
    if(des == NULL || src == NULL)
      	return -1;	
    len1 = acoral_str_len(des);
    len2 = acoral_str_len(src);
    if(len1 != len2)
    {
        return len1-len2;
    }
    while(*des != '\0')
    {
        if((c=(*des++ - * src++)) != 0)
        {
            return c;
        }
    }
    return c;
}

/**
 * acoral_str_nicmp - Case insensitive, length-limited string comparison
 * @s1: One string
 * @s2: The other string
 * @len: the maximum number of characters to compare
 */
acoral_32 acoral_str_nicmp(const acoral_char *s1, const acoral_char *s2, acoral_u32 len)
{
	/* Yes, Virginia, it had better be unsigned */
	acoral_u8 c1, c2;

	c1 = 0;	c2 = 0;
	if (len) {
		do {
			c1 = *s1; c2 = *s2;
			s1++; s2++;
			if (!c1)
				break;
			if (!c2)
				break;
			if (c1 == c2)
				continue;
			c1 = acoral_tolower(c1);
			c2 = acoral_tolower(c2);
			if (c1 != c2)
				break;
		} while (--len);
	}
	return (acoral_32)c1 - (acoral_32)c2;
}




/**
 * acoral_str_ncpy - Copy a length-limited, %NUL-terminated string
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @count: The maximum number of bytes to copy
 *
 * The result is not %NUL-terminated if the source exceeds
 * @count bytes.
 */
acoral_char * acoral_str_ncpy(acoral_char * dest,const acoral_char *src,acoral_u32 count)
{
	acoral_char *tmp = dest;

	while (count) {
		if ((*tmp = *src) == '\0')
			return dest;
		src++;
		tmp++;
		count--;
	}
	*tmp = '\0';
	return dest;
}



/**
 * acoral_str_lcpy - Copy a %NUL terminated string acoral_32o a sized buffer
 * @dest: Where to copy the string to
 * @src: Where to copy the string from
 * @size: size of destination buffer
 *
 * Compatible with *BSD: the result is always a valid
 * NUL-terminated string that fits in the buffer (unless,
 * of course, the buffer size is zero). It does not pad
 * out the result like acoral_str_ncpy() does.
 */
acoral_u32 acoral_str_lcpy(acoral_char *dest, const acoral_char *src, acoral_u32 size)
{
	acoral_u32 ret = acoral_str_len(src);

	if (size) {
		acoral_u32 len = (ret >= size) ? size-1 : ret;
		acoral_memcpy(dest, src, len);
		dest[len] = '\0';
	}
	return ret;
}


/**
 * acoral_str_cat - Append one %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 */
acoral_char * acoral_str_cat(acoral_char * dest, const acoral_char * src)
{
	acoral_char *tmp = dest;

	while (*dest)
		dest++;
	while ((*dest++ = *src++) != '\0')
		;

	return tmp;
}



/**
 * acoral_str_ncat - Append a length-limited, %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 * @count: The maximum numbers of bytes to copy
 *
 * Note that in contrast to acoral_str_ncpy, acoral_str_ncat ensures the result is
 * terminated.
 */
acoral_char * acoral_str_ncat(acoral_char *dest, const acoral_char *src, acoral_u32 count)
{
	acoral_char *tmp = dest;

	if (count) {
		while (*dest)
			dest++;
		while ((*dest++ = *src++) != 0) {
			if (--count == 0) {
				*dest = '\0';
				break;
			}
		}
	}

	return tmp;
}



/**
 * acoral_str_lcat - Append a length-limited, %NUL-terminated string to another
 * @dest: The string to be appended to
 * @src: The string to append to it
 * @count: The size of the destination buffer.
 */
acoral_u32 acoral_str_lcat(acoral_char *dest, const acoral_char *src, acoral_u32 count)
{
	acoral_u32 dsize = acoral_str_len(dest);
	acoral_u32 len = acoral_str_len(src);
	acoral_u32 res = dsize + len;

	/* This would be a bug */
//	BUG_ON(dsize >= count);

	dest += dsize;
	count -= dsize;
	if (len >= count)
		len = count-1;
	acoral_memcpy(dest, src, len);
	dest[len] = 0;
	return res;
}






/**
 * acoral_str_ncmp - Compare two length-limited strings
 * @cs: One string
 * @ct: Another string
 * @count: The maximum number of bytes to compare
 */
acoral_32 acoral_str_ncmp(const acoral_char * cs,const acoral_char * ct,acoral_u32 count)
{
	register acoral_8 __res = 0;

	while (count) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
		count--;
	}

	return __res;
}





/**
 * acoral_str_chr - Find the first occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 */
acoral_32  acoral_str_chr(const acoral_char * s, acoral_32 c)
{
	acoral_u32 pos = 1;
	for(; *s != (acoral_char) c; ++s,++pos)
		if (*s == '\0')
			return 0;
	return pos;
}



/**
 * acoral_str_rchr - Find the last occurrence of a character in a string
 * @s: The string to be searched
 * @c: The character to search for
 */
acoral_32  acoral_str_rchr(const acoral_char * s, acoral_32 c)
{
	   acoral_u32 pos = acoral_str_len(s);
       const acoral_char *p = s + pos;
       do {
           if (*p == (acoral_char)c)
               return pos;
           --pos;
       } while (--p >= s);
       return 0;
}



/**
 * acoral_str_nchr - Find a character in a length limited string
 * @s: The string to be searched
 * @count: The number of characters to be searched
 * @c: The character to search for
 */
acoral_32 acoral_str_nchr(const acoral_char *s, acoral_u32 count, acoral_32 c)
{
	acoral_u32 pos = 1;
	for (; count-- && *s != '\0'; ++s,++pos)
		if (*s == (acoral_char) c)
			return pos;
	return 0;
}




/**
 * acoral_str_nlen - Find the length of a length-limited string
 * @s: The string to be sized
 * @count: The maximum number of bytes to search
 */
acoral_u32 acoral_str_nlen(const acoral_char * s, acoral_u32 count)
{
	const acoral_char *sc;

	for (sc = s; count-- && *sc != '\0'; ++sc)
		/* nothing */;
	return sc - s;
}





/**
 * acoral_str_spn - Calculate the length of the initial substring of @s which only
 * 	contain letters in @accept
 * @s: The string to be searched
 * @accept: The string to search for
 */
acoral_u32 acoral_str_spn(const acoral_char *s, const acoral_char *accept)
{
	const acoral_char *p;
	const acoral_char *a;
	acoral_u32 count = 0;

	for (p = s; *p != '\0'; ++p) {
		for (a = accept; *a != '\0'; ++a) {
			if (*p == *a)
				break;
		}
		if (*a == '\0')
			return count;
		++count;
	}

	return count;
}




/**
 * acoral_str_cspn - Calculate the length of the initial substring of @s which does
 * 	not contain letters in @reject
 * @s: The string to be searched
 * @reject: The string to avoid
 */
acoral_u32 acoral_str_cspn(const acoral_char *s, const acoral_char *reject)
{
	const acoral_char *p;
	const acoral_char *r;
	acoral_u32 count = 0;

	for (p = s; *p != '\0'; ++p) {
		for (r = reject; *r != '\0'; ++r) {
			if (*p == *r)
				return count;
		}
		++count;
	}

	return count;
}	



/**
 * acoral_str_pbrk - Find the first occurrence of a set of characters
 * @cs: The string to be searched
 * @ct: The characters to search for
 */
acoral_char * acoral_str_pbrk(const acoral_char * cs,const acoral_char * ct)
{
	const acoral_char *sc1,*sc2;

	for( sc1 = cs; *sc1 != '\0'; ++sc1) {
		for( sc2 = ct; *sc2 != '\0'; ++sc2) {
			if (*sc1 == *sc2)
				return (acoral_char *) sc1;
		}
	}
	return NULL;
}





/**
 * acoral_str_sep - Split a string acoral_32o tokens
 * @s: The string to be searched
 * @ct: The characters to search for
 *
 * acoral_str_sep() updates @s to poacoral_32 after the token, ready for the next call.
 *
 * It returns empty tokens, too, behaving exactly like the libc function
 * of that name. In fact, it was stolen from glibc2 and de-fancy-fied.
 * Same semantics, slimmer shape. ;)
 */
acoral_char * acoral_str_sep(acoral_char **s, const acoral_char *ct)
{
	acoral_char *sbegin = *s, *end;

	if (sbegin == NULL)
		return NULL;

	end = acoral_str_pbrk(sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;

	return sbegin;
}



/**
 * memset - Fill a region of memory with the given value
 * @s: Poacoral_32er to the start of the area.
 * @c: The byte to fill the area with
 * @count: The size of the area.
 *
 * Do not use memset() to access IO space, use memset_io() instead.
 */
void * acoral_memset(void * s,acoral_char c,acoral_u32 count)
{
	acoral_char *xs = (acoral_char *) s;

	while (count--)
		*xs++ = c;

	return s;
}



/**
 * memcpy - Copy one area of memory to another
 * @dest: Where to copy to
 * @src: Where to copy from
 * @count: The size of the area.
 *
 * You should not use this function to access IO space, use memcpy_toio()
 * or memcpy_fromio() instead.
 */
void * acoral_memcpy(void * dest,const void *src,acoral_u32 count)
{
	acoral_char *tmp = (acoral_char *) dest, *s = (acoral_char *) src;

	while (count--)
		*tmp++ = *s++;

	return dest;
}



/**
 * memmove - Copy one area of memory to another
 * @dest: Where to copy to
 * @src: Where to copy from
 * @count: The size of the area.
 *
 * Unlike memcpy(), memmove() copes with overlapping areas.
 */
void * acoral_memmove(void * dest,const void *src,acoral_u32 count)
{
	acoral_char *tmp, *s;

	if (dest <= src) {
		tmp = (acoral_char *) dest;
		s = (acoral_char *) src;
		while (count--)
			*tmp++ = *s++;
		}
	else {
		tmp = (acoral_char *) dest + count;
		s = (acoral_char *) src + count;
		while (count--)
			*--tmp = *--s;
		}

	return dest;
}


/**
 * memcmp - Compare two areas of memory
 * @cs: One area of memory
 * @ct: Another area of memory
 * @count: The size of the area.
 */
acoral_32 acoral_memcmp(const void * cs,const void * ct,acoral_u32 count)
{
	const acoral_u8 *su1, *su2;
	acoral_32 res = 0;

	for( su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
		if ((res = *su1 - *su2) != 0)
			break;
	return res;
}




/**
 * memscan - Find a character in an area of memory.
 * @addr: The memory area
 * @c: The byte to search for
 * @size: The size of the area.
 *
 * returns the address of the first occurrence of @c, or 1 byte past
 * the area if @c is not found
 */
void * acoral_memscan(void * addr, acoral_32 c, acoral_u32 size)
{
	acoral_u8 * p = (acoral_u8 *) addr;

	while (size) {
		if (*p == c)
			return (void *) p;
		p++;
		size--;
	}
  	return (void *) p;
}



/**
 * acoral_strstr - Find the first substring in a %NUL terminated string
 * @s1: The string to be searched
 * @s2: The string to search for
 */
acoral_char * acoral_strstr(const acoral_char * s1,const acoral_char * s2)
{
	acoral_32 l1, l2;

	l2 = acoral_str_len(s2);
	if (!l2)
		return (acoral_char *) s1;
	l1 = acoral_str_len(s1);
	while (l1 >= l2) {
		l1--;
		if (!acoral_memcmp(s1,s2,l2))
			return (acoral_char *) s1;
		s1++;
	}
	return NULL;
}




/**
 * memchr - Find a character in an area of memory.
 * @s: The memory area
 * @c: The byte to search for
 * @n: The size of the area.
 *
 * returns the address of the first occurrence of @c, or %NULL
 * if @c is not found
 */
void *acoral_memchr(const void *s, acoral_32 c, acoral_u32 n)
{
	const acoral_u8 *p = s;
	while (n-- != 0) {
        	if ((acoral_u8)c == *p++) {
			return (void *)(p-1);
		}
	}
	return NULL;
}
