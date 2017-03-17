/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * stm32429xx-vectors.S
 *
 * Created on: Mar 8, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */


#include <asm.h>

	.section .vectors
	.align	2
	.global	_vectors
_vectors:
	.long	__stack                             /* Top of Stack */
	.long	Reset_Handler                       /* Reset Handler */
	.long	NMI_Handler                         /* NMI Handler */
	.long	HardFault_Handler                   /* Hard Fault Handler */
	.long	MemManage_Handler                   /* MPU Fault Handler */
	.long	BusFault_Handler                    /* Bus Fault Handler */
	.long	UsageFault_Handler                  /* Usage Fault Handler */
	.long	0                                   /* Reserved */
	.long	0                                   /* Reserved */
	.long	0                                   /* Reserved */
	.long	0                                   /* Reserved */
	.long	SVC_Handler                         /* SVCall Handler */
	.long	DebugMon_Handler                    /* Debug Monitor Handler */
	.long	0                                   /* Reserved */
	.long	PendSV_Handler                      /* PendSV Handler */
	.long	SysTick_Handler                     /* SysTick Handler */

	.long	WWDG_IRQHandler                   /* Window WatchDog              */
	.long	PVD_IRQHandler                    /* PVD through EXTI Line detection */
	.long	TAMP_STAMP_IRQHandler             /* Tamper and TimeStamps through the EXTI line */
	.long	RTC_WKUP_IRQHandler               /* RTC Wakeup through the EXTI line */
	.long	FLASH_IRQHandler                  /* FLASH                        */
	.long	RCC_IRQHandler                    /* RCC                          */
	.long	EXTI0_IRQHandler                  /* EXTI Line0                   */
	.long	EXTI1_IRQHandler                  /* EXTI Line1                   */
	.long	EXTI2_IRQHandler                  /* EXTI Line2                   */
	.long	EXTI3_IRQHandler                  /* EXTI Line3                   */
	.long	EXTI4_IRQHandler                  /* EXTI Line4                   */
	.long	DMA1_Stream0_IRQHandler           /* DMA1 Stream 0                */
	.long	DMA1_Stream1_IRQHandler           /* DMA1 Stream 1                */
	.long	DMA1_Stream2_IRQHandler           /* DMA1 Stream 2                */
	.long	DMA1_Stream3_IRQHandler           /* DMA1 Stream 3                */
	.long	DMA1_Stream4_IRQHandler           /* DMA1 Stream 4                */
	.long	DMA1_Stream5_IRQHandler           /* DMA1 Stream 5                */
	.long	DMA1_Stream6_IRQHandler           /* DMA1 Stream 6                */
	.long	ADC_IRQHandler                    /* ADC1, ADC2 and ADC3s         */
	.long	CAN1_TX_IRQHandler                /* CAN1 TX                      */
	.long	CAN1_RX0_IRQHandler               /* CAN1 RX0                     */
	.long	CAN1_RX1_IRQHandler               /* CAN1 RX1                     */
	.long	CAN1_SCE_IRQHandler               /* CAN1 SCE                     */
	.long	EXTI9_5_IRQHandler                /* External Line[9:5]s          */
	.long	TIM1_BRK_TIM9_IRQHandler          /* TIM1 Break and TIM9          */
	.long	TIM1_UP_TIM10_IRQHandler          /* TIM1 Update and TIM10        */
	.long	TIM1_TRG_COM_TIM11_IRQHandler     /* TIM1 Trigger and Commutation and TIM11 */
	.long	TIM1_CC_IRQHandler                /* TIM1 Capture Compare         */
	.long	TIM2_IRQHandler                   /* TIM2                         */
	.long	TIM3_IRQHandler                   /* TIM3                         */
	.long	TIM4_IRQHandler                   /* TIM4                         */
	.long	I2C1_EV_IRQHandler                /* I2C1 Event                   */
	.long	I2C1_ER_IRQHandler                /* I2C1 Error                   */
	.long	I2C2_EV_IRQHandler                /* I2C2 Event                   */
	.long	I2C2_ER_IRQHandler                /* I2C2 Error                   */
	.long	SPI1_IRQHandler                   /* SPI1                         */
	.long	SPI2_IRQHandler                   /* SPI2                         */
	.long	USART1_IRQHandler                 /* USART1                       */
	.long	USART2_IRQHandler                 /* USART2                       */
	.long	USART3_IRQHandler                 /* USART3                       */
	.long	EXTI15_10_IRQHandler              /* External Line[15:10]s        */
	.long	RTC_Alarm_IRQHandler              /* RTC Alarm (A and B) through EXTI Line */
	.long	OTG_FS_WKUP_IRQHandler            /* USB OTG FS Wakeup through EXTI line */
	.long	TIM8_BRK_TIM12_IRQHandler         /* TIM8 Break and TIM12         */
	.long	TIM8_UP_TIM13_IRQHandler          /* TIM8 Update and TIM13        */
	.long	TIM8_TRG_COM_TIM14_IRQHandler     /* TIM8 Trigger and Commutation and TIM14 */
	.long	TIM8_CC_IRQHandler                /* TIM8 Capture Compare         */
	.long	DMA1_Stream7_IRQHandler           /* DMA1 Stream7                 */
	.long	FMC_IRQHandler                    /* FMC                         */
	.long	SDIO_IRQHandler                   /* SDIO                         */
	.long	TIM5_IRQHandler                   /* TIM5                         */
	.long	SPI3_IRQHandler                   /* SPI3                         */
	.long	UART4_IRQHandler                  /* UART4                        */
	.long	UART5_IRQHandler                  /* UART5                        */
	.long	TIM6_DAC_IRQHandler               /* TIM6 and DAC1&2 underrun errors */
	.long	TIM7_IRQHandler                   /* TIM7                         */
	.long	DMA2_Stream0_IRQHandler           /* DMA2 Stream 0                */
	.long	DMA2_Stream1_IRQHandler           /* DMA2 Stream 1                */
	.long	DMA2_Stream2_IRQHandler           /* DMA2 Stream 2                */
	.long	DMA2_Stream3_IRQHandler           /* DMA2 Stream 3                */
	.long	DMA2_Stream4_IRQHandler           /* DMA2 Stream 4                */
	.long	ETH_IRQHandler                    /* Ethernet                     */
	.long	ETH_WKUP_IRQHandler               /* Ethernet Wakeup through EXTI line */
	.long	CAN2_TX_IRQHandler                /* CAN2 TX                      */
	.long	CAN2_RX0_IRQHandler               /* CAN2 RX0                     */
	.long	CAN2_RX1_IRQHandler               /* CAN2 RX1                     */
	.long	CAN2_SCE_IRQHandler               /* CAN2 SCE                     */
	.long	OTG_FS_IRQHandler                 /* USB OTG FS                   */
	.long	DMA2_Stream5_IRQHandler           /* DMA2 Stream 5                */
	.long	DMA2_Stream6_IRQHandler           /* DMA2 Stream 6                */
	.long	DMA2_Stream7_IRQHandler           /* DMA2 Stream 7                */
	.long	USART6_IRQHandler                 /* USART6                       */
	.long	I2C3_EV_IRQHandler                /* I2C3 event                   */
	.long	I2C3_ER_IRQHandler                /* I2C3 error                   */
	.long	OTG_HS_EP1_OUT_IRQHandler         /* USB OTG HS End Point 1 Out   */
	.long	OTG_HS_EP1_IN_IRQHandler          /* USB OTG HS End Point 1 In    */
	.long	OTG_HS_WKUP_IRQHandler            /* USB OTG HS Wakeup through EXTI */
	.long	OTG_HS_IRQHandler                 /* USB OTG HS                   */
	.long	DCMI_IRQHandler                   /* DCMI                         */
	.long	0                                 /* Reserved                     */
	.long	HASH_RNG_IRQHandler               /* Hash and Rng                 */
	.long	FPU_IRQHandler                    /* FPU                          */
	.long	UART7_IRQHandler                  /* UART7                        */
	.long	UART8_IRQHandler                  /* UART8                        */
	.long	SPI4_IRQHandler                   /* SPI4                         */
	.long	SPI5_IRQHandler                   /* SPI5 						  */
	.long	SPI6_IRQHandler                   /* SPI6						  */
	.long	SAI1_IRQHandler                   /* SAI1						  */
	.long	LTDC_IRQHandler                   /* LTDC_IRQHandler			  */
	.long	LTDC_ER_IRQHandler                /* LTDC_ER_IRQHandler			  */
	.long	DMA2D_IRQHandler                  /* DMA2D                        */
	.size	_vectors, . - _vectors

