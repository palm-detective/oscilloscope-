/*
 * Touch_ctl.c
 *
 *  Created on: May 5, 2021
 *      Author: paul_
 */

#include "Touch_ctl.h"
#include "main.h"
#include "stm32f4xx_hal.h"

//SPI port is left set for LCD

// initialize touch screen
void Init_Touch(void)
{
	unsigned char Cmd;
	Touch_SPI();
	HAL_GPIO_WritePin( TpCs_GPIO_Port, TpCs_Pin, GPIO_PIN_RESET);

    Cmd = 0x83; //turn on adc and ref
	HAL_SPI_Transmit(&hspi3, &Cmd, 1, 1);// timeout 1 ms

	HAL_GPIO_WritePin( TpCs_GPIO_Port, TpCs_Pin, GPIO_PIN_SET);
	LCD_SPI();
}

// set SPI for touch controller
void Touch_SPI(void)
{
//	HAL_SPI_DeInit(&hspi3);  switch on the fly
	hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16; //slow clock for touch controller
	HAL_SPI_Init(&hspi3);
}

// set SPI for LCD display
void LCD_SPI(void)
{
//	HAL_SPI_DeInit(&hspi3); switch on fly
	hspi3.Init.DataSize = SPI_DATASIZE_16BIT;
	hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
	HAL_SPI_Init(&hspi3);
}
