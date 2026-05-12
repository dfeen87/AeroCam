/*
 * MIT License
 *
 * Copyright (c) 2026 AeroCam contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*
 * Cortex-M startup code for AeroCam.
 *
 * This file performs only static startup initialization:
 *   - vector table placement
 *   - initialized data copy from FLASH to RAM
 *   - BSS zeroing
 *   - branch to main
 *
 * No heap region or dynamic memory support is defined here.
 */

.syntax unified
.cpu cortex-m4
.thumb

.global g_pfnVectors
.global Reset_Handler
.global Default_Handler

.extern _estack
.extern _etext
.extern _sdata
.extern _edata
.extern _sbss
.extern _ebss
.extern main

.section .isr_vector,"a",%progbits
.type g_pfnVectors, %object
g_pfnVectors:
    .word _estack
    .word Reset_Handler
    .word NMI_Handler
    .word HardFault_Handler
    .word MemManage_Handler
    .word BusFault_Handler
    .word UsageFault_Handler
    .word 0
    .word 0
    .word 0
    .word 0
    .word SVC_Handler
    .word DebugMon_Handler
    .word 0
    .word PendSV_Handler
    .word SysTick_Handler
    .word WWDG_IRQHandler
    .word PVD_IRQHandler
    .word TAMP_STAMP_IRQHandler
    .word RTC_WKUP_IRQHandler
    .word FLASH_IRQHandler
    .word RCC_IRQHandler
    .word EXTI0_IRQHandler
    .word EXTI1_IRQHandler
    .word EXTI2_IRQHandler
    .word EXTI3_IRQHandler
    .word EXTI4_IRQHandler
    .word DMA1_Stream0_IRQHandler
    .word DMA1_Stream1_IRQHandler
    .word DMA1_Stream2_IRQHandler
    .word DMA1_Stream3_IRQHandler
    .word DMA1_Stream4_IRQHandler
    .word DMA1_Stream5_IRQHandler
    .word DMA1_Stream6_IRQHandler
    .word ADC_IRQHandler
    .word CAN1_TX_IRQHandler
    .word CAN1_RX0_IRQHandler
    .word CAN1_RX1_IRQHandler
    .word CAN1_SCE_IRQHandler
    .word EXTI9_5_IRQHandler
    .word TIM1_BRK_TIM9_IRQHandler
    .word TIM1_UP_TIM10_IRQHandler
    .word TIM1_TRG_COM_TIM11_IRQHandler
    .word TIM1_CC_IRQHandler
    .word TIM2_IRQHandler
    .word TIM3_IRQHandler
    .word TIM4_IRQHandler
    .word I2C1_EV_IRQHandler
    .word I2C1_ER_IRQHandler
    .word I2C2_EV_IRQHandler
    .word I2C2_ER_IRQHandler
    .word SPI1_IRQHandler
    .word SPI2_IRQHandler
    .word USART1_IRQHandler
    .word USART2_IRQHandler
    .word USART3_IRQHandler
    .word EXTI15_10_IRQHandler
    .word RTC_Alarm_IRQHandler
    .word OTG_FS_WKUP_IRQHandler
    .word TIM8_BRK_TIM12_IRQHandler
    .word TIM8_UP_TIM13_IRQHandler
    .word TIM8_TRG_COM_TIM14_IRQHandler
    .word TIM8_CC_IRQHandler
    .word DMA1_Stream7_IRQHandler
    .word FMC_IRQHandler
    .word SDMMC1_IRQHandler
    .word TIM5_IRQHandler
    .word SPI3_IRQHandler
    .word UART4_IRQHandler
    .word UART5_IRQHandler
    .word TIM6_DAC_IRQHandler
    .word TIM7_IRQHandler
    .word DMA2_Stream0_IRQHandler
    .word DMA2_Stream1_IRQHandler
    .word DMA2_Stream2_IRQHandler
    .word DMA2_Stream3_IRQHandler
    .word DMA2_Stream4_IRQHandler
    .word ETH_IRQHandler
    .word ETH_WKUP_IRQHandler
    .word CAN2_TX_IRQHandler
    .word CAN2_RX0_IRQHandler
    .word CAN2_RX1_IRQHandler
    .word CAN2_SCE_IRQHandler
    .word OTG_FS_IRQHandler
    .word DMA2_Stream5_IRQHandler
    .word DMA2_Stream6_IRQHandler
    .word DMA2_Stream7_IRQHandler
    .word USART6_IRQHandler
    .word I2C3_EV_IRQHandler
    .word I2C3_ER_IRQHandler
    .word OTG_HS_EP1_OUT_IRQHandler
    .word OTG_HS_EP1_IN_IRQHandler
    .word OTG_HS_WKUP_IRQHandler
    .word OTG_HS_IRQHandler
    .word DCMI_IRQHandler
    .word CRYP_IRQHandler
    .word HASH_RNG_IRQHandler
    .word FPU_IRQHandler
    .word IRQ82_Handler
    .word IRQ83_Handler
    .word IRQ84_Handler
    .word IRQ85_Handler
    .word IRQ86_Handler
    .word IRQ87_Handler
    .word IRQ88_Handler
    .word IRQ89_Handler
    .word IRQ90_Handler
    .word IRQ91_Handler
    .word IRQ92_Handler
    .word IRQ93_Handler
    .word IRQ94_Handler
    .word IRQ95_Handler
    .word IRQ96_Handler
    .word IRQ97_Handler
    .word IRQ98_Handler
    .word IRQ99_Handler
    .word IRQ100_Handler
    .word IRQ101_Handler
    .word IRQ102_Handler
    .word IRQ103_Handler
    .word IRQ104_Handler
    .word IRQ105_Handler
    .word IRQ106_Handler
    .word IRQ107_Handler
    .word IRQ108_Handler
    .word IRQ109_Handler
    .word IRQ110_Handler
    .word IRQ111_Handler
    .word IRQ112_Handler
    .word IRQ113_Handler
    .word IRQ114_Handler
    .word IRQ115_Handler
    .word IRQ116_Handler
    .word IRQ117_Handler
    .word IRQ118_Handler
    .word IRQ119_Handler
    .word IRQ120_Handler
    .word IRQ121_Handler
    .word IRQ122_Handler
    .word IRQ123_Handler
    .word IRQ124_Handler
    .word IRQ125_Handler
    .word IRQ126_Handler
    .word IRQ127_Handler
    .word IRQ128_Handler
    .word IRQ129_Handler
    .word IRQ130_Handler
    .word IRQ131_Handler
    .word IRQ132_Handler
    .word IRQ133_Handler
    .word IRQ134_Handler
    .word IRQ135_Handler
    .word IRQ136_Handler
    .word IRQ137_Handler
    .word IRQ138_Handler
    .word IRQ139_Handler
    .word IRQ140_Handler
    .word IRQ141_Handler
    .word IRQ142_Handler
    .word IRQ143_Handler
    .word IRQ144_Handler
    .word IRQ145_Handler
    .word IRQ146_Handler
    .word IRQ147_Handler
    .word IRQ148_Handler
    .word IRQ149_Handler
    .word IRQ150_Handler
    .word IRQ151_Handler
    .word IRQ152_Handler
    .word IRQ153_Handler
    .word IRQ154_Handler
    .word IRQ155_Handler
    .word IRQ156_Handler
    .word IRQ157_Handler
    .word IRQ158_Handler
    .word IRQ159_Handler
    .word IRQ160_Handler
    .word IRQ161_Handler
    .word IRQ162_Handler
    .word IRQ163_Handler
    .word IRQ164_Handler
    .word IRQ165_Handler
