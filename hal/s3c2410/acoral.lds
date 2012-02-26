ENTRY(__ENTRY)
MEMORY
{  
	    ram (wx)  : org = 0x030000000,   len = 64M
}
SECTIONS
{
	.text : 
	{
		text_start = .;
	       	* (.text)
     		* (.init.text)
		* (.rodata*)
	}>ram 
	
	.data ALIGN(4):
	{  
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
	} >ram
	
	.bss ALIGN(4): 
	{ 
		bss_start = .;    
		* (.bss)
     		. = ALIGN(4) ;
	} >ram
	bss_end = .;    

	stack_base = 0x33ffff00;
	MMU_base   =   0x33f00000;

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
	heap_end = MMU_base - 0x1000;
}  
