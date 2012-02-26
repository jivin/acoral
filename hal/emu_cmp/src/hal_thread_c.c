#include<acoral.h>
#include "sig.h"
typedef struct {
  	acoral_u32 eax;
	acoral_u32 ebx;
	acoral_u32 ecx;
	acoral_u32 edx;
	acoral_u32 esi;
	acoral_u32 edi;
	acoral_u32 eflags;
	acoral_u32 ebp;
	acoral_u32 intr_state;
	acoral_u32 eip;
}hal_ctx_t;

extern acoral_u32 *IRQ_stack[];
void hal_sig_ret(){
  	__asm__ __volatile__("popl %%eax\n\t"\
	"mov $0x77,%%eax\n\t"               \
	"int $0x80\n\t"               \
	:          \
	);

}

void hal_stack_init(acoral_u32 **stk,void (*route)(),void (*exit)(),void *args){
	hal_ctx_t *stack;
	struct sigframe *frame;
	acoral_u32 *tmp=*stk;
	*tmp--=args;
	*tmp=exit;
	stack=(hal_ctx_t *)tmp;
	stack--;
	stack->eip=(acoral_u32 *)route;
	stack->eax=0;
	stack->ebx=1;
	stack->ecx=2;
	stack->edx=3;
	stack->intr_state=true;
	*stk=stack;
	//The tree line code is special for x86 emulation,if not,will error.
	frame=stack;
    frame--;
	acoral_memset(frame,0,sizeof(struct sigframe));
}

void hal_intr_ctx_switch(acoral_u32 **prev,acoral_u32 **next){
  acoral_u32 i;
  hal_ctx_t *tmp=IRQ_stack[acoral_current_cpu];
  acoral_u32 *ctx=tmp;
  *(--ctx)=true;
  tmp--;
  ctx=tmp;
  ctx++;
  *prev=ctx;
   hal_switch_to(next);
}

void hal_switch_to(acoral_u32 **next){
  struct sigframe *frame;
  hal_ctx_t *tmp=*next;
  acoral_u32 *ctx;
  frame=*next;
  if(tmp->intr_state){
      frame--;
      frame->sc.eip=tmp->eip;
      frame->sc.esp=tmp+1;
      frame->sc.esp_at_signal=tmp+1;
      frame->sc.eax=tmp->eax;
      frame->sc.ebx=tmp->ebx;
      frame->sc.ecx=tmp->ecx;
      frame->sc.edx=tmp->edx;
      frame->sc.esi=tmp->esi;
      frame->sc.edi=tmp->edi;
      frame->sc.cs=0x73;
      frame->sc.ds=0x7b;
      frame->sc.ss=0x7b;;
      frame->sc.fs=0x0;
      frame->sc.gs=0x33;
      frame->sc.eflags=0x246;
#ifdef CFG_CMP
      frame->sc.oldmask=1<<HAL_TICKS_INTR|(1<<HAL_CMD_IPI);
#else
      frame->sc.oldmask=1<<HAL_TICKS_INTR;
#endif
      frame->pretcode=(char *)hal_sig_ret;
      ctx=frame;
      *(--ctx)=tmp->intr_state;
      tmp=frame;
      tmp--;
      ctx=tmp;
      ctx++;
      *next=ctx;
   }
   HAL_REAL_SWITCH_TO(next);
}