.size g_pfnVectors, . - g_pfnVectors

.section .text.Reset_Handler,"ax",%progbits
.type Reset_Handler, %function
Reset_Handler:
    ldr r0, =_sdata
    ldr r1, =_edata
    ldr r2, =_etext

.Lcopy_data:
    cmp r0, r1
    bcc .Lcopy_data_word
    b .Lzero_bss_start

.Lcopy_data_word:
    ldr r3, [r2], #4
    str r3, [r0], #4
    b .Lcopy_data

.Lzero_bss_start:
    ldr r0, =_sbss
    ldr r1, =_ebss
    movs r2, #0

.Lzero_bss:
    cmp r0, r1
    bcc .Lzero_bss_word
    b .Lcall_main

.Lzero_bss_word:
    str r2, [r0], #4
    b .Lzero_bss

.Lcall_main:
    bl main

.Lmain_returned:
    b .Lmain_returned
.size Reset_Handler, . - Reset_Handler

.section .text.Default_Handler,"ax",%progbits
.type Default_Handler, %function
Default_Handler:
.Ldefault_loop:
    b .Ldefault_loop
.size Default_Handler, . - Default_Handler

.weak NMI_Handler
.thumb_set NMI_Handler, Default_Handler

.weak HardFault_Handler
.thumb_set HardFault_Handler, Default_Handler

