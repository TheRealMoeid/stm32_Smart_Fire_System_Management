/* USER CODE BEGIN Header */
/**
  * @file           : hmi.c
  * @brief          : Human-Machine Interface Implementation
  */
/* USER CODE END Header */

#include "hmi.h"
#include "lcd.h"
#include <string.h>
#include <stdio.h> 

/* Private Variables ---------------------------------------------------------*/
char current_password[NUMBER_OF_KEYS]; 
char input_buffer[NUMBER_OF_KEYS];
uint8_t input_index = 0;

// Physical mapping of the 4x4 Keypad in Proteus
const char keypad_matrix[4][4] = {
  {'7', '8', '9', 'A'}, // Row 1
  {'4', '5', '6', 'B'}, // Row 2
  {'1', '2', '3', 'C'}, // Row 3
  {'*', '0', '#', 'D'}  // Row 4
};

/* Private Functions ---------------------------------------------------------*/
// Array comparison for security authentication
static bool CheckPassword(void) {
    for (int i=0; i<4; i++) {
        if (input_buffer[i] != current_password[i]) return false;
    }
    return true;
}

/* Public Functions ----------------------------------------------------------*/

// Initialize default password to bypass Proteus RAM startup glitch
void HMI_Init(void) {
    current_password[0] = '1';
    current_password[1] = '2';
    current_password[2] = '3';
    current_password[3] = '4';
}

// Background Keypad Scanner (Called from Timer Interrupt)
void HMI_ScanKeypad(void) {
    static uint8_t scan_row = 0;
    static uint8_t no_key_count = 0;
    static char last_registered_key = '\0';
    char found_key = '\0';
    
    // Check columns for the currently active row
    if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_RESET) found_key = keypad_matrix[scan_row][0];
    else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) == GPIO_PIN_RESET) found_key = keypad_matrix[scan_row][1];
    else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) == GPIO_PIN_RESET) found_key = keypad_matrix[scan_row][2];
    else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) == GPIO_PIN_RESET) found_key = keypad_matrix[scan_row][3];
    
    // Debounce Logic (150ms delay for mechanical bounce resolution)
    if (found_key != '\0') {
        if (found_key != last_registered_key) {
            key_pressed = found_key; // Send to main loop
            last_registered_key = found_key;
        }
        no_key_count = 0;
    } else {
        no_key_count++;
        // Require 150ms of physical release to accept a new button press
        if (no_key_count >= 15) {
            last_registered_key = '\0'; 
            no_key_count = 15; 
        }
    }
    
    // Prepare for next interrupt: Pull all rows HIGH, then specific row LOW
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_SET);
    scan_row++;
    if (scan_row > 3) scan_row = 0;
    uint16_t next_row_pin = (1 << scan_row);
    HAL_GPIO_WritePin(GPIOB, next_row_pin, GPIO_PIN_RESET);
}

