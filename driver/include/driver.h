#ifndef ACORAL_DRIVER_H
#define ACORAL_DRIVER_H
#include<type.h>
#include<resource.h>
#define ACORAL_DEV_OPEN_OK 0
#define ACORAL_DEV_ERR_ID -1
#define ACORAL_ERR_NO_DRV_RES 5
#define ACORAL_ERR_DRV_FULL 5
#define ACORAL_DEV_NUM 20
typedef struct acoral_dev_operation{
	acoral_32 (*open)(void);
	acoral_32 (*read)(void *data,acoral_size size,acoral_size offset,acoral_time time_out);
	acoral_32 (*write)(void *data,acoral_size size,acoral_size offset,acoral_time time_out);
	acoral_32 (*config)(acoral_u32 cmd,void *data,acoral_time time_out);
	acoral_32 (*close)(void);
}acoral_dev_ops_t;

typedef struct acoral_drv_ctrl{
	acoral_res_t res;
	acoral_dev_ops_t *ops;
	acoral_u32 drv_id;
	acoral_char *name;
}acoral_drv_ctrl_t;

acoral_drv_ctrl_t *acoral_get_drv_ctrl_by_name(acoral_char *name);
void acoral_drv_sys_init(void);
acoral_drv_ctrl_t *acoral_alloc_drv(void);
acoral_err acoral_drv_register(acoral_dev_ops_t *ops,acoral_char *name);
void acoral_drv_unregister(acoral_id res_id);
acoral_id acoral_dev_open(acoral_char *name);
acoral_32 acoral_dev_write(acoral_id id,void *data,acoral_size size,acoral_size offset,acoral_time tm_out);
acoral_32 acoral_dev_read(acoral_id id,void *data,acoral_size size,acoral_size offset,acoral_time tm_out);
acoral_32 acoral_dev_config(acoral_id id,acoral_u32 cmd,void *data,acoral_time tm_out);
acoral_32 acoral_dev_close(acoral_id id);
#endif
