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

unsigned char val[2]; //input buffer

// initialize touch screen
void Init_Touch(void)
{
	Touch_SPI();
	HAL_GPIO_WritePin( TpCs_GPIO_Port, TpCs_Pin, GPIO_PIN_RESET);
// not needed yet
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

short read_2046( unsigned char cmd )  //internal value read
{
	short xyz;

	HAL_SPI_Transmit(&hspi3, &cmd, 1, 1);// timeout 1 ms
	HAL_SPI_Receive(&hspi3,val,2,1);   // read value

	xyz = (val[0] <<8) +val[1];
	xyz = xyz >> 3;  //12 bit value
	return xyz;
}

char Touch_Read(short *x, short*y)
{
	short z1,z2;
	short press;
	press =0;

	z1 = read_2046(0xB0);  //read z1 cmd 8+3
	z2 = read_2046(0xD0);  //read z2 cmd 8+5

	return (press<400)?0:1;
}

