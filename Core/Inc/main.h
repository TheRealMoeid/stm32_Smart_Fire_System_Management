/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#define SENSOR_TEMP_Pin GPIO_PIN_0
#define SENSOR_TEMP_GPIO_Port GPIOA
#define SENSOR_GAS_Pin GPIO_PIN_1
#define SENSOR_GAS_GPIO_Port GPIOA
#define SERVO_GAS_Pin GPIO_PIN_6
#define SERVO_GAS_GPIO_Port GPIOA
#define A_Pin GPIO_PIN_0
#define A_GPIO_Port GPIOB
#define B_Pin GPIO_PIN_1
#define B_GPIO_Port GPIOB
#define C_Pin GPIO_PIN_2
#define C_GPIO_Port GPIOB
#define D4_Pin GPIO_PIN_10
#define D4_GPIO_Port GPIOB
#define D5_Pin GPIO_PIN_11
#define D5_GPIO_Port GPIOB
#define D6_Pin GPIO_PIN_12
#define D6_GPIO_Port GPIOB
#define D7_Pin GPIO_PIN_13
#define D7_GPIO_Port GPIOB
#define UART_TX_Pin GPIO_PIN_9
#define UART_TX_GPIO_Port GPIOA
#define UART_RX_Pin GPIO_PIN_10
#define UART_RX_GPIO_Port GPIOA
#define MOTOR_WATER_Pin GPIO_PIN_15
#define MOTOR_WATER_GPIO_Port GPIOA
#define D_Pin GPIO_PIN_3
#define D_GPIO_Port GPIOB
#define C1_Pin GPIO_PIN_4
#define C1_GPIO_Port GPIOB
#define C2_Pin GPIO_PIN_5
#define C2_GPIO_Port GPIOB
#define C3_Pin GPIO_PIN_6
#define C3_GPIO_Port GPIOB
#define C4_Pin GPIO_PIN_7
#define C4_GPIO_Port GPIOB
#define RS_Pin GPIO_PIN_8
#define RS_GPIO_Port GPIOB
#define E_Pin GPIO_PIN_9
#define E_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
