.text
.globl HAL_INTR_ENTRY,IRQ_stack
.extern acoral_intr_intry
HAL_INTR_ENTRY:
#    mov $IRQ_stack,%eax
#    mov %esp,(%eax)
    pushl %esp
    call intr_special
    popl %eax
    mov 0x4(%esp),%eax
    pushl %eax
	call acoral_intr_entry
    call acoral_intr_exit
    popl %eax
    ret
