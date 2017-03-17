/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * board-init.c
 *
 * Created on: Mar 8, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

#include <cmsis.h>

#include <stm32f4-hal/stm32f4xx_hal.h>

#include <diag/diag.h>
#include <init/init-sections.h>

#include <board/board.h>

void assert_failed(uint8_t* file, uint32_t line);
void SysTick_Handler(void);
void HAL_UART_MspInit(UART_HandleTypeDef *huart);
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart);

static UART_HandleTypeDef uart_handle;

void assert_failed(uint8_t* file, uint32_t line)
{
	diag_printf("assert failed: %s @ %lu\n", file, line);
	abort();
}

void __attribute__((section(".after_vectors"), weak)) SysTick_Handler(void)
{
	HAL_IncTick();
}

static void stm32f429_init(void)
{
	SystemInit();
	SystemCoreClockUpdate();
}
PREINIT_SYSINIT_WITH_PRIORITY(stm32f429_init, 0);

static void board_init(void)
{
	HAL_Init();

	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	/* Enable Power Control clock */
	__HAL_RCC_PWR_CLK_ENABLE();

	/* The voltage scaling allows optimizing the power consumption when the device is
	 clocked below the maximum system frequency, to update the voltage scaling value
	 regarding system frequency refer to product datasheet.  */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/* Enable HSE Oscillator and activate PLL with HSE as source */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 360;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		abort();

	if(HAL_PWREx_EnableOverDrive() != HAL_OK)
		abort();

	/* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
	 clocks dividers */
	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
		abort();

	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);


}
PREINIT_PLATFORM_WITH_PRIORITY(board_init, 0);

void board_delay(unsigned long msecs)
{
	HAL_Delay(msecs);
}

int board_get_num_leds(void)
{
	return 2;
}

int board_led_on(int led)
{
	if (led < 0 || led > 1) {
		errno = EINVAL;
		return -1;
	}

	HAL_GPIO_WritePin(GPIOG, (led == 0 ? GPIO_PIN_13 : GPIO_PIN_14), GPIO_PIN_SET);
	return 0;
}

int board_led_off(int led)
{
	if (led < 0 || led > 1) {
		errno = EINVAL;
		return -1;
	}

	HAL_GPIO_WritePin(GPIOG, (led == 0 ? GPIO_PIN_13 : GPIO_PIN_14), GPIO_PIN_RESET);
	return 0;
}

int board_led_toggle(int led)
{
	if (led < 0 || led > 1) {
		errno = EINVAL;
		return -1;
	}

	HAL_GPIO_TogglePin(GPIOG, (led == 0 ? GPIO_PIN_13 : GPIO_PIN_14));
	return 0;
}

static void board_led_init(void)
{
	/* Enable the GPIO_LED Clock */
	__GPIOG_CLK_ENABLE();

	/* Configure the GPIO_LED pin */
	GPIO_InitTypeDef  gpio_init;
	gpio_init.Pin = GPIO_PIN_13;
	gpio_init.Mode = GPIO_MODE_OUTPUT_PP;
	gpio_init.Pull = GPIO_PULLUP;
	gpio_init.Speed = GPIO_SPEED_FAST;

	HAL_GPIO_Init(GPIOG, &gpio_init);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);

	gpio_init.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOG, &gpio_init);
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
}
PREINIT_PLATFORM(board_led_init);

int board_getchar(void)
{
	uint8_t data;

	if (HAL_UART_Receive(&uart_handle, &data, 1, HAL_MAX_DELAY) != HAL_OK) {
		errno = EIO;
		return -1;
	}

	return data & 0xff;
}

int board_putchar(int c)
{
	uint8_t data = c & 0xff;
	static uint8_t cr = '\r';

	if (c == '\n' && HAL_UART_Transmit(&uart_handle, &cr, 1, HAL_MAX_DELAY) != HAL_OK)
		goto error;

	if (HAL_UART_Transmit(&uart_handle, &data, 1, HAL_MAX_DELAY) != HAL_OK)
		goto error;

	return 0;

error:
	errno = EIO;
	return -1;
}

int board_puts(const char *s)
{
	uint8_t *data = (uint8_t *)s;
	uint8_t newline[] = { '\r', '\n' };

	if (HAL_UART_Transmit(&uart_handle, data, strlen(s), HAL_MAX_DELAY) != HAL_OK) {
		errno = EIO;
		return -1;
	}

	if (HAL_UART_Transmit(&uart_handle, newline, 2, HAL_MAX_DELAY) != HAL_OK) {
		errno = EIO;
		return -1;
	}

	return strlen(s) + 2;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	/* Enable the GPIO Port Clock and USART1 Clock */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_USART1_CLK_ENABLE();

	/* Setup USART1 TX GPIO pin configuration  */
	GPIO_InitTypeDef  GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* USART1 RX GPIO pin configuration  */
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
	/* Reset peripherals */
	__HAL_RCC_USART1_FORCE_RESET();
	__HAL_RCC_USART2_RELEASE_RESET();

	/* Disable peripherals and GPIO Clocks */
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9);
	HAL_GPIO_DeInit(GPIOA, GPIO_PIN_10);
}

static void board_uart_init(void)
{
	/* Initialize uart */
	uart_handle.Instance = USART1;
	uart_handle.Init.BaudRate     = 115200;
	uart_handle.Init.WordLength   = UART_WORDLENGTH_8B;
	uart_handle.Init.StopBits     = UART_STOPBITS_1;
	uart_handle.Init.Parity       = UART_PARITY_NONE;
	uart_handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
	uart_handle.Init.Mode         = UART_MODE_TX_RX;
	uart_handle.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&uart_handle) != HAL_OK)
		abort();
}
PREINIT_PLATFORM(board_uart_init);