// Handle Keypad Menu Navigation and Password Entry
void HMI_HandleKeypadInput(char key) {
    // 1. Universal Escape Key (A)
    if (key == 'A') {
        if (ui_state != SYS_NORMAL && ui_state != SYS_ALARM_FIRE && ui_state != SYS_ALARM_GAS) {
            // Check if we are just exiting the live monitor
            bool was_live_monitor = (ui_state == SYS_SHOW_TEMP || ui_state == SYS_SHOW_GAS);
            
            ui_state = SYS_NORMAL;
            input_index = 0;
            memset(input_buffer, 0, NUMBER_OF_KEYS); 
            
            if (was_live_monitor) {
                // Instant exit without delay for live monitoring
                HMI_UpdateLCD_Menu();
            } else {
                // Standard cancel for password menus
                LCD_Clear(); 
                LCD_SetCursor(0,0); 
                LCD_SendString("CANCELED...     "); 
                HAL_Delay(700);
                HMI_UpdateLCD_Menu();
            }
            
            key_pressed = '\0'; 
            return;
        }
			}

    // 2. Main Menu Routing
    if (ui_state == SYS_NORMAL) {
        if (key == 'B' && !alarm_Gas && !alarm_Fire) { 
            if (manual_gas_closed == 0) ui_state = SYS_PASS_GAS_CLOSE;
            else ui_state = SYS_PASS_GAS_OPEN;
            input_index = 0; memset(input_buffer, 0, NUMBER_OF_KEYS); HMI_UpdateLCD_Menu(); 
        }
        else if (key == 'C' && !alarm_Fire) { 
            if (manual_water_on == 0) ui_state = SYS_PASS_WATER_ON;
            else ui_state = SYS_PASS_WATER_OFF;
            input_index = 0; memset(input_buffer, 0, NUMBER_OF_KEYS); HMI_UpdateLCD_Menu(); 
        }
        else if (key == 'D') { 
            ui_state = SYS_CHANGE_PASS_OLD; 
            input_index = 0; memset(input_buffer, 0, NUMBER_OF_KEYS); HMI_UpdateLCD_Menu(); 
        } 
        else if (key == '*') {
            ui_state = SYS_SHOW_TEMP; 
            HMI_UpdateLCD_Menu();
        }
        else if (key == '#') {
            ui_state = SYS_SHOW_GAS; 
            HMI_UpdateLCD_Menu();
        }
    } 
    // 3. Password Collection & Verification
    else {
        if (key >= '0' && key <= '9' && input_index < 4) {
            
            input_buffer[input_index] = key;
            
            // UI Masking: Hide password with '*'
            LCD_SetCursor(1, 6 + input_index);
            LCD_SendString("*"); 
            input_index++;
            
            // Check password once 4 digits are collected
            if (input_index == 4) {
                HAL_Delay(300); // Visual pause for the user
                
                // Saving new password directly to RAM
                if (ui_state == SYS_CHANGE_PASS_NEW) {
                    for(int i=0; i<4; i++) current_password[i] = input_buffer[i];
                    LCD_Clear(); LCD_SetCursor(0,0); LCD_SendString("NEW PASS SAVED! "); 
                    HAL_Delay(1000);
                    ui_state = SYS_NORMAL;
                } 
                // Verify password for overrides and alarms
                else {
                    if (CheckPassword()) {
                        if (ui_state == SYS_ALARM_FIRE || ui_state == SYS_ALARM_GAS) {
                            alarm_Fire = 0; 
                            alarm_Gas = 0; 
                            ui_state = SYS_NORMAL; 
                        }
                        else if (ui_state == SYS_PASS_GAS_CLOSE) {
                            manual_gas_closed = 1;
                            LCD_Clear(); LCD_SetCursor(0,0); LCD_SendString("GAS VLV: CLOSED "); HAL_Delay(1500);
                            ui_state = SYS_NORMAL;
                        }
                        else if (ui_state == SYS_PASS_GAS_OPEN) {
                            manual_gas_closed = 0;
                            LCD_Clear(); LCD_SetCursor(0,0); LCD_SendString("GAS VLV: OPEN   "); HAL_Delay(1500);
                            ui_state = SYS_NORMAL;
                        }
                        else if (ui_state == SYS_PASS_WATER_ON) {
                            manual_water_on = 1;
                            LCD_Clear(); LCD_SetCursor(0,0); LCD_SendString("WATER PUMP: ON  "); HAL_Delay(1500);
                            ui_state = SYS_NORMAL;
                        }
                        else if (ui_state == SYS_PASS_WATER_OFF) {
                            manual_water_on = 0;
                            LCD_Clear(); LCD_SetCursor(0,0); LCD_SendString("WATER PUMP: OFF "); HAL_Delay(1500);
                            ui_state = SYS_NORMAL;
                        }
                        else if (ui_state == SYS_CHANGE_PASS_OLD) {
                            ui_state = SYS_CHANGE_PASS_NEW; // Authorize step 2
                            LCD_Clear(); LCD_SetCursor(0,0); LCD_SendString("OLD PASS OK!    "); HAL_Delay(1000);
                        }
                    } 
                    else {
                        // Access Denied Logic
                        LCD_Clear(); LCD_SetCursor(0,0); 
                        if (ui_state == SYS_CHANGE_PASS_OLD) LCD_SendString("WRONG OLD PASS! ");
                        else LCD_SendString("WRONG PASS!     "); 
                        HAL_Delay(1000); 
                        
                        if (ui_state != SYS_ALARM_FIRE && ui_state != SYS_ALARM_GAS) {
                            ui_state = SYS_NORMAL;
                        }
                    }
                }
                
                // Universal cleanup after attempting a password
                input_index = 0;
                memset(input_buffer, 0, NUMBER_OF_KEYS); 
                HMI_UpdateLCD_Menu();
                key_pressed = '\0'; 
            }
        }
    }
}

