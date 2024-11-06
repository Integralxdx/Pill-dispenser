#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void lcd_init (void);   // initialize lcd

void lcd_send_cmd (char cmd);  // send command to the lcd

void lcd_send_data (char data);  // send data to the lcd

void lcd_send_string (char *str);  // send string to the lcd

void lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);

void lcd_clear (void);





void Goto_A_Location(uint8_t x, uint8_t y);

void send_an_int (int integer);

void send_a_string_with_location(char *str, uint8_t x, uint8_t y);

void send_an_integer_with_location(int integer, uint8_t x, uint8_t y);


void send_a_float(float floatNumber);

void send_a_float_with_location(float floatnumber, uint8_t x, uint8_t y);