declare_weak_function Default_Handler
	.fnstart
	.cantunwind

	bx	lr

	.fnend
	.pool
	.size	Default_Handler, . - Default_Handler

.macro	def_irq_handler handler_name
	.weak		\handler_name
	.thumb_set	\handler_name, Default_Handler
.endm

	def_irq_handler NMI_Handler
	def_irq_handler HardFault_Handler
	def_irq_handler MemManage_Handler
	def_irq_handler BusFault_Handler
	def_irq_handler UsageFault_Handler
	def_irq_handler SVC_Handler
	def_irq_handler DebugMon_Handler
	def_irq_handler PendSV_Handler
	def_irq_handler SysTick_Handler
	def_irq_handler WWDG_IRQHandler
	def_irq_handler PVD_IRQHandler
	def_irq_handler TAMP_STAMP_IRQHandler
	def_irq_handler RTC_WKUP_IRQHandler
	def_irq_handler FLASH_IRQHandler
	def_irq_handler RCC_IRQHandler
	def_irq_handler EXTI0_IRQHandler
	def_irq_handler EXTI1_IRQHandler
	def_irq_handler EXTI2_IRQHandler
	def_irq_handler EXTI3_IRQHandler
	def_irq_handler EXTI4_IRQHandler
	def_irq_handler DMA1_Stream0_IRQHandler
	def_irq_handler DMA1_Stream1_IRQHandler
	def_irq_handler DMA1_Stream2_IRQHandler
	def_irq_handler DMA1_Stream3_IRQHandler
	def_irq_handler DMA1_Stream4_IRQHandler
	def_irq_handler DMA1_Stream5_IRQHandler
	def_irq_handler DMA1_Stream6_IRQHandler
	def_irq_handler ADC_IRQHandler
	def_irq_handler CAN1_TX_IRQHandler
	def_irq_handler CAN1_RX0_IRQHandler
	def_irq_handler CAN1_RX1_IRQHandler
	def_irq_handler CAN1_SCE_IRQHandler
	def_irq_handler EXTI9_5_IRQHandler
	def_irq_handler TIM1_BRK_TIM9_IRQHandler
	def_irq_handler TIM1_UP_TIM10_IRQHandler
	def_irq_handler TIM1_TRG_COM_TIM11_IRQHandler
	def_irq_handler TIM1_CC_IRQHandler
	def_irq_handler TIM2_IRQHandler
	def_irq_handler TIM3_IRQHandler
	def_irq_handler TIM4_IRQHandler
	def_irq_handler I2C1_EV_IRQHandler
	def_irq_handler I2C1_ER_IRQHandler
	def_irq_handler I2C2_EV_IRQHandler
	def_irq_handler I2C2_ER_IRQHandler
	def_irq_handler SPI1_IRQHandler
	def_irq_handler SPI2_IRQHandler
	def_irq_handler USART1_IRQHandler
	def_irq_handler USART2_IRQHandler
	def_irq_handler USART3_IRQHandler
	def_irq_handler EXTI15_10_IRQHandler
	def_irq_handler RTC_Alarm_IRQHandler
	def_irq_handler OTG_FS_WKUP_IRQHandler
	def_irq_handler TIM8_BRK_TIM12_IRQHandler
	def_irq_handler TIM8_UP_TIM13_IRQHandler
	def_irq_handler TIM8_TRG_COM_TIM14_IRQHandler
	def_irq_handler TIM8_CC_IRQHandler
	def_irq_handler DMA1_Stream7_IRQHandler
	def_irq_handler FMC_IRQHandler
	def_irq_handler SDIO_IRQHandler
	def_irq_handler TIM5_IRQHandler
	def_irq_handler SPI3_IRQHandler
	def_irq_handler UART4_IRQHandler
	def_irq_handler UART5_IRQHandler
	def_irq_handler TIM6_DAC_IRQHandler
	def_irq_handler TIM7_IRQHandler
	def_irq_handler DMA2_Stream0_IRQHandler
	def_irq_handler DMA2_Stream1_IRQHandler
	def_irq_handler DMA2_Stream2_IRQHandler
	def_irq_handler DMA2_Stream3_IRQHandler
	def_irq_handler DMA2_Stream4_IRQHandler
	def_irq_handler ETH_IRQHandler
	def_irq_handler ETH_WKUP_IRQHandler
	def_irq_handler CAN2_TX_IRQHandler
	def_irq_handler CAN2_RX0_IRQHandler
	def_irq_handler CAN2_RX1_IRQHandler
	def_irq_handler CAN2_SCE_IRQHandler
	def_irq_handler OTG_FS_IRQHandler
	def_irq_handler DMA2_Stream5_IRQHandler
	def_irq_handler DMA2_Stream6_IRQHandler
	def_irq_handler DMA2_Stream7_IRQHandler
	def_irq_handler USART6_IRQHandler
	def_irq_handler I2C3_EV_IRQHandler
	def_irq_handler I2C3_ER_IRQHandler
	def_irq_handler OTG_HS_EP1_OUT_IRQHandler
	def_irq_handler OTG_HS_EP1_IN_IRQHandler
	def_irq_handler OTG_HS_WKUP_IRQHandler
	def_irq_handler OTG_HS_IRQHandler
	def_irq_handler DCMI_IRQHandler
	def_irq_handler HASH_RNG_IRQHandler
	def_irq_handler FPU_IRQHandler
	def_irq_handler UART7_IRQHandler
	def_irq_handler UART8_IRQHandler
	def_irq_handler SPI4_IRQHandler
	def_irq_handler SPI5_IRQHandler
	def_irq_handler SPI6_IRQHandler
	def_irq_handler SAI1_IRQHandler
	def_irq_handler LTDC_IRQHandler
	def_irq_handler LTDC_ER_IRQHandler
	def_irq_handler DMA2D_IRQHandler
