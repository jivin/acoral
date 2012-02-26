#include"acoral.h"
void hal_scu_enable(void)
{
	acoral_u32 scu_ctrl;
	scu_ctrl = rSCUCTRL;
	if (!(scu_ctrl & 1)) {
		/* not enabled yet */
		scu_ctrl |= 1;
		rSCUCTRL=scu_ctrl;
	}
}
