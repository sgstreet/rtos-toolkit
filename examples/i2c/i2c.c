/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * i2c.c
 *
 * Created on: Mar 8, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <diag/diag.h>
#include <board/board.h>
#include <stm32f4-hal/stm32f4xx_hal.h>
#include <cmsis.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define I2C_TIMEOUT 20000

#define I2C_FREQRANGE(__PCLK__)                            ((__PCLK__)/1000000U)
#define I2C_RISE_TIME(__FREQRANGE__, __SPEED__)            (((__SPEED__) <= 100000U) ? ((__FREQRANGE__) + 1U) : ((((__FREQRANGE__) * 300U) / 1000U) + 1U))
#define I2C_SPEED_STANDARD(__PCLK__, __SPEED__)            (((((__PCLK__)/((__SPEED__) << 1U)) & I2C_CCR_CCR) < 4U)? 4U:((__PCLK__) / ((__SPEED__) << 1U)))
#define I2C_SPEED_FAST(__PCLK__, __SPEED__, __DUTYCYCLE__) (((__DUTYCYCLE__) == I2C_DUTYCYCLE_2)? ((__PCLK__) / ((__SPEED__) * 3U)) : (((__PCLK__) / ((__SPEED__) * 25U)) | I2C_DUTYCYCLE_16_9))
#define I2C_SPEED(__PCLK__, __SPEED__, __DUTYCYCLE__)      (((__SPEED__) <= 100000U)? (I2C_SPEED_STANDARD((__PCLK__), (__SPEED__))) : \
                                                                  ((I2C_SPEED_FAST((__PCLK__), (__SPEED__), (__DUTYCYCLE__)) & I2C_CCR_CCR) == 0U)? 1U : \
                                                                  ((I2C_SPEED_FAST((__PCLK__), (__SPEED__), (__DUTYCYCLE__))) | I2C_CCR_FS))

#define I2C_7BIT_ADD_WRITE(__ADDRESS__)                    ((uint8_t)((__ADDRESS__) & (~I2C_OAR1_ADD0)))
#define I2C_7BIT_ADD_READ(__ADDRESS__)                     ((uint8_t)((__ADDRESS__) | I2C_OAR1_ADD0))

#define I2C_10BIT_ADDRESS(__ADDRESS__)                     ((uint8_t)((uint16_t)((__ADDRESS__) & (uint16_t)(0x00FFU))))
#define I2C_10BIT_HEADER_WRITE(__ADDRESS__)                ((uint8_t)((uint16_t)((uint16_t)(((uint16_t)((__ADDRESS__) & (uint16_t)(0x0300U))) >> 7U) | (uint16_t)(0x00F0U))))
#define I2C_10BIT_HEADER_READ(__ADDRESS__)                 ((uint8_t)((uint16_t)((uint16_t)(((uint16_t)((__ADDRESS__) & (uint16_t)(0x0300U))) >> 7U) | (uint16_t)(0x00F1U))))

#define I2C_MEM_ADD_MSB(__ADDRESS__)                       ((uint8_t)((uint16_t)(((uint16_t)((__ADDRESS__) & (uint16_t)(0xFF00U))) >> 8U)))
#define I2C_MEM_ADD_LSB(__ADDRESS__)                       ((uint8_t)((uint16_t)((__ADDRESS__) & (uint16_t)(0x00FFU))))


int i2c_init(void)
{
	static const uint8_t APBAHBPrescTable[16] = {0U, 0U, 0U, 0U, 1U, 2U, 3U, 4U, 1U, 2U, 3U, 4U, 6U, 7U, 8U, 9U};

	uint32_t pclk1 = SystemCoreClock >> APBAHBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> POSITION_VAL(RCC_CFGR_PPRE1)];
	uint32_t freqrange = I2C_FREQRANGE(pclk1);

	I2C3->CR1 &= ~I2C_CR1_PE;
	I2C3->CR2 = freqrange;
	I2C3->TRISE = I2C_RISE_TIME(freqrange, 400000);
	I2C3->CCR = I2C_SPEED(pclk1, 400000, I2C_CCR_DUTY);
	I2C3->CR1 = 0;
	I2C3->OAR1 = 0x00004000U;
	I2C3->OAR2 = 0;
	I2C3->CR1 |= I2C_CR1_PE;
}

/* Enable Acknowledge */
hi2c->Instance->CR1 |= I2C_CR1_ACK;

/* Generate Start */
hi2c->Instance->CR1 |= I2C_CR1_START;

