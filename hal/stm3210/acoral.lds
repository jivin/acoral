ENTRY(__ENTRY)
MEMORY
{  
	rom(rx)  :  org = 0x08000000,  len = 128K
	ram(rwx)  :  org = 0x20000000,  len = 20K
}

SECTIONS
{
	.text :
	{
		* (.text)
		* (.init.text)
		* (.rodata*)
		. = ALIGN(4);
	}>rom 
	text_end = .;

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

		. = ALIGN(4);
	       	* (.data)
		*(.data.rel)
		*(.got)
		*(.got.plt)
	}>ram 
	data_end = .;
	
	.bss ALIGN(4):
	{ 
		bss_start = .;
		* (.bss*)
     		. = ALIGN(4) ;
	}>ram
	bss_end = .;    
	
	stack_base = 0x20005000 - 4;
	Msp_size   =  0x200;      
	Psp_size   =  0x200;    

	Msp_stack        =  stack_base; 
	Psp_stack        =  Msp_stack   - Msp_size;  

	heap_start = (bss_end + 3)&( ~ 3);  
	heap_end = Psp_stack - Psp_size;
}  
