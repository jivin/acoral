#include<type.h>
typedef struct {
	acoral_u32 primask;
	acoral_u32 r4;
	acoral_u32 r5;
	acoral_u32 r6;
	acoral_u32 r7;
	acoral_u32 r8;
	acoral_u32 r9;
	acoral_u32 r10;
	acoral_u32 r11;
	acoral_u32 r0;
	acoral_u32 r1;
	acoral_u32 r2;
	acoral_u32 r3;
	acoral_u32 r12;
	acoral_u32 lr;
	acoral_u32 pc;
  	acoral_u32 psr;
}hal_ctx_t;


void hal_stack_init(acoral_u32 **stk,void (*route)(),void (*exit)(),void *args){
  	hal_ctx_t *ctx=(hal_ctx_t *)*stk;	
	ctx--;
	ctx=(hal_ctx_t *)((acoral_u32 *)ctx+1);
	ctx->r0=(acoral_u32)args;
	ctx->r1=1;
	ctx->r2=2;
	ctx->r3=3;
	ctx->r4=4;
	ctx->r5=5;
	ctx->r6=6;
	ctx->r7=7;
	ctx->r8=8;
	ctx->r9=9;
	ctx->r10=10;
	ctx->r11=11;
	ctx->r12=12;
	ctx->lr=(acoral_u32)exit;
	ctx->pc=(acoral_u32)route;
	ctx->psr=0x01000000L;
	ctx->primask=0;
        *stk=(acoral_u32 *)ctx;
}

