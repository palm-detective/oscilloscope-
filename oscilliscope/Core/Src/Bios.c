/*
 * Bios.c
 *
 *  Created on: May 10, 2021
 *      Author: paul_
 */

#include "stm32f4xx_hal.h"
#include "main.h"
#include "Bios.h"
#include "Menu.h"
#include "Process.h"
#include "Draw.h"
#include "Flash.h"
#include "Touch_ctl.h"

volatile char  Twink = 0, Blink, Key_Buffer = 0/*, TmpKeyP = 0, TmpKeyM = 0*/;
short  KEYTIME = 0;
volatile short Cursor_Cnt = 0, /*Sec_Cnt,8*/ mS_Cnt = 0, Tim_Cnt, Delay_Cnt = 0, PopupCnt = 10;

// orginal72MHZ new 90 MHZ FPSC and PSC
//----------+-------+-------+-------+-------+-------+-------+-------+-------+
//          |   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |
//          |   8   |   9   |  10   |  11   |  12   |  13   |  14   |  15   |
//----------+-------+-------+-------+-------+-------+-------+-------+-------+
//            "10Hz", "20Hz", "50Hz","100Hz","200Hz","500Hz","1KHz ","2KHz",
//            "5KHz","10KHz","20KHz","50KHz",".1MHz",".2MHz",".5MHz","1MHz"
//----------+-------+-------+-------+-------+-------+-------+-------+-------+
// old 72MHZ   128-1,  128-1,   64-1,   64-1,   16-1,   16-1,    4-1,    4-1,
//               1-1,    1-1,    1-1,    1-1,    1-1,    1-1,    1-1,    1-1
const short FPSC[]={  160-1,  160-1,   80-1,   80-1,   20-1,   20-1,    5-1,    5-1,
                        1-1,    1-1,    1-1,    1-1,    1-1,    1-1,    1-1,    1-1};
const short FARR[]={56250-1,28125-1,22500-1,11250-1,22500-1, 9000-1, 18000-1,9000-1,
                    14400-1, 7200-1, 3600-1, 1440-1,  720-1,  360-1,  144-1,   72-1};
//----------+-------+-------+-------+-------+-------+-------+-------+-------+

//----------+------+------+------+------+------+------+------+------+------+------+
//          |   0  |   1  |   2  |   3  |   4  |   5  |   6  |   7  |   8  |   9  |
//          |  10  |  11  |  12  |  13  |  14  |  15  |  16  |  17  |  18  |  19  |
//----------+------+------+------+------+------+------+------+------+------+------+
//           {"1uS " ,"2uS " ,"5uS ","10uS","20uS","50uS",".1mS",".2mS",".5mS","1mS " ,
//            "2mS " ,"5mS " ,"10mS","20mS","50mS","0.1S","0.2S","0.5S"," 1S "," 2S ",};
//----------+------+------+------+------+------+------+------+------+------+------+
const short PSC[] ={ 12-1 , 12-1 , 12-1 , 12-1 , 12-1 , 16-1 , 16-1 , 16-1 , 16-1 , 16-1 ,
              16-1 , 32-1 ,  64-1, 64-1 , 128-1, 256-1, 256-1, 256-1, 512-1, 512-1};
const short ARR[] ={ 6-1  , 6-1 ,  6-1  , 6-1 , 6-1  ,  9-1 , 18-1 , 36-1 ,  90-1 ,   180-1,
              360-1, 450-1, 450-1, 900-1,1125-1,1125-1,2250-1,5625-1, 5625-1,11250-1};

//----------+------+------+------+------+------+------+------+------+------+------+

GPIO_PinState Touch, LastTouch =0;

