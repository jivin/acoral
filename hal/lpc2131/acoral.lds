ENTRY(__ENTRY)
MEMORY
{  
	rom(rx)  :  org = 0x00000000,  len = 32K
	ram(rwx)  :  org = 0x40000000, len = 8K
}
SECTIONS
{
	.text :
	{
		* (.text)
		* (.init.text)
		* (.rodata*)
		. = ALIGN(4);
		text_end = .;
	}>rom 

	.data : AT( text_end ){
		data_start = .;
		* (.vector)
		. = ALIGN(4);
		*(.acoral1.call) 
		*(.acoral2.call) 
		*(.acoral3.call) 
		*(.acoral4.call) 
		*(.acoral5.call) 
		*(.acoral6.call) 
		*(.acoral7.call) 
		*(.acoral8.call) 
		*(.acoral9.call) 
		*(.acoral10.call) 
		*(.data)
		*(.data.rel)
		*(.got)
		*(.got.plt)
		data_end = .;
	}>ram 
	
	.bss ALIGN(4):
	{ 
	 	bss_start = .;
	  	* (.bss*)
	}>ram

	bss_end = .;    
	stack_base = 0x40002000 - 4;
     	SYS_stack_size   =  0x200;      
	SVC_stack_size   =  0x200;    
	Undef_stack_size =  0x100;        
	Abort_stack_size =  0x100;     
	IRQ_stack_size   =  0x200;       
	FIQ_stack_size   =  0x0;   

	FIQ_stack        =  stack_base; 
	IRQ_stack        =  FIQ_stack   - FIQ_stack_size;  
	ABT_stack      =  IRQ_stack   - IRQ_stack_size;  
	UDF_stack      =  ABT_stack - Abort_stack_size;    
	SVC_stack        =  UDF_stack - Undef_stack_size;    
	SYS_stack        =  SVC_stack   - SVC_stack_size;  

	heap_start = (bss_end + 3)&( ~ 3);  
	heap_end = (SYS_stack - SYS_stack_size)&( ~ 3) ;
}  
