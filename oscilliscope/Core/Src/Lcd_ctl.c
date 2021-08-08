/*
 * Lcd_ctl.c
 *
 *  Created on: May 5, 2021
 *      Author: paul_
 */

#include "Lcd_ctl.h"
#include "stm32f4xx_hal.h"
#include "main.h"
#include <stdlib.h>
#include "Func.h"

unsigned short data[15];

void lcd_write_cmd_data( unsigned short Cmd, short Len, unsigned short *Data )
{
	HAL_GPIO_WritePin( LcdRs_GPIO_Port, LcdRs_Pin, GPIO_PIN_RESET); // low for command
	HAL_SPI_Transmit(&hspi3, (unsigned char*)&Cmd, 1, 1);// timeout 1 ms

	if(Len !=0)
	{
		HAL_GPIO_WritePin( LcdRs_GPIO_Port, LcdRs_Pin, GPIO_PIN_SET);  //high for data
		HAL_SPI_Transmit(&hspi3, (unsigned char*)Data, Len, 1);// timeout 1 ms
	}
}

// initialize the LCD
void Init_LCD(void)
{
	HAL_GPIO_WritePin( LcdCs_GPIO_Port, LcdCs_Pin, GPIO_PIN_RESET);  //enable lcd controller

	HAL_GPIO_WritePin( Dres_GPIO_Port, Dres_Pin, GPIO_PIN_SET);  //display reset
	HAL_Delay(1);
	HAL_GPIO_WritePin( Dres_GPIO_Port, Dres_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin( Dres_GPIO_Port, Dres_Pin, GPIO_PIN_SET);
	HAL_Delay(120);

	data[0]=0;
	lcd_write_cmd_data(0xb0, 1, data);  //set interface
	lcd_write_cmd_data(0x11, 0, data);  //sleep
	HAL_Delay(120);
	data[0]=0x55;
	lcd_write_cmd_data(0x3A, 1, data);  //set pixels to 16 bit
	data[0]=data[1]=9;
	lcd_write_cmd_data(0xc0, 2, data);  //set power
	data[0]= 0x41;
	data[1]=0;
	lcd_write_cmd_data(0xc1, 2, data);  //set power
	data[0]= 0x33;
	lcd_write_cmd_data(0xc2, 1, data);  //set power
	data[0]=data[1]=0;
	data[2]=data[3]=0;  //#3 may be 0x36
	lcd_write_cmd_data(0xc5, 4, data);  //Vcom control

	data[0]=  0;
	lcd_write_cmd_data(0x36, 1, data);  //madctl

	/* PGAMCTRL(Positive Gamma Control)
	-1, 0xE0, 0x0F, 0x1F, 0x1C, 0x0C, 0x0F, 0x08, 0x48, 0x98,
	          0x37, 0x0A, 0x13, 0x04, 0x11, 0x0D, 0x00,
	 NGAMCTRL(Negative Gamma Control)/
	-1, 0xE1, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75,
	          0x37, 0x06, 0x10, 0x03, 0x24, 0x20, 0x00,
	 Digital Gamma Control 1
	-1, 0xE2, 0x0F, 0x32, 0x2E, 0x0B, 0x0D, 0x05, 0x47, 0x75,
	          0x37, 0x06, 0x10, 0x03, 0x24, 0x20, 0x00,
    SPI_TRANSFER(0xE0 Positive Gamma Control, 0x00, 0x2C, 0x2C, 0x0B, 0x0C, 0x04, 0x4C, 0x64, 0x36, 0x03, 0x0E, 0x01, 0x10, 0x01, 0x00);
    SPI_TRANSFER(0xE1 Negative Gamma Control, 0x0F, 0x37, 0x37, 0x0C, 0x0F, 0x05, 0x50, 0x32, 0x36, 0x04, 0x0B, 0x00, 0x19, 0x14, 0x0F); */
	data[0]=data[14]=  0;
	data[1]=data[2]=   0x2C;
	data[3]=  0x0B;
	data[4]=  0x0C;
	data[5]=  0x04;
	data[6]=  0x4C;
	data[7]=  0x64;
	data[8]=  0x36;
	data[9]=  0x03;
	data[10]= 0x0E;
	data[11]=data[13]= 0x2C;
	data[12]= 0x10;
	lcd_write_cmd_data(0xe0, 15, data);  //positive gamma control

	data[0]=data[4]= data[14]= 0x0F;
	data[1]=data[2]= 0x37;
	data[3]=  0x0C;
	data[5]=  0x05;
	data[6]=  0x50;
	data[7]=  0x32;
	data[8]=  0x36;
	data[9]=  0x04;
	data[10]= 0x0B;
	data[11]= 0;
	data[12]= 0x19;
	data[13]= 0x14;
	lcd_write_cmd_data(0xe1, 15, data);  //Negitive gamma control

//	SPI_TRANSFER(0xB6/*Display Function Control*/, 0, /*ISC=2*/2, /*Display Height h=*/59); // Actual display height = (h+1)*8 so (59+1)*8=480
	data[0]=0;
	data[1]=2;
	data[2]=59;
	lcd_write_cmd_data(0xb6, 3, data);  //display function control

	lcd_write_cmd_data(0x11, 0, data);  //sleep
	HAL_Delay(120);
	lcd_write_cmd_data(0x29, 0, data);  //display on
	lcd_write_cmd_data(0x38, 0, data);  //idle off
	lcd_write_cmd_data(0x13, 0, data);  //Normal Display Mode ON

	HAL_GPIO_WritePin( LcdCs_GPIO_Port, LcdCs_Pin, GPIO_PIN_SET);  //disable lcd controller

	LCD_ClrScrn(0);  //clear screen to black
}

void LCD_ClrScrn(unsigned short Color)
{
short y;
unsigned short *ptr;

ptr = malloc(320*2);
HAL_GPIO_WritePin( LcdCs_GPIO_Port, LcdCs_Pin, GPIO_PIN_RESET);  //enable lcd controller

for(y=0; y<480; ++y )
{
	data[0]=data[1]= 0;
	data[2]= 319>>8;
	data[3]= 319 & 0xFF;
	lcd_write_cmd_data(0x2A, 4, data);  //set x
	data[0]= y>>8;
	data[1]= y & 0xFF;
	data[2]= 479>>8;
	data[3]= 479 & 0xFF;
	lcd_write_cmd_data(0x2B, 4, data);  //set y

	if(ptr != NULL)
	{
		memset16(ptr, Color, 320);
		lcd_write_cmd_data(0x2C, 320, ptr);  //write out pixels
	}
	else
		break; //bail out
}

free(ptr);

data[0]=data[1]= 0;
data[2]= 319>>8;
data[3]= 319 & 0xFF;
lcd_write_cmd_data(0x2A, 4, data);  //set x
data[0]=data[1]= 0;
data[2]= 479>>8;
data[3]= 479 & 0xFF;
lcd_write_cmd_data(0x2B, 4, data);  //set y

HAL_GPIO_WritePin( LcdCs_GPIO_Port, LcdCs_Pin, GPIO_PIN_SET);  //disable lcd controller

}

void LCD_Set_Posi(short Sx, short Sy)
{
	HAL_GPIO_WritePin( LcdCs_GPIO_Port, LcdCs_Pin, GPIO_PIN_RESET);  //enable lcd controller

	data[0]= data[2]= Sx>>8;
	data[1]= data[3]= Sx & 0xFF;
	lcd_write_cmd_data(0x2A, 4, data);  //set x
	data[0]= data[1]= Sy>>8;
	data[2]= data[3]= Sy & 0xFF;
	lcd_write_cmd_data(0x2B, 4, data);  //set y

	HAL_GPIO_WritePin( LcdCs_GPIO_Port, LcdCs_Pin, GPIO_PIN_SET);    //disable lcd controller
}

void LCD_Set_Pixel(unsigned short Color)
{
	HAL_GPIO_WritePin( LcdCs_GPIO_Port, LcdCs_Pin, GPIO_PIN_RESET);  //enable lcd controller

	data[0]= Color>>8;
	data[1]= Color & 0xFF;
	lcd_write_cmd_data(0x2C, 2, data);  //set pixel
	HAL_GPIO_WritePin( LcdCs_GPIO_Port, LcdCs_Pin, GPIO_PIN_SET);    //disable lcd controller
}

unsigned short LCD_Get_Pixel(void)
{
	short ret;
	unsigned short fred;

	HAL_GPIO_WritePin( LcdCs_GPIO_Port, LcdCs_Pin, GPIO_PIN_RESET);  //enable lcd controller
	lcd_write_cmd_data(0x2E, 0, data);  //read data

	ret = HAL_SPI_Receive(&hspi3, (unsigned char *)data, 2, 1);

	HAL_GPIO_WritePin( LcdCs_GPIO_Port, LcdCs_Pin, GPIO_PIN_SET);    //disable lcd controller

	if( ret == HAL_OK )
		{
		fred =data[0]<<8;
		fred |= data[1];
		return (fred);
		}
	else
		return 0;

}