/*******************************************************************************
 Info :  Ӳ���豸��Ϣ
*******************************************************************************/
unsigned BIOS_Info(char Item)
{
  switch (Item){
    case CHA_CNT:
    	return /*(u16)DMA_CH_A->CNDTR*/0;
/*    case SIN_TAB: return (u32)SIN_DATA;
    case TRG_TAB: return (u32)TRG_DATA;
    case SAW_TAB: return (u32)SAW_DATA;    */
    default:
    	return 0;
  }
}
/*******************************************************************************
 DevCtrl:  Ӳ���豸����
*******************************************************************************/
void BIOS_Ctrl(char Item, unsigned Val)
{
  GPIO_InitTypeDef         GPIO_InitStructure;

  switch (Item){
    case SMPL_ST: if(Val == DISABLE)
    			  {
                    /*DMA1_Channel1->CCR &= (~ENABLE);*/
                  }
    			  else
    				  if(Val == ENABLE)
    				  {
                   /* DMA1_Channel1->CCR |= ENABLE; */
                    __HAL_ADC_ENABLE(&hadc1);  //ADC1->CR2 |= 0x00000001; //AD_ON
                    __HAL_ADC_ENABLE(&hadc2);  //ADC2->CR2 |= 0x00000001; //AD_ON
                    __HAL_TIM_ENABLE(&htim1);  /*TIM1->CR1 = 0x0081; */
    				  }
    				  else
    				  {
/*                    if(Val == SIMULTANEO)
                    {
                      DMA1_Channel1->CCR = 0x3580;
                    }
                    if(Val == INTERLEAVE)
                    {
                      DMA1_Channel1->CCR = 0x3A80;
                    } */
                  }
    			  break;

    case SMPLTIM:
    			  octim1.Pulse = (Val+1)/2;
    			  HAL_TIM_OC_ConfigChannel(&htim1, &octim1, 1 );
    			  htim1.Init.Prescaler = 0;
    			  htim1.Init.Period = Val -1;
    			  HAL_TIM_Base_Init(&htim1);
                  break;

    case SMPLNUM:
    			  __HAL_TIM_URS_DISABLE(&htim1);
    			  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    			  HAL_TIM_Base_Init(&htim1);
                  /* DMA_CH_A->CNDTR = (u32)Val;    */
                  break;

    case AiRANGE: if(Val & DC)
    				  HAL_GPIO_WritePin(Az_GPIO_Port, Az_Pin,GPIO_PIN_SET);  // AZ_HIGH;
                  else
                	  HAL_GPIO_WritePin(Az_GPIO_Port, Az_Pin,GPIO_PIN_RESET);  //AZ_LOW;   CH_A_AC
                  if(Val & HV)
                	  HAL_GPIO_WritePin(Ay_GPIO_Port, Ay_Pin,GPIO_PIN_SET);  //AY_HIGH;  CH_A_HV
                  else
                	  HAL_GPIO_WritePin(Ay_GPIO_Port, Ay_Pin,GPIO_PIN_RESET);  //AY_LOW;   CH_A_LV
                  if(Val & ACT)
                	  HAL_GPIO_WritePin(Ax_GPIO_Port, Ax_Pin,GPIO_PIN_SET);  //AX_HIGH;  CH_A_ACT
                  else
                	  HAL_GPIO_WritePin(Ax_GPIO_Port, Ax_Pin,GPIO_PIN_RESET);  //AX_LOW;   CH_A_GND
                  break;

    case AOFFSET: HAL_DAC_SetValue( &hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, Val);  /*DAC->DHR12R2  = (u16)Val; */
                  break;

    case DAC_TIM:
    			  htim7.Init.Period = Val-1;
    			  HAL_TIM_Base_Init(&htim7);
    			  break;

    case OUT_BUF:
    			  /* OUT_DMA->CCR   = 0x15B0; // PL=01, M/P_SIZE=0101, M/P_INC=10, CIRC=1, DIR=1, En=0
                  OUT_DMA->CPAR  = (u32)&DAC->DHR12R1;
                  OUT_DMA->CMAR  = (u32)Val; */
                  break;

    case OUT_CNT: /*OUT_DMA->CNDTR = (u16)Val; */    break;

    case OUT_MOD:
      GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_MEDIUM;
      switch (Val){
                    case PULSED:
//                                  OUT_DMA->CCR &= 0xFFFE;               //�رղ������DMA
                    		      __HAL_DAC_DISABLE(&hdac, DAC_CHANNEL_1);
                                  GPIO_InitStructure.Pin   = Fo2_Pin;
                                  GPIO_InitStructure.Mode  = GPIO_MODE_AF_PP;
                                  HAL_GPIO_Init(Fo2_GPIO_Port, &GPIO_InitStructure);

                                  GPIO_InitStructure.Pin   = Fo4_Pin ;
                                  GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
                                  HAL_GPIO_Init(Fo4_GPIO_Port, &GPIO_InitStructure);
                                  break;

                    case ANALOG:
                    			  GPIO_InitStructure.Pin   = Fo2_Pin ;
                                  GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
                                  HAL_GPIO_Init(Fo2_GPIO_Port, &GPIO_InitStructure);
                                  GPIO_InitStructure.Pin   = Fo4_Pin ;
                                  GPIO_InitStructure.Mode  = GPIO_MODE_ANALOG;
                                  HAL_GPIO_Init(Fo4_GPIO_Port, &GPIO_InitStructure);

//                                  OUT_DMA->CCR |= ENABLE;               //���������DMA
                                  __HAL_DAC_ENABLE(&hdac, DAC_CHANNEL_1);
                                  break;

                    case DISABLE:
 //                                 OUT_DMA->CCR &= 0xFFFE;                //�رղ������DMA
                    		      __HAL_DAC_DISABLE(&hdac, DAC_CHANNEL_1);

                                  GPIO_InitStructure.Pin   = Fo2_Pin ;
                                  GPIO_InitStructure.Mode  = GPIO_MODE_AF_PP;
                                  HAL_GPIO_Init(Fo2_GPIO_Port, &GPIO_InitStructure);

                                  GPIO_InitStructure.Pin   = Fo4_Pin ;
                                  GPIO_InitStructure.Mode  = GPIO_MODE_INPUT;
                                  HAL_GPIO_Init(Fo4_GPIO_Port, &GPIO_InitStructure);
                                  break;
                  }
                  break;

    default: break;
  }
}

