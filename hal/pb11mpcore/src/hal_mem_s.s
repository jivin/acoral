@**************************************************************
@ MMU Cache/TLB/etc on/off functions
@**************************************************************
.equ 	R1_I	,	1<<12
.equ 	R1_C	,	1<<2
.equ 	R1_A	,	1<<1
.equ 	R1_M    ,	1
.equ 	BUS_M, 	3<<30

   .global EnableICache
EnableICache:
   mrc p15,0,r0,c1,c0,0
   orr r0,r0,#R1_I
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void DisableICache(void)
@**************************************************************
   .global DisableICache
DisableICache:
   mrc p15,0,r0,c1,c0,0
   bic r0,r0,#R1_I
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void EnableDCache(void)
@**************************************************************
   .global EnableDCache
EnableDCache:
   mrc p15,0,r0,c1,c0,0
   orr r0,r0,#R1_C
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void DisableDCache(void)
@**************************************************************
   .global DisableDCache
DisableDCache:
   mrc p15,0,r0,c1,c0,0
   bic r0,r0,#R1_C
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void EnableAlignFault(void)
@**************************************************************
   .global EnableAlignFault
EnableAlignFault:
   mrc p15,0,r0,c1,c0,0
   orr r0,r0,#R1_A
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void DisableAlignFault(void)
@**************************************************************
   .global DisableAlignFault
DisableAlignFault:
   mrc p15,0,r0,c1,c0,0
   bic r0,r0,#R1_A
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void EnableMMU(void)
@**************************************************************
   .global EnableMMU
EnableMMU:
   mrc p15,0,r0,c1,c0,0
   orr r0,r0,#R1_M
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@void DisableMMU(void)
@**************************************************************
   .global DisableMMU
DisableMMU:
   mrc p15,0,r0,c1,c0,0
   bic r0,r0,#R1_M
   mcr p15,0,r0,c1,c0,0
   mov pc ,lr

@**************************************************************
@ Set TTBase
@ ro=TTBase
@void SetTTBase(int base)
@**************************************************************
   .global SetTTBase
SetTTBase:
   mcr p15,0,r0,c2,c0,0
   mov pc ,lr

@**************************************************************
@ Set Domain
@void SetDomain(int domain)
@ro=domain
@**************************************************************
   .global SetDomain
SetDomain:
   mcr p15,0,r0,c3,c0,0
   mov pc ,lr

@**************************************************************
@ ICache/DCache functions
@void InvalidateIDCache(void)
@**************************************************************
   .global InvalidateIDCache
InvalidateIDCache:
   mcr p15,0,r0,c7,c7,0
   mov pc ,lr

@**************************************************************
@void InvalidateICache(void)
@**************************************************************
   .global InvalidateICache
InvalidateICache:
   mcr p15,0,r0,c7,c5,0
   mov pc ,lr

@**************************************************************
@void InvalidateICacheMVA(U32 mva)
@r0=mva
@**************************************************************
   .global InvalidateICacheMVA
InvalidateICacheMVA:
   mcr p15,0,r0,c7,c5,1
   mov pc ,lr

@**************************************************************
@void PrefetchICacheMVA(U32 mva)
@r0=mva
@**************************************************************
   .global PrefetchICacheMVA
PrefetchICacheMVA:
   mcr p15,0,r0,c7,c13,1
   mov pc ,lr

@**************************************************************
@void InvalidateDCache(void)
@**************************************************************
   .global InvalidateDCache
InvalidateDCache:
   mcr p15,0,r0,c7,c6,0
   mov pc ,lr

@**************************************************************
@void InvalidateDCacheMVA(U32 mva)
@**************************************************************
   .global InvalidateDCacheMVA
InvalidateDCacheMVA:
   @r0=mva
   mcr p15,0,r0,c7,c6,1
   mov pc ,lr

@**************************************************************
@void CleanDCacheMVA(U32 mva)
@**************************************************************
   .global CleanDCacheMVA
CleanDCacheMVA:
   @r0=mva
   mcr p15,0,r0,c7,c10,1
   mov pc ,lr

@**************************************************************
@void CleanInvalidateDCacheMVA(U32 mva)
@r0=mva
@**************************************************************
   .global CleanInvalidateDCacheMVA
CleanInvalidateDCacheMVA:
   mcr p15,0,r0,c7,c14,1
   mov pc ,lr

@**************************************************************
@void CleanDCacheIndex(U32 index)
@r0=index
@**************************************************************
   .global CleanDCacheIndex
CleanDCacheIndex:
   mcr p15,0,r0,c7,c10,2
   mov pc ,lr

@**************************************************************
@void CleanInvalidateDCacheIndex(U32 index)
@r0=index
@**************************************************************
   .global CleanInvalidateDCacheIndex
CleanInvalidateDCacheIndex:
   mcr p15,0,r0,c7,c14,2
   mov pc ,lr

@**************************************************************
@void WaitForInterrupt(void)
@**************************************************************
   .global WaitForInterrupt
WaitForInterrupt:
   mcr p15,0,r0,c7,c0,4
   mov pc ,lr

@**************************************************************
@ TLB functions
@voic InvalidateTLB(void)
@**************************************************************
   .global InvalidateTLB
InvalidateTLB:
   mcr p15,0,r0,c8,c7,0
   mov pc ,lr

@**************************************************************
@void InvalidateITLB(void)
@**************************************************************
   .global InvalidateITLB
InvalidateITLB:
   mcr p15,0,r0,c8,c5,0
   mov pc ,lr

@**************************************************************
@void InvalidateITLBMVA(U32 mva)
@ro=mva
@**************************************************************
   .global InvalidateITLBMVA
InvalidateITLBMVA:
   mcr p15,0,r0,c8,c5,1
   mov pc ,lr

@**************************************************************
@void InvalidateDTLB(void)
@**************************************************************
	.global InvalidateDTLB
InvalidateDTLB:
	mcr p15,0,r0,c8,c6,0
	mov pc ,lr

@**************************************************************
@ Process ID
@void SetProcessId(U32 pid)
@r0= pid
@**************************************************************
   .global SetProcessId
SetProcessId:
   mcr p15,0,r0,c13,c0,0
   mov pc ,lr

