/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Lcd_ctl.h"
#include "Touch_ctl.h"
#include "Flash.h"
#include "Draw.h"
#include "Process.h"
#include "File.h"
#include "Menu.h"
#include "Display.h"
#include "Bios.h"
#include "Flash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc1;

DAC_HandleTypeDef hdac;
DMA_HandleTypeDef hdma_dac1;

SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim7;
DMA_HandleTypeDef hdma_tim7_up;

/* USER CODE BEGIN PV */
TIM_OC_InitTypeDef octim1;
TIM_OC_InitTypeDef octim3;
TIM_OC_InitTypeDef octim5;
char  APP_VERSION[] = "V2.00";   //bomp version to 2
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_DAC_Init(void);
static void MX_SPI2_Init(void);
static void MX_SPI3_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM7_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  short i;
  char SAVE_FLAG = 0;
  char Pop_Num = 0;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_DAC_Init();
  MX_SPI2_Init();
  MX_SPI3_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_USB_DEVICE_Init();
  MX_TIM7_Init();
  /* USER CODE BEGIN 2 */

  Init_LCD();
  Init_Touch();

  SetColor(BLK, WHT);

  DispStr8x14(8, 90, PRN, "         Oscilloscope              ");
  DispStr8x14(8+22*8, 90, PRN, APP_VERSION);
  DispStr8x14(8, 70, PRN, "        System Initializing...       ");

  HAL_Delay(1000);

