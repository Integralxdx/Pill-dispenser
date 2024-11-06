
/** Put this in the src folder **/

#include "i2c-lcd.h"
extern I2C_HandleTypeDef hi2c1;  // change your handler here accordingly

#define SLAVE_ADDRESS_LCD 0x4E // change this according to ur setup

char FirstColumePositionForLCD [4] = {0,64,20,84};

void lcd_send_cmd (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0 -> bxxxx1100
	data_t[1] = data_u|0x08;  //en=0, rs=0 -> bxxxx1000
	data_t[2] = data_l|0x0C;  //en=1, rs=0 -> bxxxx1100
	data_t[3] = data_l|0x08;  //en=0, rs=0 -> bxxxx1000
	HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

void lcd_send_data (char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0 -> bxxxx1101
	data_t[1] = data_u|0x09;  //en=0, rs=0 -> bxxxx1001
	data_t[2] = data_l|0x0D;  //en=1, rs=0 -> bxxxx1101
	data_t[3] = data_l|0x09;  //en=0, rs=0 -> bxxxx1001
	HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
}

void lcd_clear (void)
{
	lcd_send_cmd (0x80);
	for (int i=0; i<70; i++)
	{
		lcd_send_data (' ');
	}
}

void lcd_put_cur(int row, int col)
{
    switch (row)
    {
        case 0:
            col |= 0x80;
            break;
        case 1:
            col |= 0xC0;
            break;
    }

    lcd_send_cmd (col);
}


void lcd_init (void)
{
	// 4 bit initialisation
	HAL_Delay(50);  // wait for >40ms
	lcd_send_cmd (0x30);
	HAL_Delay(50); // wait for >4.1ms
	lcd_send_cmd (0x30);
	HAL_Delay(50); // wait for >100us
	lcd_send_cmd (0x30);
	HAL_Delay(50);
	lcd_send_cmd (0x20);  // 4bit mode
	HAL_Delay(50);

  // dislay initialisation
	lcd_send_cmd (0x28); // Function set --> DL=0 (4 bit mode), N = 1 (2 line display) F = 0 (5x8 characters)
	HAL_Delay(50);
	lcd_send_cmd (0x08); //Display on/off control --> D=0,C=0, B=0  ---> display off
	HAL_Delay(50);
	lcd_send_cmd (0x01);  // clear display
	HAL_Delay(50);
	lcd_send_cmd (0x06); //Entry mode set --> I/D = 1 (increment cursor) & S = 0 (no shift)
	HAL_Delay(50);
	lcd_send_cmd (0x0C); //Display on/off control --> D = 1, C and B = 0. (Cursor and blink, last two bits)
	HAL_Delay(200);
	send_a_string_with_location("osdematrix", 1 ,1);
}

void lcd_send_string (char *str)
{
	while (*str) lcd_send_data (*str++);
}


void Goto_A_Location(uint8_t x, uint8_t y)
{
	lcd_send_cmd(0x80+FirstColumePositionForLCD[y-1]+(x-1));
}

void send_an_int (int integer)
{
char str[10];

  sprintf(str, "%d", integer);
  lcd_send_string (str);
} 


void send_a_string_with_location(char *str, uint8_t x, uint8_t y){
	Goto_A_Location(x, y);
	lcd_send_string(str);
}
void send_an_integer_with_location(int integer, uint8_t x, uint8_t y){
	Goto_A_Location(x, y);
	send_an_int(integer);
}

 void send_a_float(float floatNumber){
	char str[10]; // Assuming a buffer size of 20 for the string representation
	sprintf(str, "%f", floatNumber);
	lcd_send_string(str);
}
void send_a_float_with_location(float floatnumber, uint8_t x, uint8_t y){
	Goto_A_Location(x, y);
	send_a_float(floatnumber);
}
