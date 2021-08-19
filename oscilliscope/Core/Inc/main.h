/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern DAC_HandleTypeDef hdac;
extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_dac1;
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim5;
extern TIM_HandleTypeDef htim7;
extern TIM_OC_InitTypeDef octim1;
extern TIM_OC_InitTypeDef octim5;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Fo2_Pin GPIO_PIN_2
#define Fo2_GPIO_Port GPIOA
#define Fo4_Pin GPIO_PIN_4
#define Fo4_GPIO_Port GPIOA
#define Ap_Pin GPIO_PIN_5
#define Ap_GPIO_Port GPIOA
#define Ao_Pin GPIO_PIN_6
#define Ao_GPIO_Port GPIOA
#define AoA7_Pin GPIO_PIN_7
#define AoA7_GPIO_Port GPIOA
#define Az_Pin GPIO_PIN_4
#define Az_GPIO_Port GPIOC
#define Ay_Pin GPIO_PIN_5
#define Ay_GPIO_Port GPIOC
#define Ax_Pin GPIO_PIN_0
#define Ax_GPIO_Port GPIOB
#define Boot1_Pin GPIO_PIN_2
#define Boot1_GPIO_Port GPIOB
#define SS_Pin GPIO_PIN_12
#define SS_GPIO_Port GPIOB
#define SCK_Pin GPIO_PIN_13
#define SCK_GPIO_Port GPIOB
#define So_Pin GPIO_PIN_14
#define So_GPIO_Port GPIOB
#define Si_Pin GPIO_PIN_15
#define Si_GPIO_Port GPIOB
#define D__Pin GPIO_PIN_11
#define D__GPIO_Port GPIOA
#define D_A12_Pin GPIO_PIN_12
#define D_A12_GPIO_Port GPIOA
#define LcdRs_Pin GPIO_PIN_11
#define LcdRs_GPIO_Port GPIOC
#define LcdCs_Pin GPIO_PIN_12
#define LcdCs_GPIO_Port GPIOC
#define TpCk_Pin GPIO_PIN_3
#define TpCk_GPIO_Port GPIOB
#define TpSo_Pin GPIO_PIN_4
#define TpSo_GPIO_Port GPIOB
#define TpSi_Pin GPIO_PIN_5
#define TpSi_GPIO_Port GPIOB
#define TpCs_Pin GPIO_PIN_6
#define TpCs_GPIO_Port GPIOB
#define TpI_Pin GPIO_PIN_8
#define TpI_GPIO_Port GPIOB
#define Dres_Pin GPIO_PIN_9
#define Dres_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
