#include"acoral.h"
extern void console_init(void);
extern void sd_init(void);
extern void brd_driver_init();
#define DRV_MAIN_ID_SHIFT 5
#define DRV_MAIN_ID(id) (id>>DRV_MAIN_ID_SHIFT)
#define DRV_CHK_ID(id) (DRV_MAIN_ID(id)>=0&&DRV_MAIN_ID(id)<ACORAL_DEV_NUM)
acoral_pool_ctrl_t acoral_drv_pool_ctrl;
acoral_drv_ctrl_t *drv_table[ACORAL_DEV_NUM];
acoral_drv_ctrl_t *acoral_get_drv_ctrl_by_name(acoral_char *name){
  	acoral_u32 i;
	acoral_drv_ctrl_t *ctrl;
	for(i=0;i<ACORAL_DEV_NUM;i++){
	  	ctrl=drv_table[i];	    
		if(ctrl!=NULL&&!acoral_str_cmp(ctrl->name,name))
		   return ctrl;
	}
	return NULL;
}

acoral_err acoral_drv_register(acoral_dev_ops_t *ops,acoral_char *name){
	acoral_u32 i;
	acoral_drv_ctrl_t *tmp,*ctrl;
	ctrl=acoral_alloc_drv();
	if(ctrl==NULL)
		return ACORAL_ERR_NO_DRV_RES;
	ctrl->name=name;
	ctrl->ops=ops;
	for(i=0;i<ACORAL_DEV_NUM;i++){
	  	tmp=drv_table[i];	    
		if(tmp==NULL){
			drv_table[i]=ctrl;
			ctrl->drv_id=i<<DRV_MAIN_ID_SHIFT;
			break;
		}
	}
	if(i==ACORAL_DEV_NUM)
		return ACORAL_ERR_DRV_FULL;
	return 0;
}

void acoral_drv_unregister(acoral_id res_id){
	acoral_drv_ctrl_t *ctrl=(acoral_drv_ctrl_t *)acoral_get_res_by_id(res_id);
	if(ctrl->ops!=NULL&&ctrl->ops->close!=NULL)
		ctrl->ops->close();
	drv_table[DRV_MAIN_ID(ctrl->drv_id)]=NULL;
	acoral_release_res((acoral_res_t *)ctrl);
}

acoral_id acoral_dev_open(acoral_char *name){
	acoral_drv_ctrl_t *ctrl=acoral_get_drv_ctrl_by_name(name);
	if(ctrl==NULL)
		return ACORAL_DEV_ERR_ID;
	if(ctrl->ops!=NULL&&ctrl->ops->open!=NULL){
		if(ctrl->ops->open()!=0)
			return ACORAL_DEV_ERR_ID;
	}
	return ctrl->res.id;
}

acoral_32 acoral_dev_write(acoral_id id,void *data,acoral_size size,acoral_size offset,acoral_time time_out){
	acoral_drv_ctrl_t *ctrl=(acoral_drv_ctrl_t *)acoral_get_res_by_id(id);
	if(ctrl!=NULL&&ctrl->ops!=NULL&&ctrl->ops->write!=NULL)
		return ctrl->ops->write(data,size,offset,time_out);
	return 0;
}

acoral_32 acoral_dev_read(acoral_id id,void *data,acoral_size size,acoral_size offset,acoral_time time_out){
	acoral_drv_ctrl_t *ctrl=(acoral_drv_ctrl_t *)acoral_get_res_by_id(id);
	if(ctrl!=NULL&&ctrl->ops!=NULL&&ctrl->ops->read!=NULL)
		return ctrl->ops->read(data,size,offset,time_out);
	return 0;
}

acoral_32 acoral_dev_config(acoral_id id,acoral_u32 cmd,void *data,acoral_time tm_out){
	acoral_drv_ctrl_t *ctrl=(acoral_drv_ctrl_t *)acoral_get_res_by_id(id);
	if(ctrl!=NULL&&ctrl->ops!=NULL&&ctrl->ops->config!=NULL)
		return ctrl->ops->config(cmd,data,tm_out);
	return 0;	
}
acoral_32 acoral_dev_close(acoral_id id){
	acoral_drv_ctrl_t *ctrl=(acoral_drv_ctrl_t *)acoral_get_res_by_id(id);
	if(ctrl!=NULL&&ctrl->ops!=NULL&&ctrl->ops->close!=NULL)
		return ctrl->ops->close();
	return 0;
}

acoral_drv_ctrl_t *acoral_alloc_drv(){
  	return (acoral_drv_ctrl_t *)acoral_get_res(&acoral_drv_pool_ctrl);
}


void acoral_drv_pool_init(){
	acoral_drv_pool_ctrl.type=ACORAL_RES_DRIVER;
	acoral_drv_pool_ctrl.size=sizeof(acoral_drv_ctrl_t);
	acoral_drv_pool_ctrl.num_per_pool=10;
	acoral_drv_pool_ctrl.max_pools=3;
	acoral_pool_ctrl_init(&acoral_drv_pool_ctrl);
}

void acoral_drv_sys_init(){
	acoral_u32 i;
	for(i=0;i<ACORAL_DEV_NUM;i++)
	  	drv_table[i]=NULL;	    
	acoral_drv_pool_init();
	brd_driver_init();
}