// Render the static parts of the LCD menu
void HMI_UpdateLCD_Menu(void) {
    if (ui_state != SYS_SHOW_TEMP && ui_state != SYS_SHOW_GAS) {
        LCD_Clear();
    }
    LCD_SetCursor(0, 0);
    
    switch (ui_state) {
        case SYS_NORMAL:
            LCD_SendString("SYSTEM: SECURE  "); 
            LCD_SetCursor(1, 0);
            if (manual_gas_closed && manual_water_on) LCD_SendString("OVR: GAS & WATER");
            else if (manual_gas_closed)               LCD_SendString("OVERRIDE: GAS   ");
            else if (manual_water_on)                 LCD_SendString("OVERRIDE: WATER ");
            else                                      LCD_SendString("B:G C:W D:P #:* "); 
            break;
        case SYS_ALARM_FIRE:
            LCD_SendString("!! FIRE ALARM !!");
            LCD_SetCursor(1, 0); LCD_SendString("Code:           ");
            break;
        case SYS_ALARM_GAS:
            LCD_SendString("!! GAS LEAK !!  ");
            LCD_SetCursor(1, 0); LCD_SendString("Code:           ");
            break;
        case SYS_PASS_GAS_CLOSE:
            LCD_SendString("SHUT GAS[A=Cncl]");
            LCD_SetCursor(1, 0); LCD_SendString("Code:           ");
            break;
        case SYS_PASS_GAS_OPEN:
            LCD_SendString("OPEN GAS[A=Cncl]");
            LCD_SetCursor(1, 0); LCD_SendString("Code:           ");
            break;
        case SYS_PASS_WATER_ON:
            LCD_SendString("STRT WTR[A=Cncl]");
            LCD_SetCursor(1, 0); LCD_SendString("Code:           ");
            break;
        case SYS_PASS_WATER_OFF:
            LCD_SendString("STOP WTR[A=Cncl]");
            LCD_SetCursor(1, 0); LCD_SendString("Code:           ");
            break;
        case SYS_CHANGE_PASS_OLD:
            LCD_SendString("Old Pwd [A=Cncl]");
            LCD_SetCursor(1, 0); LCD_SendString("Code:           ");
            break;
        case SYS_CHANGE_PASS_NEW:
            LCD_SendString("New Pwd [A=Cncl]");
            LCD_SetCursor(1, 0); LCD_SendString("Code:           ");
            break;
        case SYS_SHOW_TEMP:
            LCD_SendString("LIVE TEMP       "); 
            LCD_SetCursor(1, 0); LCD_SendString("Press A to Exit "); 
            break;
        case SYS_SHOW_GAS:
            LCD_SendString("LIVE GAS (%)    "); 
            LCD_SetCursor(1, 0); LCD_SendString("Press A to Exit "); 
            break;
    }
}

// Update the dynamic live sensor data on the LCD
void HMI_UpdateLiveMonitoring(uint32_t temp_val, uint32_t gas_val) {
    if (ui_state == SYS_SHOW_TEMP) {
        char buf[32]; // ????? ?????? ????? ???? ??????? ?? ????? ?????
        
        // Proteus specific math correction for 5V LM35 model
        uint32_t temp_c = ((temp_val * 500) + 2047) / 4095;
        
        // %-3lu ensures the number takes exactly 3 spaces.
        // Format length: 3(num) + 1(space) + 1(C) + 1(space) + 8([A=Exit]) + 2(spaces) = 16 characters
        sprintf(buf, "%-3lu C [A=Exit]  ", temp_c); 
        
        LCD_SetCursor(1, 0);
        LCD_SendString(buf);
    }
    else if (ui_state == SYS_SHOW_GAS) {
        char buf[32]; // ????? ?????? ????? ???? ??????? ?? ????? ?????
        
        // Calculate percentage (0 to 100%)
        uint32_t gas_p = ((gas_val * 100) + 2047) / 4095;
        
        // Format length: 3(num) + 1(space) + 1(%) + 1(space) + 8([A=Exit]) + 2(spaces) = 16 characters
        sprintf(buf, "%-3lu %% [A=Exit]  ", gas_p); 
        
        LCD_SetCursor(1, 0);
        LCD_SendString(buf);
    }
}