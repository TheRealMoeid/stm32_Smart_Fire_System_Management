#include "lcd.h"

void LCD_PulseEnable(void) {
    HAL_GPIO_WritePin(LCD_PORT, LCD_EN, GPIO_PIN_SET);
    HAL_Delay(1);
    HAL_GPIO_WritePin(LCD_PORT, LCD_EN, GPIO_PIN_RESET);
    HAL_Delay(1);
}

void LCD_Send4Bits(uint8_t data) {
    HAL_GPIO_WritePin(LCD_PORT, LCD_D4, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_PORT, LCD_D5, ((data >> 1) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_PORT, LCD_D6, ((data >> 2) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_PORT, LCD_D7, ((data >> 3) & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void LCD_SendCommand(uint8_t cmd) {
    HAL_GPIO_WritePin(LCD_PORT, LCD_RS, GPIO_PIN_RESET);
    LCD_Send4Bits(cmd >> 4); // Send higher nibble
    LCD_PulseEnable();
    LCD_Send4Bits(cmd);      // Send lower nibble
    LCD_PulseEnable();
}

void LCD_SendData(uint8_t data) {
    HAL_GPIO_WritePin(LCD_PORT, LCD_RS, GPIO_PIN_SET);
    LCD_Send4Bits(data >> 4);
    LCD_PulseEnable();
    LCD_Send4Bits(data);
    LCD_PulseEnable();
}

void LCD_Init(void) {
    HAL_Delay(20);
    LCD_SendCommand(0x33);
    LCD_SendCommand(0x32);
    LCD_SendCommand(0x28); // 4-bit mode, 2 lines, 5x8 font
    LCD_SendCommand(0x0C); // Display ON, Cursor OFF
    LCD_SendCommand(0x01); // Clear display
    HAL_Delay(2);
    LCD_SendCommand(0x06); // Increment cursor
}

void LCD_SendString(char *str) {
    while (*str) LCD_SendData(*str++);
}

void LCD_SetCursor(uint8_t row, uint8_t col) {
    uint8_t address = (row == 0) ? 0x80 : 0xC0;
    address += col;
    LCD_SendCommand(address);
}

void LCD_Clear(void) {
    LCD_SendCommand(0x01);
    HAL_Delay(2);
}