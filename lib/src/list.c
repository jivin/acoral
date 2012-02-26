#include<list.h>
void acoral_list_add(acoral_list_t *new, acoral_list_t *head)
{
	new->prev=head;	
	new->next=head->next;	
	head->next->prev=new;	
	head->next=new;	
}

void acoral_list_add2_tail(acoral_list_t *new, acoral_list_t *head)
{
	new->prev=head->prev;
	new->next=head;
	head->prev->next=new;
	head->prev=new;
}

void acoral_list_del(acoral_list_t *entry)
{
	entry->prev->next= entry->next;
	entry->next->prev=entry->prev;
	entry->next = entry;
	entry->prev = entry;
}
