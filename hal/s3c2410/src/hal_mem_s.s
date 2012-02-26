@**************************************************************
@ MMU Cache/TLB/etc on/off functions
@**************************************************************
.equ 	R1_I	,	1<<12
.equ 	R1_C	,	1<<2
.equ 	R1_A	,	1<<1
.equ 	R1_M    ,	1
.equ 	BUS_M, 	3<<30

   .global MMU_EnableICache
MMU_EnableICache:
   mrc p15,0,r0,c1,c0,0
   orr r0,r0,#R1_I
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void MMU_DisableICache(void)
@**************************************************************
   .global MMU_DisableICache
MMU_DisableICache:
   mrc p15,0,r0,c1,c0,0
   bic r0,r0,#R1_I
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void MMU_EnableDCache(void)
@**************************************************************
   .global MMU_EnableDCache
MMU_EnableDCache:
   mrc p15,0,r0,c1,c0,0
   orr r0,r0,#R1_C
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void MMU_DisableDCache(void)
@**************************************************************
   .global MMU_DisableDCache
MMU_DisableDCache:
   mrc p15,0,r0,c1,c0,0
   bic r0,r0,#R1_C
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void MMU_EnableAlignFault(void)
@**************************************************************
   .global MMU_EnableAlignFault
MMU_EnableAlignFault:
   mrc p15,0,r0,c1,c0,0
   orr r0,r0,#R1_A
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void MMU_DisableAlignFault(void)
@**************************************************************
   .global MMU_DisableAlignFault
MMU_DisableAlignFault:
   mrc p15,0,r0,c1,c0,0
   bic r0,r0,#R1_A
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void MMU_EnableMMU(void)
@**************************************************************
   .global MMU_EnableMMU
MMU_EnableMMU:
   mrc p15,0,r0,c1,c0,0
   orr r0,r0,#R1_M
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void MMU_DisableMMU(void)
@**************************************************************
   .global MMU_DisableMMU
MMU_DisableMMU:
   mrc p15,0,r0,c1,c0,0
   bic r0,r0,#R1_M
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@ Set TTBase
@ ro=TTBase
@void MMU_SetTTBase(int base)
@**************************************************************
   .global MMU_SetTTBase
MMU_SetTTBase:
   mcr p15,0,r0,c2,c0,0
   mov pc ,lr

@**************************************************************
@ Set Domain
@void MMU_SetDomain(int domain)
@ro=domain
@**************************************************************
   .global MMU_SetDomain
MMU_SetDomain:
   mcr p15,0,r0,c3,c0,0
   mov pc ,lr

@**************************************************************
@ ICache/DCache functions
@void MMU_InvalidateIDCache(void)
@**************************************************************
   .global MMU_InvalidateIDCache
MMU_InvalidateIDCache:
   mcr p15,0,r0,c7,c7,0
   mov pc ,lr

@**************************************************************
@void MMU_InvalidateICache(void)
@**************************************************************
   .global MMU_InvalidateICache
MMU_InvalidateICache:
   mcr p15,0,r0,c7,c5,0
   mov pc ,lr

@**************************************************************
@void MMU_InvalidateICacheMVA(U32 mva)
@r0=mva
@**************************************************************
   .global MMU_InvalidateICacheMVA
MMU_InvalidateICacheMVA:
   mcr p15,0,r0,c7,c5,1
   mov pc ,lr

@**************************************************************
@void MMU_PrefetchICacheMVA(U32 mva)
@r0=mva
@**************************************************************
   .global MMU_PrefetchICacheMVA
MMU_PrefetchICacheMVA:
   mcr p15,0,r0,c7,c13,1
   mov pc ,lr

@**************************************************************
@void MMU_InvalidateDCache(void)
@**************************************************************
   .global MMU_InvalidateDCache
MMU_InvalidateDCache:
   mcr p15,0,r0,c7,c6,0
   mov pc ,lr

@**************************************************************
@void MMU_InvalidateDCacheMVA(U32 mva)
@**************************************************************
   .global MMU_InvalidateDCacheMVA
MMU_InvalidateDCacheMVA:
   @r0=mva
   mcr p15,0,r0,c7,c6,1
   mov pc ,lr

@**************************************************************
@void MMU_CleanDCacheMVA(U32 mva)
@**************************************************************
   .global MMU_CleanDCacheMVA
MMU_CleanDCacheMVA:
   @r0=mva
   mcr p15,0,r0,c7,c10,1
   mov pc ,lr

@**************************************************************
@void MMU_CleanInvalidateDCacheMVA(U32 mva)
@r0=mva
@**************************************************************
   .global MMU_CleanInvalidateDCacheMVA
MMU_CleanInvalidateDCacheMVA:
   mcr p15,0,r0,c7,c14,1
   mov pc ,lr

@**************************************************************
@void MMU_CleanDCacheIndex(U32 index)
@r0=index
@**************************************************************
   .global MMU_CleanDCacheIndex
MMU_CleanDCacheIndex:
   mcr p15,0,r0,c7,c10,2
   mov pc ,lr

@**************************************************************
@void MMU_CleanInvalidateDCacheIndex(U32 index)
@r0=index
@**************************************************************
   .global MMU_CleanInvalidateDCacheIndex
MMU_CleanInvalidateDCacheIndex:
   mcr p15,0,r0,c7,c14,2
   mov pc ,lr

@**************************************************************
@void MMU_WaitForInterrupt(void)
@**************************************************************
   .global MMU_WaitForInterrupt
MMU_WaitForInterrupt:
   mcr p15,0,r0,c7,c0,4
   mov pc ,lr

@**************************************************************
@ TLB functions
@voic MMU_InvalidateTLB(void)
@**************************************************************
   .global MMU_InvalidateTLB
MMU_InvalidateTLB:
   mcr p15,0,r0,c8,c7,0
   mov pc ,lr

@**************************************************************
@void MMU_InvalidateITLB(void)
@**************************************************************
   .global MMU_InvalidateITLB
MMU_InvalidateITLB:
   mcr p15,0,r0,c8,c5,0
   mov pc ,lr

@**************************************************************
@void MMU_InvalidateITLBMVA(U32 mva)
@ro=mva
@**************************************************************
   .global MMU_InvalidateITLBMVA
MMU_InvalidateITLBMVA:
   mcr p15,0,r0,c8,c5,1
   mov pc ,lr

@**************************************************************
@void MMU_InvalidateDTLB(void)
@**************************************************************
	.global MMU_InvalidateDTLB
MMU_InvalidateDTLB:
	mcr p15,0,r0,c8,c6,0
	mov pc ,lr

@**************************************************************
@ Process ID
@void MMU_SetProcessId(U32 pid)
@r0= pid
@**************************************************************
   .global MMU_SetProcessId
MMU_SetProcessId:
   mcr p15,0,r0,c13,c0,0
   mov pc ,lr

