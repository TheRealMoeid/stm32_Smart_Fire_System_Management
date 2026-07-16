#ifndef LCD_H_
#define LCD_H_

#include "main.h"

// LCD Pin Definitions mapping to Port B
#define LCD_PORT GPIOB
#define LCD_RS GPIO_PIN_8
#define LCD_EN GPIO_PIN_9
#define LCD_D4 GPIO_PIN_10
#define LCD_D5 GPIO_PIN_11
#define LCD_D6 GPIO_PIN_12
#define LCD_D7 GPIO_PIN_13

void LCD_Init(void);
void LCD_SendCommand(uint8_t cmd);
void LCD_SendData(uint8_t data);
void LCD_SendString(char *str);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Clear(void);

#endif