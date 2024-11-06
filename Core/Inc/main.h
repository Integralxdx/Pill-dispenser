/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PWD_Pin GPIO_PIN_0
#define PWD_GPIO_Port GPIOA
#define PWD_EXTI_IRQn EXTI0_IRQn
#define LLR_Pin GPIO_PIN_1
#define LLR_GPIO_Port GPIOA
#define BN_RESET_Pin GPIO_PIN_2
#define BN_RESET_GPIO_Port GPIOA
#define LED_LLR_Pin GPIO_PIN_0
#define LED_LLR_GPIO_Port GPIOB
#define LED_WAIT_Pin GPIO_PIN_1
#define LED_WAIT_GPIO_Port GPIOB
#define BN_SEL_Pin GPIO_PIN_12
#define BN_SEL_GPIO_Port GPIOB
#define BN_DN_Pin GPIO_PIN_13
#define BN_DN_GPIO_Port GPIOB
#define BN_UP_Pin GPIO_PIN_14
#define BN_UP_GPIO_Port GPIOB
#define BN_ESC_Pin GPIO_PIN_15
#define BN_ESC_GPIO_Port GPIOB
#define ESP_RST_Pin GPIO_PIN_3
#define ESP_RST_GPIO_Port GPIOB
#define DOOR_Pin GPIO_PIN_4
#define DOOR_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