/* Wait until SB flag is set */
if(I2C_WaitOnFlagUntilTimeout(hi2c, I2C_FLAG_SB, RESET, Timeout, Tickstart) != HAL_OK)
{
  return HAL_TIMEOUT;
}


int i2c_tx_start(uint8_t device, bool ack)
{
	unsigned int timeout = I2C_TIMEOUT;

	if (ack)
		I2C3->CR1 |= I2C_CR1_ACK;
	I2C3->CR1 |= I2C_CR1_START;

	/* EV5 */
	while ((I2C3->SR1 & I2C_SR1_SB) == 0)
		if (--timeout == 0x00)
			return -1;
	I2C3->DR = device & ~I2C_OAR1_ADD0;

	/* EV6 */
	timeout = I2C_TIMEOUT;
	while ((I2C3->SR1 & I2C_SR1_ADDR) == 0)
		if (--timeout == 0x00)
			return -1;
	(void)I2C3->SR2;

	return 0;
}

int i2c_rx_start(uint8_t device, bool ack)
{
	unsigned int timeout = I2C_TIMEOUT;

	if (ack)
		I2C3->CR1 |= I2C_CR1_ACK;
	I2C3->CR1 |= I2C_CR1_START;

	while ((I2C3->SR1 & I2C_SR1_SB) == 0)
		if (--timeout == 0)
			return -1;

	I2C3->DR = device | I2C_OAR1_ADD0;

	timeout = I2C_TIMEOUT;
	while ((I2C3->SR1 & I2C_SR1_ADDR) == 0)
		if (--timeout == 0x00)
			return -1;

	(void)I2C3->SR2;

	return 0;
}

int i2c_write_data(uint8_t data)
{
	unsigned int timeout = I2C_TIMEOUT;

	while ((I2C3->SR1 & I2C_SR1_TXE) == 0)
		if (--timeout == 0)
			return -1;

	I2C3->DR = data;

	return 0;
}

int i2c_stop(void)
{
	unsigned int timeout = I2C_TIMEOUT;

	while ((I2C3->SR1 & I2C_SR1_TXE) == 0 || (I2C3->SR1 & I2C_SR1_BTF) == 0)
		if (--timeout == 0)
			return -1;

	I2C3->CR1 |= I2C_CR1_STOP;

	return 0;
}

int i2c_read_nack(void)
{
	unsigned int timeout = I2C_TIMEOUT;

	I2C3->CR1 &= ~I2C_CR1_ACK;
	I2C3->CR1 |= I2C_CR1_STOP;

	while ((I2C3->SR1 & I2C_SR1_RXNE) != 0)
		if (--timeout == 0)
			return -1;

	return I2C3->DR;
}

int i2c_read_ack(void)
{
	uint8_t data;
	unsigned int timeout = I2C_TIMEOUT;

	I2C3->CR1 |= I2C_CR1_ACK;

	while ((I2C3->SR1 & I2C_SR1_RXNE) != 0)
		if (--timeout == 0)
			return -1;

	return I2C3->DR;
}

int i2c_read(uint8_t device, uint8_t reg, *data, size_t count)
{
	i2c_tx_start(device, true);

	i2c_write_data(reg);

	i2c_rx_start(device, true);

	return i2c_read_nack();
}

int i2c_write(uin8_t device, uint8_t reg, uint8_t *data, size_t count)
{
	return -1;
}

int main(int argc, char **argv)
{
	uint16_t who_am_i;

	I2C_HandleTypeDef i2c3;
	i2c3.Instance = I2C3;
	i2c3.Init.ClockSpeed = 400000;
	i2c3.Init.DutyCycle = I2C_DUTYCYCLE_16_9;
	i2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	i2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLED;
	i2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLED;
	i2c3.Init.OwnAddress1 = 0x00;
	i2c3.Init.OwnAddress2 = 0x00;

//	HAL_I2C_Init(&i2c3);
//	I2C3->CR1 |= I2C_CR1_SWRST;
//	I2C3->CR1 &= ~I2C_CR1_SWRST;

	i2c_init();
	while (true) {
//		int ret = HAL_I2C_Mem_Read(&i2c3, 0x82, 0x00, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&who_am_i, 1, 2000);
		who_am_i = 0;
		int ret = i2c_read(0x82, 0x00, (uint8_t *)&who_am_i, 1);
		who_am_i = __REV16(who_am_i);
		if (who_am_i != 0x0811)
			diag_printf("bad id: 0x%04hx\n", who_am_i);
	}

	return 0;
}

