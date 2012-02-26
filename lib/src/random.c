#include<acoral.h>
static acoral_u32 rand_seed;
acoral_u32 acoral_rand(void)
{
	rand_seed=(rand_seed*123+59)%65536;
	return rand_seed;
}

void acoral_rand_start(void)
{
	rand_seed=acoral_ticks;
}
