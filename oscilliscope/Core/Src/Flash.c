/*
 * Flash.c
 *
 *  Created on: May 9, 2021
 *      Author: paul_
 */

#include "Flash.h"
#include "Menu.h"
#include "Process.h"
#include "Draw.h"
#include "stm32f4xx_hal.h"

#define Kpg_Address     0x08004000  //only a few bytes Sector 1
#define Page_Address    0x08008000  //parameter address sector 2

char  Cal_Flag = 1;  //not callibrated
short TIM_BASE_Val;
unsigned short F_Buff[128];
unsigned long err;

FLASH_EraseInitTypeDef Flash_Erase ={0, 1, 1, 1, 2};

 void Read_CalFlag(void)
 {
	  unsigned short* ptr;
	  ptr=(unsigned short*)Kpg_Address;
	  if(*ptr++!=0xaa55)
		  return;
	  if(*ptr++!=0x0400)
		  return;
	  Cal_Flag = *ptr++;
	  return;
 }

 /*******************************************************************************
  Save_Parameter:                          Return: 0= Success
 *******************************************************************************/
 char Save_Parameter(void)
 {
   short i, j;
   unsigned short* ptr;

   Flash_Erase.Sector = 2;

   HAL_FLASH_Unlock();
   j=HAL_FLASHEx_Erase( &Flash_Erase, &err);
   if(j== HAL_OK)
   {
     ptr=F_Buff;
     *ptr++=0x0400;                              //Ver:4.00
     *ptr++=Current;
     *ptr++=Status;
     for(i=0;i<42;i++)
    	 *ptr++=List[i].Val;
     for(i=0;i<8;i++)
    	 *ptr++=MenuNow[i];
     *ptr++=0xaa55;
     ptr=F_Buff;
     for(i=0; i<256; i+=2)
     {                      //128 SHORT
       j= HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Page_Address+i, *ptr++);
       if(j!= HAL_OK)
    	   break;
     }
   }
   HAL_FLASH_Lock();

   return j;
 }
 /*******************************************************************************
 Read_Parameter:
 *******************************************************************************/
 void Read_Parameter(void)
 {
   short i;
   unsigned short* ptr;
   ptr=(unsigned short*)(Page_Address+55*2); //94*2-40
   TIM_BASE_Val=List[TIM_BASE].Val;
   List[TIM_BASE].Val=0;
   if(*ptr!=0xaa55)
	   return ;
   ptr=(unsigned short*)Page_Address;
   if(*ptr++!=0x0400)
	   return ;
   Current=*ptr++;
   List[MenuNow[Current]].Flg |= UPD;
   ptr++;
   for(i=0;i<42;i++)
	   List[i].Val=*ptr++;
   for(i=0;i<8;i++)
	   MenuNow[i]=*ptr++;
   TIM_BASE_Val=List[TIM_BASE].Val;
   List[TIM_BASE].Val=0;

   return ;
 }
 /*******************************************************************************
  Save_Kpg:                       Return: 0= Success
 *******************************************************************************/
 char Save_Kpg(void)
 {
   short i, j;
   unsigned short* ptr;

   Flash_Erase.Sector = 1;

   HAL_FLASH_Unlock();
   j=HAL_FLASHEx_Erase( &Flash_Erase, &err);
   if(j== HAL_OK)
   {
     ptr=F_Buff;
     *ptr++=0xaa55;
     *ptr++=0x0400;                              //Ver:3.00
     *ptr++=Cal_Flag;
     for(i=0;i<3;i++)
    	 *ptr++=Kpg[i];
     ptr=F_Buff;
     for(i=0; i<256; i+=2)
     {                      //128 SHORT
       j= HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, Kpg_Address+i, *ptr++);
       if(j!= HAL_OK)
    	   break;
     }
   }
   HAL_FLASH_Lock();
   return j;
 }
 /*******************************************************************************
 Read_Kpg:
 *******************************************************************************/
 void Read_Kpg(void)
 {
   short i;
   unsigned short* ptr;
   ptr=(unsigned short*)Kpg_Address;
   if(*ptr++!=0xaa55)
	   return ;
   if(*ptr++!=0x0400)
	   return ;
   Cal_Flag = *ptr++;
   for(i=0;i< 3;i++)
       Kpg[i] = *ptr++;  //CH_A,CH_B
   return ;
 }

 void Restore(void)
 {
   short i;
   TIM_BASE_Val=List[TIM_BASE].Val;
   Current=0;
   List[MenuNow[Current]].Flg |= UPD;
   for(i=0;i<42;i++)
	   List[i].Val= List_c[i].Val;
   for(i=0;i<8;i++)
	   MenuNow[i]= MenuNow_C[i];
   Update_Title();
}

 void Restore_CalPar(void)
 {
   short i = 0;
   List[SMPL_DPTH].Flg |= UPD;
   TIM_BASE_Val=List[TIM_BASE].Val;
   List[TIM_BASE].Val=0;
   Current=0;
   List[MenuNow[Current]].Flg |= UPD;
   for(i=0;i<42;i++) List[i].Val= List_c[i].Val;
   for(i=0;i<8;i++)  MenuNow[i]= MenuNow_C[i];
   TIM_BASE_Val=List[TIM_BASE].Val;
   List[TIM_BASE].Val=0;
   for(i=Y_RANGES; i<=STANDBY; i++){
   List[i].Flg |= UPD;
   }
   Update_Title();
   Close_Pop();
 }
