/*
 * Ext_flash.c
 *
 *  Created on: Jun 4, 2021
 *      Author: paul_
 */

#include "Ext_flash.h"
#include "stm32f4xx_hal.h"
#include "main.h"

#define TMAX         100000       //

unsigned Tout;

/*******************************************************************************
 SPI_FLASH_SendByte
*******************************************************************************/
char ExtFlash_SendByte(char byte)
{
   char retbyte=0;
	Tout = 0;
  while(HAL_SPI_GetState( &hspi2 ) != HAL_SPI_STATE_READY) /*SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET*/
  {
    if(Tout++ > TMAX)
    	return 255;
  }
  HAL_SPI_Transmit(&hspi2, (unsigned char*)&byte, 1, 1);// timeout 1 ms  SPI_I2S_SendData(SPI2, byte);
  Tout = 0;
  while(HAL_SPI_GetState( &hspi2 ) != HAL_SPI_STATE_READY) //  while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
  {
    if(Tout++ > TMAX)
    	return 255;
  }
  HAL_SPI_Receive(&hspi2, (unsigned char*)&retbyte, 1, 1); //timeout 1 ms
  return retbyte/*SPI_I2S_ReceiveData(SPI2)*/;
}

/*******************************************************************************
 SPI_FLASH_WaitForWriteEnd
*******************************************************************************/
void ExtFlash_WaitForWriteEnd(void)
{
  char FLASH_Status = 0;

  HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_RESET); //ExtFlash_CS_LOW();
  ExtFlash_SendByte(RDSR);
  Tout = 0;
  do
  {
	  FLASH_Status = ExtFlash_SendByte(Dummy_Byte);
      if(Tout++ > TMAX)
    	  return;
  }
  while((FLASH_Status & WIP_Flag) == SET);              // Write in progress
  HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_SET);
}

/*******************************************************************************
 SPI_FLASH_BufferRead
*******************************************************************************/
void ExtFlash_PageRD(char* pBuffer, unsigned ReadAddr, short Length)
{
  HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_SET); //ExtFlash_CS_HIGH();
  ExtFlash_WaitForWriteEnd();
  HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_RESET); //ExtFlash_CS_LOW();
  ExtFlash_SendByte(READ);
  ExtFlash_SendByte((ReadAddr & 0xFF0000) >> 16);
  ExtFlash_SendByte((ReadAddr& 0xFF00) >> 8);
  ExtFlash_SendByte(ReadAddr & 0xFF);

  while(Length--)
  { // while there is data to be read
    *pBuffer = (ExtFlash_SendByte(Dummy_Byte));
    pBuffer++;
  }
  HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_SET);
}

/*******************************************************************************
 SPI_FLASH_WriteEnable
*******************************************************************************/
void ExtFlash_WriteEnable(void)
{
  HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_RESET); //ExtFlash_CS_LOW();
  ExtFlash_SendByte(WREN);
  HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_SET); //ExtFlash_CS_HIGH();
}

/*******************************************************************************
 дFLASHҳ(256 Bytes)�� Mode=0: ��0��1���ݸ�д   Mode=1: ������д
*******************************************************************************/
void ExtFlash_PageProg(char* pBuffer, unsigned WriteAddr)
{
  short   Lenght = 256;

  HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_SET); //ExtFlash_CS_HIGH();
  ExtFlash_WaitForWriteEnd();
  ExtFlash_WriteEnable();
  HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_RESET); //ExtFlash_CS_LOW();
  ExtFlash_SendByte(PP);
  ExtFlash_SendByte((WriteAddr & 0xFF0000) >> 16);
  ExtFlash_SendByte((WriteAddr & 0xFF00) >> 8);
  ExtFlash_SendByte(WriteAddr & 0xFF);
  while(Lenght--) {
    ExtFlash_SendByte((*pBuffer));
    pBuffer++;
  }
  HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_SET); //ExtFlash_CS_HIGH();
  ExtFlash_WaitForWriteEnd();
}

/*******************************************************************************
 SPI_FLASH_SectorErase  : Sector Erases the specified FLASH Page.(4k/sector)
*******************************************************************************/
void ExtFLASH_SectorErase(unsigned SectorAddr)
{
    ExtFlash_WriteEnable();
    HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_RESET); //ExtFlash_CS_LOW();
    ExtFlash_SendByte(SE);
    ExtFlash_SendByte((SectorAddr & 0xFF0000) >> 16);   //Send high address byte
    ExtFlash_SendByte((SectorAddr & 0xFF00) >> 8);      //Send medium address byte
    ExtFlash_SendByte(SectorAddr & 0xFF);               //Send low address byte */
    HAL_GPIO_WritePin(SS_GPIO_Port, SS_Pin, GPIO_PIN_SET); //ExtFlash_CS_HIGH();
    ExtFlash_WaitForWriteEnd();                         // Wait the end of Flash writing
}

void ExtFlash_PageWR(char* pBuffer, unsigned WriteAddr)
{
  unsigned addr,i,j;
  char* ptr;
  char  page=0,flag=0;
  char  buffer[256];

  flag=0;
    addr=WriteAddr & 0xFFF000;
    page=16;
  while(page>0)
  {
    ExtFlash_PageRD((char*)&buffer,addr, 256);
    for(j=0;j<255;j++){
      if(buffer[j++]!=0xff){
        flag=1;
        break;
      }
    }
    addr+=256;
    page--;
  }
    page=16;
    addr=WriteAddr & 0xFFF000;
    if(flag==1)ExtFLASH_SectorErase(addr);
  ptr=pBuffer;
  for(i=0;i<page;i++){
    ExtFlash_PageProg(ptr, addr);
    addr+=256;
    ptr+=256;
  }
}