.weak SysTick_Handler
.thumb_set SysTick_Handler, Default_Handler

.weak MemManage_Handler
.thumb_set MemManage_Handler, Default_Handler

.weak BusFault_Handler
.thumb_set BusFault_Handler, Default_Handler

.weak UsageFault_Handler
.thumb_set UsageFault_Handler, Default_Handler

.weak SVC_Handler
.thumb_set SVC_Handler, Default_Handler

.weak DebugMon_Handler
.thumb_set DebugMon_Handler, Default_Handler

.weak PendSV_Handler
.thumb_set PendSV_Handler, Default_Handler

.weak WWDG_IRQHandler
.thumb_set WWDG_IRQHandler, Default_Handler
.weak PVD_IRQHandler
.thumb_set PVD_IRQHandler, Default_Handler
.weak TAMP_STAMP_IRQHandler
.thumb_set TAMP_STAMP_IRQHandler, Default_Handler
.weak RTC_WKUP_IRQHandler
.thumb_set RTC_WKUP_IRQHandler, Default_Handler
.weak FLASH_IRQHandler
.thumb_set FLASH_IRQHandler, Default_Handler
.weak RCC_IRQHandler
.thumb_set RCC_IRQHandler, Default_Handler
.weak EXTI0_IRQHandler
.thumb_set EXTI0_IRQHandler, Default_Handler
.weak EXTI1_IRQHandler
.thumb_set EXTI1_IRQHandler, Default_Handler
.weak EXTI2_IRQHandler
.thumb_set EXTI2_IRQHandler, Default_Handler
.weak EXTI3_IRQHandler
.thumb_set EXTI3_IRQHandler, Default_Handler
.weak EXTI4_IRQHandler
.thumb_set EXTI4_IRQHandler, Default_Handler
.weak DMA1_Stream0_IRQHandler
.thumb_set DMA1_Stream0_IRQHandler, Default_Handler
.weak DMA1_Stream1_IRQHandler
.thumb_set DMA1_Stream1_IRQHandler, Default_Handler
.weak DMA1_Stream2_IRQHandler
.thumb_set DMA1_Stream2_IRQHandler, Default_Handler
.weak DMA1_Stream3_IRQHandler
.thumb_set DMA1_Stream3_IRQHandler, Default_Handler
.weak DMA1_Stream4_IRQHandler
.thumb_set DMA1_Stream4_IRQHandler, Default_Handler
.weak DMA1_Stream5_IRQHandler
.thumb_set DMA1_Stream5_IRQHandler, Default_Handler
.weak DMA1_Stream6_IRQHandler
.thumb_set DMA1_Stream6_IRQHandler, Default_Handler
.weak ADC_IRQHandler
.thumb_set ADC_IRQHandler, Default_Handler
.weak CAN1_TX_IRQHandler
.thumb_set CAN1_TX_IRQHandler, Default_Handler
.weak CAN1_RX0_IRQHandler
.thumb_set CAN1_RX0_IRQHandler, Default_Handler
.weak CAN1_RX1_IRQHandler
.thumb_set CAN1_RX1_IRQHandler, Default_Handler
.weak CAN1_SCE_IRQHandler
.thumb_set CAN1_SCE_IRQHandler, Default_Handler
.weak EXTI9_5_IRQHandler
.thumb_set EXTI9_5_IRQHandler, Default_Handler
.weak TIM1_BRK_TIM9_IRQHandler
.thumb_set TIM1_BRK_TIM9_IRQHandler, Default_Handler
.weak TIM1_UP_TIM10_IRQHandler
.thumb_set TIM1_UP_TIM10_IRQHandler, Default_Handler
.weak TIM1_TRG_COM_TIM11_IRQHandler
.thumb_set TIM1_TRG_COM_TIM11_IRQHandler, Default_Handler
.weak TIM1_CC_IRQHandler
.thumb_set TIM1_CC_IRQHandler, Default_Handler
.weak TIM2_IRQHandler
.thumb_set TIM2_IRQHandler, Default_Handler
.weak TIM3_IRQHandler
.thumb_set TIM3_IRQHandler, Default_Handler
.weak TIM4_IRQHandler
.thumb_set TIM4_IRQHandler, Default_Handler
.weak I2C1_EV_IRQHandler
.thumb_set I2C1_EV_IRQHandler, Default_Handler
.weak I2C1_ER_IRQHandler
.thumb_set I2C1_ER_IRQHandler, Default_Handler
.weak I2C2_EV_IRQHandler
.thumb_set I2C2_EV_IRQHandler, Default_Handler
.weak I2C2_ER_IRQHandler
.thumb_set I2C2_ER_IRQHandler, Default_Handler
.weak SPI1_IRQHandler
.thumb_set SPI1_IRQHandler, Default_Handler
.weak SPI2_IRQHandler
.thumb_set SPI2_IRQHandler, Default_Handler
.weak USART1_IRQHandler
.thumb_set USART1_IRQHandler, Default_Handler
.weak USART2_IRQHandler
.thumb_set USART2_IRQHandler, Default_Handler
.weak USART3_IRQHandler
.thumb_set USART3_IRQHandler, Default_Handler
.weak EXTI15_10_IRQHandler
.thumb_set EXTI15_10_IRQHandler, Default_Handler
.weak RTC_Alarm_IRQHandler
.thumb_set RTC_Alarm_IRQHandler, Default_Handler
.weak OTG_FS_WKUP_IRQHandler
.thumb_set OTG_FS_WKUP_IRQHandler, Default_Handler
.weak TIM8_BRK_TIM12_IRQHandler
.thumb_set TIM8_BRK_TIM12_IRQHandler, Default_Handler
.weak TIM8_UP_TIM13_IRQHandler
.thumb_set TIM8_UP_TIM13_IRQHandler, Default_Handler
.weak TIM8_TRG_COM_TIM14_IRQHandler
.thumb_set TIM8_TRG_COM_TIM14_IRQHandler, Default_Handler
.weak TIM8_CC_IRQHandler
.thumb_set TIM8_CC_IRQHandler, Default_Handler
.weak DMA1_Stream7_IRQHandler
.thumb_set DMA1_Stream7_IRQHandler, Default_Handler
.weak FMC_IRQHandler
.thumb_set FMC_IRQHandler, Default_Handler
.weak SDMMC1_IRQHandler
.thumb_set SDMMC1_IRQHandler, Default_Handler
.weak TIM5_IRQHandler
.thumb_set TIM5_IRQHandler, Default_Handler
.weak SPI3_IRQHandler
.thumb_set SPI3_IRQHandler, Default_Handler
.weak UART4_IRQHandler
.thumb_set UART4_IRQHandler, Default_Handler
.weak UART5_IRQHandler
.thumb_set UART5_IRQHandler, Default_Handler
.weak TIM6_DAC_IRQHandler
.thumb_set TIM6_DAC_IRQHandler, Default_Handler
.weak TIM7_IRQHandler
.thumb_set TIM7_IRQHandler, Default_Handler
.weak DMA2_Stream0_IRQHandler
.thumb_set DMA2_Stream0_IRQHandler, Default_Handler
.weak DMA2_Stream1_IRQHandler
.thumb_set DMA2_Stream1_IRQHandler, Default_Handler
.weak DMA2_Stream2_IRQHandler
.thumb_set DMA2_Stream2_IRQHandler, Default_Handler
.weak DMA2_Stream3_IRQHandler
.thumb_set DMA2_Stream3_IRQHandler, Default_Handler
.weak DMA2_Stream4_IRQHandler
.thumb_set DMA2_Stream4_IRQHandler, Default_Handler
.weak ETH_IRQHandler
.thumb_set ETH_IRQHandler, Default_Handler
.weak ETH_WKUP_IRQHandler
.thumb_set ETH_WKUP_IRQHandler, Default_Handler
.weak CAN2_TX_IRQHandler
.thumb_set CAN2_TX_IRQHandler, Default_Handler
.weak CAN2_RX0_IRQHandler
.thumb_set CAN2_RX0_IRQHandler, Default_Handler
.weak CAN2_RX1_IRQHandler
.thumb_set CAN2_RX1_IRQHandler, Default_Handler
.weak CAN2_SCE_IRQHandler
.thumb_set CAN2_SCE_IRQHandler, Default_Handler
.weak OTG_FS_IRQHandler
.thumb_set OTG_FS_IRQHandler, Default_Handler
.weak DMA2_Stream5_IRQHandler
.thumb_set DMA2_Stream5_IRQHandler, Default_Handler
.weak DMA2_Stream6_IRQHandler
.thumb_set DMA2_Stream6_IRQHandler, Default_Handler
.weak DMA2_Stream7_IRQHandler
.thumb_set DMA2_Stream7_IRQHandler, Default_Handler
.weak USART6_IRQHandler
.thumb_set USART6_IRQHandler, Default_Handler
.weak I2C3_EV_IRQHandler
.thumb_set I2C3_EV_IRQHandler, Default_Handler
.weak I2C3_ER_IRQHandler
.thumb_set I2C3_ER_IRQHandler, Default_Handler
.weak OTG_HS_EP1_OUT_IRQHandler
.thumb_set OTG_HS_EP1_OUT_IRQHandler, Default_Handler
.weak OTG_HS_EP1_IN_IRQHandler
.thumb_set OTG_HS_EP1_IN_IRQHandler, Default_Handler
.weak OTG_HS_WKUP_IRQHandler
.thumb_set OTG_HS_WKUP_IRQHandler, Default_Handler
.weak OTG_HS_IRQHandler
.thumb_set OTG_HS_IRQHandler, Default_Handler
.weak DCMI_IRQHandler
.thumb_set DCMI_IRQHandler, Default_Handler
.weak CRYP_IRQHandler
.thumb_set CRYP_IRQHandler, Default_Handler
.weak HASH_RNG_IRQHandler
.thumb_set HASH_RNG_IRQHandler, Default_Handler
.weak FPU_IRQHandler
.thumb_set FPU_IRQHandler, Default_Handler
.weak IRQ82_Handler
.thumb_set IRQ82_Handler, Default_Handler
.weak IRQ83_Handler
.thumb_set IRQ83_Handler, Default_Handler
.weak IRQ84_Handler
.thumb_set IRQ84_Handler, Default_Handler
.weak IRQ85_Handler
.thumb_set IRQ85_Handler, Default_Handler
.weak IRQ86_Handler
.thumb_set IRQ86_Handler, Default_Handler
.weak IRQ87_Handler
.thumb_set IRQ87_Handler, Default_Handler
.weak IRQ88_Handler
.thumb_set IRQ88_Handler, Default_Handler
.weak IRQ89_Handler
.thumb_set IRQ89_Handler, Default_Handler
.weak IRQ90_Handler
.thumb_set IRQ90_Handler, Default_Handler
.weak IRQ91_Handler
.thumb_set IRQ91_Handler, Default_Handler
.weak IRQ92_Handler
.thumb_set IRQ92_Handler, Default_Handler
.weak IRQ93_Handler
.thumb_set IRQ93_Handler, Default_Handler
.weak IRQ94_Handler
.thumb_set IRQ94_Handler, Default_Handler
.weak IRQ95_Handler
.thumb_set IRQ95_Handler, Default_Handler
.weak IRQ96_Handler
.thumb_set IRQ96_Handler, Default_Handler
.weak IRQ97_Handler
.thumb_set IRQ97_Handler, Default_Handler
.weak IRQ98_Handler
.thumb_set IRQ98_Handler, Default_Handler
.weak IRQ99_Handler
.thumb_set IRQ99_Handler, Default_Handler
.weak IRQ100_Handler
.thumb_set IRQ100_Handler, Default_Handler
.weak IRQ101_Handler
.thumb_set IRQ101_Handler, Default_Handler
.weak IRQ102_Handler
.thumb_set IRQ102_Handler, Default_Handler
.weak IRQ103_Handler
.thumb_set IRQ103_Handler, Default_Handler
.weak IRQ104_Handler
.thumb_set IRQ104_Handler, Default_Handler
.weak IRQ105_Handler
.thumb_set IRQ105_Handler, Default_Handler
.weak IRQ106_Handler
.thumb_set IRQ106_Handler, Default_Handler
.weak IRQ107_Handler
.thumb_set IRQ107_Handler, Default_Handler
.weak IRQ108_Handler
.thumb_set IRQ108_Handler, Default_Handler
.weak IRQ109_Handler
.thumb_set IRQ109_Handler, Default_Handler
.weak IRQ110_Handler
.thumb_set IRQ110_Handler, Default_Handler
.weak IRQ111_Handler
.thumb_set IRQ111_Handler, Default_Handler
.weak IRQ112_Handler
.thumb_set IRQ112_Handler, Default_Handler
.weak IRQ113_Handler
.thumb_set IRQ113_Handler, Default_Handler
.weak IRQ114_Handler
.thumb_set IRQ114_Handler, Default_Handler
.weak IRQ115_Handler
.thumb_set IRQ115_Handler, Default_Handler
.weak IRQ116_Handler
.thumb_set IRQ116_Handler, Default_Handler
.weak IRQ117_Handler
.thumb_set IRQ117_Handler, Default_Handler
.weak IRQ118_Handler
.thumb_set IRQ118_Handler, Default_Handler
.weak IRQ119_Handler
.thumb_set IRQ119_Handler, Default_Handler
.weak IRQ120_Handler
.thumb_set IRQ120_Handler, Default_Handler
.weak IRQ121_Handler
.thumb_set IRQ121_Handler, Default_Handler
.weak IRQ122_Handler
.thumb_set IRQ122_Handler, Default_Handler
.weak IRQ123_Handler
.thumb_set IRQ123_Handler, Default_Handler
.weak IRQ124_Handler
.thumb_set IRQ124_Handler, Default_Handler
.weak IRQ125_Handler
.thumb_set IRQ125_Handler, Default_Handler
.weak IRQ126_Handler
.thumb_set IRQ126_Handler, Default_Handler
.weak IRQ127_Handler
.thumb_set IRQ127_Handler, Default_Handler
.weak IRQ128_Handler
.thumb_set IRQ128_Handler, Default_Handler
.weak IRQ129_Handler
.thumb_set IRQ129_Handler, Default_Handler
.weak IRQ130_Handler
.thumb_set IRQ130_Handler, Default_Handler
.weak IRQ131_Handler
.thumb_set IRQ131_Handler, Default_Handler
.weak IRQ132_Handler
.thumb_set IRQ132_Handler, Default_Handler
.weak IRQ133_Handler
.thumb_set IRQ133_Handler, Default_Handler
.weak IRQ134_Handler
.thumb_set IRQ134_Handler, Default_Handler
.weak IRQ135_Handler
.thumb_set IRQ135_Handler, Default_Handler
.weak IRQ136_Handler
.thumb_set IRQ136_Handler, Default_Handler
.weak IRQ137_Handler
.thumb_set IRQ137_Handler, Default_Handler
.weak IRQ138_Handler
.thumb_set IRQ138_Handler, Default_Handler
.weak IRQ139_Handler
.thumb_set IRQ139_Handler, Default_Handler
.weak IRQ140_Handler
.thumb_set IRQ140_Handler, Default_Handler
.weak IRQ141_Handler
.thumb_set IRQ141_Handler, Default_Handler
.weak IRQ142_Handler
.thumb_set IRQ142_Handler, Default_Handler
.weak IRQ143_Handler
.thumb_set IRQ143_Handler, Default_Handler
.weak IRQ144_Handler
.thumb_set IRQ144_Handler, Default_Handler
.weak IRQ145_Handler
.thumb_set IRQ145_Handler, Default_Handler
.weak IRQ146_Handler
.thumb_set IRQ146_Handler, Default_Handler
.weak IRQ147_Handler
.thumb_set IRQ147_Handler, Default_Handler
.weak IRQ148_Handler
.thumb_set IRQ148_Handler, Default_Handler
.weak IRQ149_Handler
.thumb_set IRQ149_Handler, Default_Handler
.weak IRQ150_Handler
.thumb_set IRQ150_Handler, Default_Handler
.weak IRQ151_Handler
.thumb_set IRQ151_Handler, Default_Handler
.weak IRQ152_Handler
.thumb_set IRQ152_Handler, Default_Handler
.weak IRQ153_Handler
.thumb_set IRQ153_Handler, Default_Handler
.weak IRQ154_Handler
.thumb_set IRQ154_Handler, Default_Handler
.weak IRQ155_Handler
.thumb_set IRQ155_Handler, Default_Handler
.weak IRQ156_Handler
.thumb_set IRQ156_Handler, Default_Handler
.weak IRQ157_Handler
.thumb_set IRQ157_Handler, Default_Handler
.weak IRQ158_Handler
.thumb_set IRQ158_Handler, Default_Handler
.weak IRQ159_Handler
.thumb_set IRQ159_Handler, Default_Handler
.weak IRQ160_Handler
.thumb_set IRQ160_Handler, Default_Handler
.weak IRQ161_Handler
.thumb_set IRQ161_Handler, Default_Handler
.weak IRQ162_Handler
.thumb_set IRQ162_Handler, Default_Handler
.weak IRQ163_Handler
.thumb_set IRQ163_Handler, Default_Handler
.weak IRQ164_Handler
.thumb_set IRQ164_Handler, Default_Handler
.weak IRQ165_Handler
.thumb_set IRQ165_Handler, Default_Handler