//=============================check callibration׼===========================
  Read_CalFlag();

  if(Cal_Flag == 1)
  {
	  Cal_Flag = 0;
	  SetColor(BLK, WHT);
	  Zero_Align();                              //set zero
	  Restore();                                 //restore list
	  Save_Parameter();                          //save list
	  Save_Kpg();
  }
  Read_Kpg();
  Read_Parameter();
  File_Num();
  memcpy(Data, DEMO, 310);

  LCD_ClrScrn(GRY);
  Update_Title();

  List[SMPL_DPTH].Flg |= UPD;

  StdBy_Time = List[STANDBY].Val * MIN_UNIT;

  // Start timer
  HAL_TIM_Base_Start_IT(&htim3);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	    if((List[STANDBY].Val != 0)&&(StdBy_Time == 0))
	    {
	      StdBy_Flag = 1;
	    }

	    Update_Detail();
	    Update_Blink();
	    WaveProcess();
	    DrawWindow(WaveBuf, ParamTab);

	   if(TIM_BASE_Val!=0)
	    {
	      List[TIM_BASE].Val=TIM_BASE_Val;
	      List[TIM_BASE].Flg |= UPD;
	      TIM_BASE_Val=Current;
	      Update_Title();
	      Current=TIM_BASE_Val;
	      TIM_BASE_Val=0;
	    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	    if(Key_Buffer != 0)
	    {
	      StdBy_Flag = 0;
	      PopupCnt = 10;                  // �ް���ʱ�˵���ά����ʾ 10��
	      List[MenuNow[Current]].Flg |= UPD;

	      switch (Key_Buffer)
	      {
	     case KEYCODE_MEM:
	         i = Save_Bmp(List[SAVE_BMP].Val);
	         DispFileInfo(i);
	         DispBMPInfo();
	        break;

	     case KEYCODE_ACT:
	       SAVE_FLAG=0;
	       if(PopType & DAILOG_POP) Close_Pop();
	       else {
	         Dialog_Pop("Save All ParamTab?");
	         Pop_Num = 1;
	       }
	       break;

	     case KEYCODE_PLAY:
	       if((Current == ST)&&(Cal_Flag==0))
	       {
	         if(PopType & DAILOG_POP){
	           if(Pop_Num == 1){
	             Save_Parameter();
	             Close_Pop();
	           }
	           else{
	             Close_Pop();
	             List_Pop();
	           }
	         }
	         else if((PopType & LIST_POP) && (MenuNow[ST]==RESTORE )) {
	           Restore_CalPar();
	           Dialog_Pop("Save All ParamTab?");
	           Cal_Flag = 1;
	         }
	         else if((PopType & LIST_POP) && (MenuNow[ST]==AUTO_CAL )) {
	           Close_Pop();
	           Calibrate_Pop("Waiting for Calibration...");
	           Cal_Flag = 1;
	           DrawWindow(WaveBuf, ParamTab);
	           Zero_Align();
	           Update_Proc_All();
	           Close_Pop();
	           CalSave_Pop("Save Cal ParamTab?");
	         }
	         break;
	       }
	       else if((Current == ST)&&(Cal_Flag==1)){
	         if(PopType & DAILOG_POP){
	           Save_Kpg();
	           Close_Pop();
	           Cal_Flag = 0;
	         }
	         break;
	       }

	       if ((Current == FN)&&(PopType & LIST_POP)){
	         i = 0;
	         {
	           if(SAVE_FLAG==KEYCODE_MEM)Close_Pop();
	           if(MenuNow[FN] == SAVE_BMP) i = Save_Bmp(List[SAVE_BMP].Val);
	           if(MenuNow[FN] == SAVE_DAT) i = Save_Dat(List[SAVE_DAT].Val);
	           if(MenuNow[FN] == SAVE_BUF) i = Save_Buf(List[SAVE_BUF].Val);
	           if(MenuNow[FN] == SAVE_CSV) i = Save_Csv(List[SAVE_CSV].Val);
	           if(MenuNow[FN] == SAVE_SVG) i = Save_Svg(List[SAVE_SVG].Val);
	           if(MenuNow[FN] == LOAD_DAT) i = Load_Dat(List[LOAD_DAT].Val);
	           if(MenuNow[FN] == LOAD_BUF) i = Load_Buf(List[LOAD_BUF].Val);
	           SAVE_FLAG = 0;
	         }
	         DispFileInfo(i);

	         if(PopType & DAILOG_POP)  Close_Pop();

	       }
	       else if((PopType & DAILOG_POP ) &&(SAVE_FLAG!=KEYCODE_MEM)){
	         Save_Parameter();
	         Cal_Flag = 0;
	         Close_Pop();
	         break;
	       }
	       else {
	          if(Status & STOP){
	            if(Status & LOADM )Status &= ~LOADM;
	            Status &= ~STOP;     // ��������״̬
	            if(List[SYNCMODE].Val==SINGL) ADC_Start();
	            if(List[SYNCMODE].Val==NORM)  ADC_Start();
	            Norm_Clr = 1;
	            SNGL_Kflag = 1;
	            Update_Proc_All();

	            if(*(short*)F_Sector==0xaa55)Restore_Scene();

	          }
	          else {
	            Status |=  STOP;     // ������ͣ״̬
	            Ch1_Posi = List[V0_POSI].Val;
	            Ch2_Posi = List[EXT_POSI].Val;
	            Tri_Posi = List[THRESHOLD].Val;
	          #ifdef SAVEBMP
	            Save_Bmp(List[SAVE_BMP].Val);
	            List[SAVE_BMP].Val++;
	          #endif
	          }
	        }
	        DispStatus();            // ��ʾ����״̬
 	       break;

	      case KEYCODE_D_MANU:
	        if(List[FITTRIGG].Val)
	        {
	          Auto_Fit();
	        }
	        break;

	      case KEYCODE_MANU:
	        if(!(PopType & LIST_POP)){
	            if(Cal_Flag){
	              Cal_Flag = 0;
	              Close_Pop();
	            }else{
	              Cal_Flag = 0;
	              List_Pop(); // �˵����ر�ʱ, �򿪲˵���
	            }
	        }
	        else {
	          Close_Pop();                // �˵����Ѵ�ʱ, �رղ˵���
	          // ------------ �ָ���ʾ��Ϣ��ʾ�����ԭ����ʾ��Ϣ ------------
	          if(Current == XN)      List[T1_POSI   ].Flg |= UPD;
	          else if(Current == YN) List[V1_POSI   ].Flg |= UPD;
	          else                   List[MenuNow[SN]].Flg |= UPD;
	          // ------------ �ָ���ʾ��Ϣ��ʾ���Ҷ�ԭ����ʾ��Ϣ ------------
	          if(MenuNow[Current] == SMPL_DPTH) List[SMPL_DPTH].Flg  |= UPD; // ������ȸ�����ʾ
	          else if(Current == FN)           List[MenuNow[FN]].Flg |= UPD; // ָ�����ļ���д�������ʾ
	          else                             List[MenuNow[ME]].Flg |= UPD; // ָ���Ĳ��β����������ʾ
	        }
	        break;

	      case KEYCODE_UP:
	        if (PopType & LIST_POP){
	          if(MenuNow[Current] > MU_BEGN[Current]) MenuNow[Current]--;
	          else                  MenuNow[Current] += M_ITEMS[Current]-1;
	        } else {
	          if (Current > 0) Current--;
	          else             Current = ST;
	        }
	        break;

	     case KEYCODE_DOWN:
	        if(PopType & LIST_POP){
	          MenuNow[Current]++;
	          if(MenuNow[Current] >=(MU_BEGN[Current]+M_ITEMS[Current])){
	            MenuNow[Current] = MU_BEGN[Current];
	          }
	        } else {
	          if (Current < 7) Current++;
	          else             Current = YN;
	        }
	        break;

	      case KEYCODE_LEFT:
	        if((Current==ST)&& !(PopType & LIST_POP))break;
	        List[MenuNow[Current]].Flg |= UPD;  // ˢ�±��ǰԭ��ʾ��Ŀ����
	        if(List[MenuNow[Current]].Val <=MIN[MenuNow[Current]]){
	          if((MenuNow[Current] == COUPLING)||(MenuNow[Current] == YN_TYPE )||
	            (MenuNow[Current] == XN_TYPE )||(MenuNow[Current] == EXT_HIDE)||
	            ( MenuNow[Current] == FITTRIGG)||(MenuNow[Current] == TR_MODE )||
	            ( MenuNow[Current] == SYNCMODE)||(MenuNow[Current] == EXT_REFN)||
	            (MenuNow[Current] == PROBEATT)||(MenuNow[Current] == TPYE_OUT))
	            List[MenuNow[Current]].Val =MAX[MenuNow[Current]] ;
	        } else {
	          if((MenuNow[Current] == VIEWPOSI)&&((List[MenuNow[Current]].Val) >=150 ))
	            List[MenuNow[Current]].Val-=25;
	          else{
	          //============5.04�޸������߳�����������========
	            if(MenuNow[Current] == THRESHOLD){
	              if((List[V0_POSI].Val + List[THRESHOLD].Val) > 2)  //����������
	                List[MenuNow[Current]].Val--;
	            }
	            else if(MenuNow[Current] == V0_POSI){
	              if((List[V0_POSI].Val + List[THRESHOLD].Val) <2){
	                List[V0_POSI].Val--;
	                List[THRESHOLD].Val++;
	              }else List[MenuNow[Current]].Val--;
	            }
	            else if((MenuNow[Current] ==V1_POSI) && (List[V1_POSI].Val <= List[V2_POSI].Val))
	            List[MenuNow[Current]].Val=List[V2_POSI].Val;
	            else List[MenuNow[Current]].Val--;
	          }
	          //==============================================
	            if(MenuNow[Current] == SMPL_DPTH){
	              List[VIEWPOSI].Val=1 ;
	           }
	            if((MenuNow[Current] ==T2_POSI) && (List[T2_POSI].Val <= List[T1_POSI].Val))
	              List[MenuNow[Current]].Val=List[T1_POSI].Val;
	        }
	        if(Current == TRG) Status &= ~STOP; // �ı�ͬ��ģʽ��ָ�����״̬
	        break;

	      case KEYCODE_RIGHT:
	        if((Current==ST)&& !(PopType & LIST_POP))break;
	        List[MenuNow[Current]].Flg |= UPD;  // ˢ�±��ǰԭ��ʾ��Ŀ����
	        if(List[MenuNow[Current]].Val >=MAX[MenuNow[Current]] ){
	          if((MenuNow[Current] == COUPLING)||(MenuNow[Current] == YN_TYPE )||
	            (MenuNow[Current] == XN_TYPE )||(MenuNow[Current] == EXT_HIDE)||
	            ( MenuNow[Current] == FITTRIGG)||(MenuNow[Current] == TR_MODE )||
	            ( MenuNow[Current] == SYNCMODE)||(MenuNow[Current] == EXT_REFN)||
	            (MenuNow[Current] == PROBEATT)||(MenuNow[Current] == TPYE_OUT))
	            List[MenuNow[Current]].Val =MIN[MenuNow[Current]] ;
	        } else {
	            if(MenuNow[Current] == VIEWPOSI)
	            {
	              if(List[MenuNow[Current]].Val <(DEPTH[List[SMPL_DPTH].Val]-(350-List[SMPL_DPTH].Val)))
	              {
	                if(List[MenuNow[Current]].Val >150 )
	                  List[MenuNow[Current]].Val += 25;
	                else
	                  List[MenuNow[Current]].Val++;
	              }
	            }
	            else{
	              //============4.27�޸������߳�����������========
	              if(MenuNow[Current] == THRESHOLD){
	                if((List[V0_POSI].Val + List[THRESHOLD].Val) < 198)
	                 List[MenuNow[Current]].Val++;
	              }
	              else if(MenuNow[Current] == V0_POSI){
	                if((List[V0_POSI].Val + List[THRESHOLD].Val) > 198){
	                  List[V0_POSI].Val++;
	                  List[THRESHOLD].Val--;
	                }else List[MenuNow[Current]].Val++;
	              }
	              else if((MenuNow[Current] ==V2_POSI) && (List[V2_POSI].Val >= List[V1_POSI].Val))
	                List[MenuNow[Current]].Val=List[V1_POSI].Val;
	              else List[MenuNow[Current]].Val++;
	            }
	            //==============================================
	            if((MenuNow[Current] ==T1_POSI) && (List[T1_POSI].Val >= List[T2_POSI].Val))
	               List[MenuNow[Current]].Val=List[T2_POSI].Val;
	        }
	        if(Current == TRG) {Status &= ~STOP; // �ı�ͬ��ģʽ��ָ�����״̬
	        if(List[SYNCMODE].Val==SINGL)ADC_Start(); }
	        break;

	     } //switch end
	     List[MenuNow[Current]].Flg |= UPD;
	     StdBy_Time  = List[STANDBY].Val * MIN_UNIT;  //����

	      if(PopType & LIST_POP) Show_List();
	      Update_Title();
	      Key_Buffer = 0;
	    } //if key end
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
  hadc2.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */
  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT2 config
  */
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */
  HAL_DAC_SetValue(&hdac, DAC_CHANNEL_2, DAC_ALIGN_12B_R, 620);  //(620*4095)/330;//50mv
  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_HIGH;
  hspi2.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_LSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_16BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_LSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */
  octim1.Pulse = 500;
  octim1.OCMode = TIM_OCMODE_PWM2;
  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 89;
  htim1.Init.CounterMode = TIM_COUNTERMODE_DOWN;
  htim1.Init.Period = 999;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */
  HAL_TIM_OC_ConfigChannel(&htim1, &octim1, 1 );
  __HAL_TIM_URS_ENABLE(&htim1);
  __HAL_TIM_ENABLE_DMA(&htim1, TIM_DMA_CC1 | TIM_DMA_TRIGGER);
  __HAL_TIM_ENABLE(&htim1);
  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */
  octim3.Pulse = 3750;
  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 11;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 7499;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */
  HAL_TIM_OC_ConfigChannel(&htim3, &octim3, 1 );
  __HAL_TIM_ENABLE(&htim3);

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */
  octim5.Pulse = (FARR[List[FREQ_OUT].Val]+1)/2;
  octim5.OCMode = TIM_OCMODE_PWM2;
  octim5.OCFastMode = TIM_OCFAST_ENABLE;
  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 9;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 18000;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */
  htim5.Init.Period = FARR[List[FREQ_OUT].Val];
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_OC_ConfigChannel(&htim5, &octim5, 3 );
  __HAL_TIM_URS_ENABLE(&htim5);
  __HAL_TIM_ENABLE(&htim5);
  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */
  // aka tim_da
  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 2;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 999;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */
  __HAL_TIM_ENABLE_DMA(&htim7,TIM_DMA_UPDATE);
  __HAL_TIM_ENABLE(&htim7);
  /* USER CODE END TIM7_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Fo2_GPIO_Port, Fo2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, Az_Pin|Ay_Pin|LcdRs_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Ax_Pin|Dres_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SS_Pin|TpCs_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LcdCs_GPIO_Port, LcdCs_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : Fo2_Pin */
  GPIO_InitStruct.Pin = Fo2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Fo2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Az_Pin Ay_Pin LcdRs_Pin LcdCs_Pin */
  GPIO_InitStruct.Pin = Az_Pin|Ay_Pin|LcdRs_Pin|LcdCs_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Ax_Pin SS_Pin TpCs_Pin Dres_Pin */
  GPIO_InitStruct.Pin = Ax_Pin|SS_Pin|TpCs_Pin|Dres_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : Boot1_Pin */
  GPIO_InitStruct.Pin = Boot1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Boot1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TpI_Pin */
  GPIO_InitStruct.Pin = TpI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(TpI_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
// Callback: timer has rolled over
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  // Check which version of the timer triggered this callback
  if (htim == &htim3 )
  {
//	  htim3->Instance->SR =0;
	  Tim3_ISP();
  }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
