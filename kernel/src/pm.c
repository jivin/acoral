#include<hal.h>
void acoral_suspend(){
	HAL_SUSP_PREPARE();
	HAL_SUSP_ENTER();
	HAL_SUSP_FINISH();
}
