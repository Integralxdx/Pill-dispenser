/*
 * ESP8266.h
 *
 *  Created on: Nov 17, 2021
 *      Author: vikto
 */

#ifndef INC_ESP8266_H_
#define INC_ESP8266_H_

#include "main.h"
#include "i2c-lcd.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WiFi_Credentials	"AT+CWSAP=\"Pill_Dispencer\",\"0123456789\",1,4\r\n"
#define maxnumberofentries  10  // Max number of users
#define door_delay_time 1000
#define stepsperrev 4096
#define TIM_FREQ 840000000

extern UART_HandleTypeDef huart1;

//extern uint32_t seconds;
extern uint8_t buffer[10000];
extern uint16_t buffer_index, timeout, messageHandlerFlag;


extern int usernumber;
extern int flag4door;

extern char mpillA[3];
extern char mpillB[3];
extern char mpillC[3];
extern char mpillD[3];
extern int rpthrs;

extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern TIM_HandleTypeDef htim5;

extern char ATcommand[80];


extern int flag4door;
extern int alarmflag;

typedef struct
{
	char datetime[21];
	char pillA[4];
	char pillB[4];
	char pillC[4];
	char pillD[4];
}DataEntry;

void GetDataFromBuffedate (char *buf);
void get_time_date(void);
void ESP_RESET();
void ESP_Server_Init(char *STAIP);
//void ESP_Server_Init();
void ESP_Clear_Buffer();
uint8_t string_compare(char array1[], char array2[], uint16_t length);
int string_contains(char bufferArray[], char searchedString[], uint16_t length);
void messageHandler();
void sendData(uint8_t data_nun);
void set_mytime_date (uint8_t year, uint8_t month, uint8_t date,uint8_t weekday,uint8_t hr, uint8_t min, uint8_t sec);//monday is 1
void set_myalarm (uint8_t hr, uint8_t min, uint8_t sec,uint8_t weekday);
void GetDataFromBufferalam (char *buf);
void Dataandexecution (void);
int get_timehrs(void);
void get_time_date(void);
void set_mytime_date (uint8_t year, uint8_t month, uint8_t date, uint8_t weekday, uint8_t hr, uint8_t min, uint8_t sec);
void set_myalarm (uint8_t hr, uint8_t min, uint8_t sec,uint8_t weekday);
char* convertToString(char* a, int size);
void get_time_stamp(void);
void Buzz_ring (void);
void Buzz_stop(void);
int presForFrequency (int frequency);
void delay (uint16_t us);
void stepper_set_rpm (int rpm);
void stepper_stop (void);
void stepper_half_drive (int step);
void stepper_step_angle (float angle, int direction, int rpm);
void drop_pill (uint8_t pill_no);
void reset_tray(void);


#endif /* INC_ESP8266_H_ */
