/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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
#define Temperature_Eight_Pin GPIO_PIN_1
#define Temperature_Eight_GPIO_Port GPIOA
#define Temperature_Seven_Pin GPIO_PIN_2
#define Temperature_Seven_GPIO_Port GPIOA
#define Temperature_Six_Pin GPIO_PIN_3
#define Temperature_Six_GPIO_Port GPIOA
#define Temperature_Five_Pin GPIO_PIN_4
#define Temperature_Five_GPIO_Port GPIOA
#define Temperature_Four_Pin GPIO_PIN_5
#define Temperature_Four_GPIO_Port GPIOA
#define Temperature_Three_Pin GPIO_PIN_6
#define Temperature_Three_GPIO_Port GPIOA
#define Temperature_Two_Pin GPIO_PIN_7
#define Temperature_Two_GPIO_Port GPIOA
#define Temperature_One_Pin GPIO_PIN_0
#define Temperature_One_GPIO_Port GPIOB
#define CTRL_ONE_Pin GPIO_PIN_1
#define CTRL_ONE_GPIO_Port GPIOB
#define CTRL_TWO_Pin GPIO_PIN_2
#define CTRL_TWO_GPIO_Port GPIOB
#define CTRL_EIGHT_Pin GPIO_PIN_15
#define CTRL_EIGHT_GPIO_Port GPIOA
#define CTRL_THREE_Pin GPIO_PIN_3
#define CTRL_THREE_GPIO_Port GPIOB
#define CTRL_FOUR_Pin GPIO_PIN_4
#define CTRL_FOUR_GPIO_Port GPIOB
#define CTRL_FIVE_Pin GPIO_PIN_5
#define CTRL_FIVE_GPIO_Port GPIOB
#define CTRL_SIX_Pin GPIO_PIN_6
#define CTRL_SIX_GPIO_Port GPIOB
#define CTRL_SEVEN_Pin GPIO_PIN_7
#define CTRL_SEVEN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
typedef struct{
	volatile uint16_t Temperature_Eight_Value;  
	volatile uint16_t Temperature_Seven_Value;  
	volatile uint16_t Temperature_Six_Value;	
	volatile uint16_t Temperature_Five_Value;    
	volatile uint16_t Temperature_Four_Value;    
	volatile uint16_t Temperature_Three_Value;
	volatile uint16_t Temperature_Two_Value;    	
	volatile uint16_t Temperature_One_Value;    
}ADC_Value_t;
extern ADC_Value_t adc_value;
extern volatile uint16_t adc_t_value[8];
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
