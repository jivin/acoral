.cpu cortex-m3
.fpu softvfp 
.syntax unified
.thumb

.text
.global     HAL_INTR_ENTRY
.global     HAL_INTR_TABLE
.global     HAL_INTR_DISABLE
.global     HAL_INTR_ENABLE
.global     HAL_INTR_RESTORE
.global     HAL_INTR_DISABLE_SAVE

.type HAL_INTR_DISABLE ,function
.type HAL_INTR_ENABLE ,function
.type HAL_INTR_RESTORE ,function
.type HAL_INTR_DISABLE_SAVE,function

.type HAL_INTR_ENTRY ,function
.type EXP_HANDLER ,function

.section .vector
HAL_INTR_TABLE:
.word  Msp_stack
.word  RESET_HANDLER
.word  EXP_HANDLER @NMI_Handler
.word  EXP_HANDLER  @HardFault_Handler
.word  EXP_HANDLER  @MemManage_Handler
.word  EXP_HANDLER  @BusFault_Handler
.word  EXP_HANDLER  @UsageFault_Handler
.word  0
.word  0
.word  0
.word  0
.word  EXP_HANDLER  @svc
.word  EXP_HANDLER  @DebugMon_Handler
.word  0
.word  PENDSV_CALL  @pendsv
.word  HAL_INTR_ENTRY  @SysTick_Handler
.word  HAL_INTR_ENTRY  @WWDG_IRQHandler
.word  HAL_INTR_ENTRY  @PVD_IRQHandler
.word  HAL_INTR_ENTRY  @TAMPER_IRQHandler
.word  HAL_INTR_ENTRY  @RTC_IRQHandler
.word  HAL_INTR_ENTRY  @FLASH_IRQHandler
.word  HAL_INTR_ENTRY  @RCC_IRQHandler
.word  HAL_INTR_ENTRY  @EXTI0_IRQHandler
.word  HAL_INTR_ENTRY  @EXTI1_IRQHandler
.word  HAL_INTR_ENTRY  @EXTI2_IRQHandler
.word  HAL_INTR_ENTRY  @EXTI3_IRQHandler
.word  HAL_INTR_ENTRY  @EXTI4_IRQHandler
.word  HAL_INTR_ENTRY  @DMA1_Channel1_IRQHandler
.word  HAL_INTR_ENTRY  @DMA1_Channel2_IRQHandler
.word  HAL_INTR_ENTRY  @DMA1_Channel3_IRQHandler
.word  HAL_INTR_ENTRY  @DMA1_Channel4_IRQHandler
.word  HAL_INTR_ENTRY  @DMA1_Channel5_IRQHandler
.word  HAL_INTR_ENTRY  @DMA1_Channel6_IRQHandler
.word  HAL_INTR_ENTRY  @DMA1_Channel7_IRQHandler
.word  HAL_INTR_ENTRY  @ADC1_2_IRQHandler
.word  HAL_INTR_ENTRY  @USB_HP_CAN1_TX_IRQHandler
.word  HAL_INTR_ENTRY  @USB_LP_CAN1_RX0_IRQHandler
.word  HAL_INTR_ENTRY  @CAN1_RX1_IRQHandler
.word  HAL_INTR_ENTRY  @CAN1_SCE_IRQHandler
.word  HAL_INTR_ENTRY  @EXTI9_5_IRQHandler
.word  HAL_INTR_ENTRY  @TIM1_BRK_IRQHandler
.word  HAL_INTR_ENTRY  @TIM1_UP_IRQHandler
.word  HAL_INTR_ENTRY  @TIM1_TRG_COM_IRQHandler
.word  HAL_INTR_ENTRY  @TIM1_CC_IRQHandler
.word  HAL_INTR_ENTRY  @TIM2_IRQHandler
.word  HAL_INTR_ENTRY  @TIM3_IRQHandler
.word  HAL_INTR_ENTRY  @TIM4_IRQHandler
.word  HAL_INTR_ENTRY  @I2C1_EV_IRQHandler
.word  HAL_INTR_ENTRY  @I2C1_ER_IRQHandler
.word  HAL_INTR_ENTRY  @I2C2_EV_IRQHandler
.word  HAL_INTR_ENTRY  @I2C2_ER_IRQHandler
.word  HAL_INTR_ENTRY  @SPI1_IRQHandler
.word  HAL_INTR_ENTRY  @SPI2_IRQHandler
.word  HAL_INTR_ENTRY  @USART1_IRQHandler
.word  HAL_INTR_ENTRY  @USART2_IRQHandler
.word  HAL_INTR_ENTRY  @USART3_IRQHandler
.word  HAL_INTR_ENTRY  @EXTI15_10_IRQHandler
.word  HAL_INTR_ENTRY  @RTCAlarm_IRQHandler
.word  HAL_INTR_ENTRY  @USBWakeUp_IRQHandler  
.word  0
.word  0
.word  0
.word  0
.word  0
.word  0
.word  0
@.word  BootRAM   


.text        
HAL_INTR_ENTRY:
    push    {r0,lr}			
    mrs     r0,psp
    stmfd   r0!,{r4-r11}       
    msr     psp,r0

    @进入c中断处理函数是需要关中断，这里其实有点点bug，就是第一条指令到当前指令存在开中断真空期,
    @但是当前系统必须原子操作的nesting值没有用到，所以不会有影响,但是对于以后系统扩展可能会出现点点问题
    mov  r0,#1
    msr  primask,r0
    mrs     r0, ipsr          @取异常编号
    mov     lr,pc             @这两条指令模拟函数BL，调用真正的中断处理函数,
    add lr,lr,#7
    ldr pc,=acoral_intr_entry

    @stm32没有自动恢复中断状态的指令，只有手工恢复
    mov  r0,#0
    msr  primask,r0
    pop     {r0,lr}
    mrs     r0,psp
    ldmfd   r0!,{r4-r11}
    msr     psp,r0
    bx      lr

EXP_HANDLER:
    mrs r1,psp
    ldr r0,[r1,#24]
    ldr r2,=acoral_fault_entry
    bx  r2 
    bx  lr

HAL_INTR_ENABLE:
    CPSIE   I
    bx      lr

HAL_INTR_DISABLE:
    CPSID   I
    bx      lr

HAL_INTR_RESTORE:
    msr  PRIMASK,r0   @恢复状态
    bx      lr

HAL_INTR_DISABLE_SAVE:
	mrs     r0, PRIMASK	@保存中断状态	
        CPSID   I               @关中断
        bx      lr                  