/*******************************************************************************
 Set_Base: ˮƽɨ��ʱ��ת������
*******************************************************************************/
void Set_Base(short Base)
{
	octim1.Pulse = (ARR[Base]+1)/2;
	HAL_TIM_OC_ConfigChannel(&htim1, &octim1, 1 );
	htim1.Init.Prescaler = PSC[Base];
	htim1.Init.Period = ARR[Base];
	HAL_TIM_Base_Init(&htim1);
}

/*******************************************************************************
 Set_Duty: �������ռ�ձ�����
*******************************************************************************/
void Set_Duty(void)
{
	 octim5.Pulse = (FARR[List[FREQ_OUT].Val]+1)*(10-List[FREQ_DUT].Val)/10;
	 HAL_TIM_OC_ConfigChannel(&htim5, &octim5, 3 );
}

/*******************************************************************************
 Set_Fout: �������Ƶ������
*******************************************************************************/
void Set_Fout(short Range)
{
	octim5.Pulse = (FARR[Range]+1)/2;
	HAL_TIM_OC_ConfigChannel(&htim5, &octim5, 3 );
	htim5.Init.Prescaler = FPSC[Range];
	htim5.Init.Period = FARR[Range];
	HAL_TIM_Base_Init(&htim5);
}

/*******************************************************************************
 ADC_Start: ���¿�ʼADCɨ�����
*******************************************************************************/
void ADC_Start(void)
{
  Sampl[0]   = List[TIM_BASE].Val;         // �������ʱ����λֵ
//  DMA1_Channel1->CCR   &= 0xFFFFFFFFE;
//  DMA1_Channel1->CMAR  = (u32)&Sampl[2];             // �����趨DMAͨ��1
  if(( List[SYNCMODE].Val == NONE|| List[SYNCMODE].Val == SCAN)
     &&( List[TIM_BASE].Val>11))
  {
	  __HAL_DMA_SET_COUNTER(&hdma_adc1, 302);//       DMA1_Channel1->CNDTR  = 302;                       //10ms
       List[VIEWPOSI].Val= 0;
     }
  else
  __HAL_DMA_SET_COUNTER(&hdma_adc1, DEPTH[List[SMPL_DPTH].Val]);//    DMA1_Channel1->CNDTR = DEPTH[List[SMPL_DPTH].Val]; // 0x00001000;
//  DMA1_Channel1->CCR  |= 0x00000001;                 // ���¿�ʼɨ�����
}

/*******************************************************************************
* Tim3�жϴ������
*******************************************************************************/
void Tim3_ISP(void)
{

	char checkpress;
	short x,y;
//  unsigned char KeyCode;
//  TIM3->SR = 0;                             //���жϱ�־
//  if(KeymS_F)KeymS_Cnt++;                  //�������ʱ
  Tim_Cnt++;
  if(Delay_Cnt>0) Delay_Cnt--;
  if (mS_Cnt > 0)
  {
    mS_Cnt--;
    if ((mS_Cnt%20)== 0)
    {                 //  20mS  ÿ20mS��1�μ���
    	checkpress =0 ;
    	Touch = HAL_GPIO_ReadPin(TpI_GPIO_Port, TpI_Pin);  //read pen
    	if (Touch != LastTouch)
    	{
    	// pen changed
    		checkpress =1; //check pressure if pen changed
    		LastTouch = Touch;
    	}
    	else
    	{
    	  if(Touch != 0)
    	  {
    	    //check that press is valid
    		checkpress =1;
    	  }
    	}

    	if(checkpress !=0)
    	{
    		Touch_Read(&x,&y);
       	}
//      if(Key_Wait_Cnt)    Key_Wait_Cnt--;
//      if(Key_Repeat_Cnt)  Key_Repeat_Cnt--;
//      KeyCode = KeyScan();
//      if(KeyCode !=0) Key_Buffer = KeyCode;

      if(Cursor_Cnt >0)
    	  Cursor_Cnt--;
      else
      {
        Cursor_Cnt = 12;                  // 12*20mS=240mS
        Twink = !Twink;
        Blink =  BLINK;
      }
    }
  }
  else
  {  //one second
    mS_Cnt = 1000;

    //----------------����----------------
    if((List[STANDBY].Val !=0) && (StdBy_Time > 0))
      StdBy_Time--;      //������ʱ

    if(PopupCnt > 0)
    {
      if(Cal_Flag == 0)
    	  PopupCnt--;
    }
    else
    {
      Close_Pop();        // �رնԻ���
      if(Windows_Pop)
      {
        Windows_Pop = 0;
        Clr_WavePosi(DAR);
      }
    }
  }
}
