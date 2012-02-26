#include<queue.h>
#include<list.h>
#include<bitops.h>
void acoral_prio_queue_add(acoral_prio_array_t *array,acoral_u8 prio,acoral_list_t *list){
	acoral_queue_t *queue;
	acoral_list_t *head;
	array->num++;
        queue=array->queue + prio;
	head=&queue->head;
	acoral_list_add2_tail(list,head);
	acoral_set_bit(prio,array->bitmap);
}

void acoral_prio_queue_del(acoral_prio_array_t *array,acoral_u8 prio,acoral_list_t *list){
	acoral_queue_t *queue;
	acoral_list_t *head;
        queue= array->queue + prio;
	head=&queue->head;
	array->num--;
	acoral_list_del(list);
	if(acoral_list_empty(head))
		acoral_clear_bit(prio,array->bitmap);
}

acoral_u32 acoral_get_highprio(acoral_prio_array_t *array){
	return acoral_find_first_bit(array->bitmap,PRIO_BITMAP_SIZE);
}

void acoral_prio_queue_init(acoral_prio_array_t *array){
  	acoral_u8 i;
	acoral_queue_t *queue;
	acoral_list_t *head;
	array->num=0;
	for(i=0;i<PRIO_BITMAP_SIZE;i++)
	    array->bitmap[i]=0;
  	for(i=0;i<ACORAL_MAX_PRIO_NUM;i++){
	    queue= array->queue + i;
	    head=&queue->head;
	    acoral_init_list(head);
	} 	
}
