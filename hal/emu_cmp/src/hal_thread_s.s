.text
.globl HAL_REAL_SWITCH_TO,HAL_CONTEXT_SWITCH,HAL_SIG_RET
HAL_REAL_SWITCH_TO:
     mov 0x4(%esp),%eax
     mov (%eax),%eax
     mov 4(%eax),%ebx
     mov 8(%eax),%ecx
     mov 12(%eax),%edx
     mov 16(%eax),%esi
     mov 20(%eax),%edi
     mov 28(%eax),%ebp
#  mov 24(%eax),%eflags
     mov %eax,%esp
     mov 32(%esp),%ebx
     mov %ebx,(%eax)
     mov (%esp),%eax
     add $36,%esp
     ret
    
HAL_CONTEXT_SWITCH:
     sub $36,%sp
     mov %eax,(%esp)
     mov %ebx,4(%esp)
     mov %ecx,8(%esp)
     mov %edx,12(%esp)
     mov %esi,16(%esp)
     mov %edi,20(%esp)
     mov %ebp,28(%esp)
     mov $0,%eax
     mov %eax,32(%esp)
     mov 40(%esp),%eax
     mov %esp,(%eax)
     mov 44(%esp),%eax
     push %eax
     call hal_switch_to      
     ret

HAL_SIG_RET:
     pop    %eax
     mov    $0x77,%eax
     int    $0x80
     ret
