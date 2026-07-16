/* USER CODE BEGIN Header */
/**
  * @file           : hmi.h
  * @brief          : Header for Human-Machine Interface module
  */
/* USER CODE END Header */

#ifndef HMI_H
#define HMI_H

#include "main.h"
#include <stdbool.h>

/* Defines -------------------------------------------------------------------*/
#define NUMBER_OF_KEYS 4

/* Enums ---------------------------------------------------------------------*/
// System State Machine Enumerations
typedef enum { 
    SYS_NORMAL,           
    SYS_ALARM_FIRE,       
    SYS_ALARM_GAS,        
    SYS_PASS_GAS_CLOSE,   
    SYS_PASS_GAS_OPEN,    
    SYS_PASS_WATER_ON,    
    SYS_PASS_WATER_OFF,   
    SYS_CHANGE_PASS_OLD,  
    SYS_CHANGE_PASS_NEW,  
    SYS_SHOW_TEMP,        
    SYS_SHOW_GAS          
} UI_State;

/* Extern Variables (Shared with main.c) -------------------------------------*/
extern UI_State ui_state;
extern uint8_t alarm_Fire;
extern uint8_t alarm_Gas;
extern uint8_t manual_water_on;   
extern uint8_t manual_gas_closed; 
extern volatile char key_pressed;

/* Function Prototypes -------------------------------------------------------*/
void HMI_Init(void);
void HMI_ScanKeypad(void);
void HMI_HandleKeypadInput(char key);
void HMI_UpdateLCD_Menu(void);
void HMI_UpdateLiveMonitoring(uint32_t temp_val, uint32_t gas_val);

#endif /* HMI_H */