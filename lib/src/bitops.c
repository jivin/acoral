#include<type.h>
#include<bitops.h>
acoral_u32 acoral_ffs(acoral_u32 word)
{
	acoral_u32 k;
	k = 31;
	if (word & 0x0000ffff) { k -= 16; word <<= 16; }
	if (word & 0x00ff0000) { k -= 8;  word <<= 8;  }
	if (word & 0x0f000000) { k -= 4;  word <<= 4;  }
	if (word & 0x30000000) { k -= 2;  word <<= 2;  }
	if (word & 0x40000000) { k -= 1; }
        return k;
}

acoral_u32 acoral_find_first_bit(const acoral_u32 *b,acoral_u32 length)
{
	acoral_u32 v;
	acoral_u32 off;

	for (off = 0; v = b[off], off < length; off++) {
		if (v)
			break;
	}
	return acoral_ffs(v) + off * 32;
}

void acoral_set_bit(acoral_32 nr,acoral_u32 *bitmap)
{
	acoral_u32 oldval, mask = 1UL << (nr & 31);
	acoral_u32 *p;
	p =bitmap+(nr>>5);
	oldval = *p;
	*p = oldval | mask;
}

void acoral_clear_bit(acoral_32 nr,acoral_u32 *bitmap)
{
	acoral_u32 oldval, mask = 1UL << (nr & 31);
	acoral_u32 *p;
	p =bitmap+(nr >> 5);
	oldval = *p;
	*p = oldval &(~mask);
}

acoral_u32 acoral_get_bit(acoral_32 nr,acoral_u32 *bitmap)
{
	acoral_u32 oldval, mask = 1UL << (nr & 31);
	acoral_u32 *p;
	p =bitmap+(nr>>5);
	oldval = *p;
	return oldval & mask;
}
