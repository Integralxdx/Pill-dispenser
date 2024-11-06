

#include "ESP8266.h"

uint32_t seconds = 0;
uint8_t buffer[10000];
uint16_t buffer_index = 0, timeout = 0, messageHandlerFlag = 0;
uint8_t data_nun=0;

DataEntry entry[maxnumberofentries];


uint8_t rstate =0;
uint8_t count =0;
char mpillA[3]={0};
char mpillB[3]={0};
char mpillC[3]={0};
char mpillD[3]={0};
int rpthrs;
int usernumber;
int flag4door;



int sizeofentry (DataEntry *entry)
{
	int size=0;
	while (entry[size].datetime[0] != '\0') size++;
	return size+1;
}

int get_timehrs(void)
{
	char mtime[3];
	RTC_TimeTypeDef gTime;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  sprintf((char*)mtime,"%02d",gTime.Hours);
  int num=0;
  sscanf(mtime, "%02d", &num);
  return num;
}

void get_time_date(void)
{
	char time[15];
	char date[15];
  RTC_DateTypeDef gDate;
  RTC_TimeTypeDef gTime;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);


  /* Display date Format: dd-mm-yyyy */
  sprintf((char*)time,"%02d:%02d:%02d",gTime.Hours, gTime.Minutes, gTime.Seconds);
  sprintf((char*)date,"%02d-%02d-%2d",gDate.Date, gDate.Month, gDate.Year);
  lcd_clear();
  send_a_string_with_location(time, 1, 2);
  send_a_string_with_location(date, 1, 1);
}

void set_mytime_date (uint8_t year, uint8_t month, uint8_t date, uint8_t weekday, uint8_t hr, uint8_t min, uint8_t sec)
{
	HAL_TIM_Base_Stop_IT(&htim3);
	RTC_TimeTypeDef sTime = {0};

	sTime.Hours = hr;
	sTime.Minutes = min;
	sTime.Seconds = sec;
	sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sTime.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}

	RTC_DateTypeDef sDate = {0};
	sDate.WeekDay = weekday;
	sDate.Month = month;
	sDate.Date = date;
	sDate.Year = year;
	if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x2345);  // backup register
	HAL_TIM_Base_Start_IT(&htim3);
}


void set_myalarm (uint8_t hr, uint8_t min, uint8_t sec,uint8_t weekday)
{
	RTC_AlarmTypeDef sAlarm = {0};
	sAlarm.AlarmTime.Hours = hr;
	sAlarm.AlarmTime.Minutes = min;
	sAlarm.AlarmTime.Seconds = sec;
	sAlarm.AlarmTime.SubSeconds = 0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = weekday;
	sAlarm.Alarm = RTC_ALARM_A;
	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
	{
		Error_Handler();
	}
}
char* convertToString(char* a, int size)
{
    int i;
    char* s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}

void get_time_stamp(void)
{
	char time[20];
  RTC_DateTypeDef gDate;
  RTC_TimeTypeDef gTime;

  /* Get the RTC current Time */
  HAL_RTC_GetTime(&hrtc, &gTime, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&hrtc, &gDate, RTC_FORMAT_BIN);
  lcd_clear();
  /* Display time Format: hh:mm:ss */
  sprintf((char*)time,"%02d-%02d-%02d/%02d:%02d:%02d",gDate.Date, gDate.Month, 2000 + gDate.Year,gTime.Hours, gTime.Minutes, gTime.Seconds);
 // char *time1=convertToString(time, 17);
  //return time1;
}



void Buzz_ring (void){
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
}
void Buzz_stop(void){
	HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
}
int presForFrequency (int frequency)
{
	if (frequency == 0) return 0;
	return ((TIM_FREQ/(1000*frequency))-1);  // 1 is added in the register
}

void delay (uint16_t us)
{
	__HAL_TIM_SET_COUNTER(&htim2, 0);
	while (__HAL_TIM_GET_COUNTER(&htim2) < us);
}

void stepper_set_rpm (int rpm)  // Set rpm--> max 13, min 1,,,  went to 14 rev/min
{
	delay(84000000/stepsperrev/rpm);
}
void stepper_stop (void){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);   // IN1
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);   // IN2
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);   // IN3
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);   // IN4
}
void stepper_half_drive (int step)
{
	switch (step){
		case 0:
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // IN1
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);   // IN2
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);   // IN3
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);   // IN4
			  break;

		case 1:
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // IN1
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // IN2
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);   // IN3
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);   // IN4
			  break;

		case 2:
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);   // IN1
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // IN2
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);   // IN3
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);   // IN4
			  break;

		case 3:
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);   // IN1
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);   // IN2
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);   // IN3
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);   // IN4
			  break;

		case 4:
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);   // IN1
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);   // IN2
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);   // IN3
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);   // IN4
			  break;

		case 5:
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);   // IN1
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);   // IN2
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);   // IN3
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);   // IN4
			  break;

		case 6:
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);   // IN1
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);   // IN2
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);   // IN3
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);   // IN4
			  break;

		case 7:
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);   // IN1
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);   // IN2
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);   // IN3
			  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);   // IN4
			  break;
		}
}

void stepper_step_angle (float angle, int direction, int rpm)
{
	send_a_string_with_location("changing tray", 1, 2);
	float anglepersequence = 0.703125;  // 360 = 512 sequences
	int numberofsequences = (int) (angle/anglepersequence);

	for (int seq=0; seq<numberofsequences; seq++)
	{
		if (direction == 0)  // for clockwise
		{
			for (int step=7; step>=0; step--)
			{
				stepper_half_drive(step);
				stepper_set_rpm(rpm);
			}

		}

		else if (direction == 1)  // for anti-clockwise
		{
			for (int step=0; step<8; step++)
			{
				stepper_half_drive(step);
				stepper_set_rpm(rpm);
			}
		}
	}
	stepper_stop();
}

void drop_pill (uint8_t pill_no){
	uint8_t pill_count=0;
	while(pill_no>0){
		lcd_clear();
		send_a_string_with_location("Dropping pill", 1, 1);
		HAL_GPIO_WritePin(LED_LLR_GPIO_Port, LED_LLR_Pin, GPIO_PIN_SET);
		HAL_Delay(200);
		flag4door=0;
		HAL_GPIO_WritePin(DOOR_GPIO_Port, DOOR_Pin, GPIO_PIN_SET);
		HAL_Delay(door_delay_time);
		HAL_GPIO_WritePin(DOOR_GPIO_Port, DOOR_Pin, GPIO_PIN_RESET);
		pill_count++;
		pill_no--;
		HAL_GPIO_WritePin(LED_LLR_GPIO_Port, LED_LLR_Pin, GPIO_PIN_RESET);
		lcd_clear();
		send_a_string_with_location("Pill dropped", 1, 1);
		send_an_integer_with_location(pill_count, 1, 2);
	}
}

void reset_tray(void){
	uint8_t rtray =1;
	while (rtray!=1){
		stepper_step_angle(90, 1, 12);
		if(rstate==1){
			rtray =0;
			rstate=0;
		}
	}
}



void doorflag (void){
	if ((mpillA[0]>=1)||(mpillA[1]>=1)){
		flag4door=1;
	}
	else if ((mpillB[0]>=1)||(mpillB[1]>=1)){
		flag4door=2;
	}
	else if ((mpillC[0]>=1)||(mpillC[1]>=1)){
		flag4door=3;
	}
	else if ((mpillD[0]>=1)||(mpillD[1]>=1)){
		flag4door=4;
	}
	else{
		flag4door=0;
	}
}

void Dataandexecution (void){
		//char *t=
		char time[20];
		get_time_stamp();
		//memcpy((entry[usernumber].datetime), t, strlen(t));
		sprintf((entry[usernumber].datetime), "%s", time);
		sprintf((entry[usernumber].pillA), "%s", mpillA);
		sprintf((entry[usernumber].pillB), "%s", mpillB);
		sprintf(entry[usernumber].pillC,"%s",mpillC);
		sprintf((entry[usernumber].pillD), "%s", mpillD);
		//entry[usernumber].datetime="dgrsv";
		if(flag4door==1){
			HAL_TIM_Base_Stop_IT(&htim3);
			//entry[usernumber].pillA=mpillA;
			//memcpy((entry[usernumber].pillA), mpillA, strlen(mpillA));

			if (HAL_GPIO_ReadPin(BN_RESET_GPIO_Port, BN_RESET_Pin)==1){
				reset_tray();
			}
			drop_pill(atoi(mpillA));
			if (mpillB[0]>=1){
				flag4door=2;
			}
			else if (mpillC[0]>=1){
				flag4door=3;
			}
			else if (mpillD[0]>=1){
				flag4door=4;
			}
			else{
				flag4door=0;
			}
			HAL_TIM_Base_Start_IT(&htim3);
		}
		else if(flag4door==2){
			//  entry[usernumber].pillB=mpillB;

			HAL_TIM_Base_Stop_IT(&htim3);
			if (HAL_GPIO_ReadPin(BN_RESET_GPIO_Port, BN_RESET_Pin)==1){
				reset_tray();
			}
			stepper_step_angle(90, 1, 12);
			drop_pill(atoi(mpillB));
			if (mpillA[0]>=1){
				flag4door=1;
			}
			else if (mpillC[0]>=1){
				flag4door=3;
			}
			else if (mpillD[0]>=1){
				flag4door=4;
			}
			else{
				flag4door=0;
			}
			HAL_TIM_Base_Start_IT(&htim3);
		}
		else if(flag4door==3){
			//  entry[usernumber].pillC=mpillC;


			HAL_TIM_Base_Stop_IT(&htim3);
			if (HAL_GPIO_ReadPin(BN_RESET_GPIO_Port, BN_RESET_Pin)==1){
				reset_tray();
			}
			stepper_step_angle(180, 1, 12);
			drop_pill(atoi(mpillC));
			if (mpillA[0]>=1){
				flag4door=1;
			}
			else if (mpillB[0]>=1){
				flag4door=2;
			}
			else if (mpillD[0]>=1){
				flag4door=4;
			}
			else{
				flag4door=0;
			}
			HAL_TIM_Base_Start_IT(&htim3);
		}
		else if(flag4door==4){
			//  entry[usernumber].pillD=mpillD;

			HAL_TIM_Base_Stop_IT(&htim3);
			if (HAL_GPIO_ReadPin(BN_RESET_GPIO_Port, BN_RESET_Pin)==1){
				reset_tray();
			}
			stepper_step_angle(270, 1, 12);
			drop_pill(atoi(mpillD));
			if (mpillA[0]>=1){
				flag4door=1;
			}
			else if (mpillB[0]>=1){
				flag4door=2;
			}
			else if (mpillC[0]>=1){
				flag4door=3;
			}
			else{
				flag4door=0;
			}
			HAL_TIM_Base_Start_IT(&htim3);
		}
		usernumber++;
		Buzz_stop();
}


void ESP_Server_Init(char *STAIP)
{
	char data[80];

	//ESP_RESET();
	HAL_Delay(200);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+RST\r\n", strlen("AT+RST\r\n"), 100);
	HAL_Delay(500);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT\r\n", strlen("AT\r\n"), 100);
	HAL_Delay(500);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CWMODE=2\r\n", strlen("AT+CWMODE=2\r\n"), 100);
	HAL_Delay(500);
	ESP_Clear_Buffer();

	sprintf (data, "AT+CIPSTA=\"%s\"\r\n", STAIP);
	HAL_UART_Transmit(&huart1, (uint8_t*)data, strlen(data), 100);
	HAL_Delay(500);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)WiFi_Credentials, strlen(WiFi_Credentials), 100);
	HAL_Delay(500);
	ESP_Clear_Buffer();



	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CWDHCP=1,1\r\n", strlen("AT+CWDHCP=1,1\r\n"), 100);
	HAL_Delay(500);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPMUX=1\r\n", strlen("AT+CIPMUX=1\r\n"), 100);
	HAL_Delay(500);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPSERVER=1,80\r\n", strlen("AT+CIPSERVER=1,80\r\n"), 100);
	HAL_Delay(500);
	ESP_Clear_Buffer();

	//HAL_UART_Transmit(&huart1, (uint8_t*)WiFi_Credentials, strlen(WiFi_Credentials), 100);
}

void ESP_Clear_Buffer()
{
	memset(buffer, 0, 4000);
	buffer_index = 0;
}

uint8_t string_compare(char array1[], char array2[], uint16_t length)
{
	 uint16_t comVAR=0, i;
	 for(i=0;i<length;i++)
	   	{
	   		  if(array1[i]==array2[i])
	   	  		  comVAR++;
	   	  	  else comVAR=0;
	   	}
	 if (comVAR==length)
		 	return 1;
	 else 	return 0;
}

int string_contains(char bufferArray[], char searchedString[], uint16_t length)
{
	uint8_t result=0;
	for(uint16_t i=0; i<length; i++)
	{
		result = string_compare(&bufferArray[i], &searchedString[0], strlen(searchedString));
		if(result == 1)
			return i;
	}
	return -1;
}

void messageHandler()
{
	__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
	int position = 0;
	if((position = string_contains((char*)buffer, "GET", buffer_index)) != -1)
	{
		position = 0;
		if((position = string_contains((char*)buffer,"/data", buffer_index)) != -1)
		{
			sendData(1);
		}
		else if ((position = string_contains((char*)buffer,"/Home", buffer_index)) != -1)
		{
			int position = 0;
			if((position = string_contains((char*)buffer,"?yy=", buffer_index)) != -1){

				GetDataFromBuffedate((char*)buffer);
			}
			sendData(2);
		}
		else if ((position = string_contains((char*)buffer, "/alarmA", buffer_index)) != -1)
		{
			int position = 0;
			if((position = string_contains((char*)buffer,"/alarmA?Hh", buffer_index)) != -1){
				GetDataFromBufferalam((char*)buffer);
			}

			sendData(3);
		}
		else
		{
			ESP_Clear_Buffer();
			sendData(2);
		}
	}
	else if(string_contains((char*)buffer, "CWJAP", buffer_index) != -1
			&& (string_contains((char*)buffer, "FAIL", buffer_index) != -1
			|| string_contains((char*)buffer, "DISCONNECT", buffer_index) != -1)){
		HAL_UART_Transmit(&huart1, (uint8_t*)WiFi_Credentials, strlen(WiFi_Credentials), 100);
	}
	ESP_Clear_Buffer();
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}



char *alarmA = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>PILL DISPENCER</h1>\n\
		<nav style=\"text-decoration: none; color: black;\">\n\
		<a href=\"/Home\">Home</a> |\n\
		<a href=\"/alarmA\">Set Alarm A</a> |\n\
		<a href=\"/data\">Data</a>\n\
		</nav>\n\
		<h2>PLEASE SET THE ALARM</h2>\n\
		<p>Enter the Details in the form below: </p>\n\
		<form action=\"/alarmA\">\n\
		<label for=\"hour\">HOUR:</label><br>\n\
		<input type=\"number\" id=\"Hh\" name=\"Hh\"  min=\"0\" max=\"12\" step=\"1\" value=\"0\"><br><br>\n\
		<label for=\"minute\">MINUTE:</label><br>\n\
		<input type=\"number\" id=\"Mm\" name=\"Mm\"  min=\"0\" max=\"60\" step=\"1\" value=\"0\"><br><br>\n\
		<label for=\"seconds\">SECONDS:</label><br>\n\
		<input type=\"number\" id=\"Ss\" name=\"Ss\"  min=\"0\" max=\"60\" step=\"1\" value=\"0\"><br><br>\n\
		<label for=\"wday\">WEEKDAY(MONDAY IS 1):</label><br>\n\
		<input type=\"number\" id=\"wd\" name=\"wd\" min=\"01\" max=\"7\" step=\"1\" value=\"1\"><br><br>\n\
		<label for=\"pillA\">Input PillA:</label><br>\n\
		<input type=\"number\" id=\"PA\" name=\"PA\"  min=\"0\" max=\"60\" step=\"1\" value=\"0\"><br><br>\n\
		<label for=\"pillB\">Input PillB:</label><br>\n\
		<input type=\"number\" id=\"PB\" name=\"PB\"  min=\"0\" max=\"60\" step=\"1\" value=\"0\"><br><br>\n\
		<label for=\"pillB\">Input PillC:</label><br>\n\
		<input type=\"number\" id=\"PC\" name=\"PC\"  min=\"0\" max=\"60\" step=\"1\" value=\"0\"><br><br>\n\
		<label for=\"pillD\">Input PillD:</label><br>\n\
		<input type=\"number\" id=\"PD\" name=\"PD\"  min=\"0\" max=\"60\" step=\"1\" value=\"0\"><br><br>\n\
		<label for=\"Repeat\">Repeat Hours:</label><br>\n\
		<input type=\"number\" id=\"RT\" name=\"RT\"  min=\"0\" max=\"24\" step=\"1\" value=\"0\"><br><br>\n\
		<input type=\"submit\" value=\"Submit\">\n\
		</body></html>";

char *Home= "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>PILL DISPENSER</h1>\n\
		<nav style=\"text-decoration: none; color: black;\">\n\
		<a href=\"/Home\">Home</a> |\n\
		<a href=\"/alarmA\">Set Alarm A</a> |\n\
		<a href=\"/data\">Data</a>\n\
		</nav>\n\
		<h1>PLEASE SET DATE</h1>\n\
		<p>Enter the Details in the form below: </p>\n\
		<form action=\"/Home\">\n\
		<label for=\"year\">YEAR:</label><br>\n\
		<input type=\"number\" id=\"yy\" name=\"yy\" min=\"2024\" max=\"2099\" step=\"1\" value=\"2024\"><br><br>\n\
		<label for=\"month\">MONTH:</label><br>\n\
		<input type=\"number\" id=\"mm\" name=\"mm\" min=\"01\" max=\"12\" step=\"1\" value=\"1\"><br><br>\n\
		<label for=\"day\">DAY:</label><br>\n\
		<input type=\"number\" id=\"dd\" name=\"dd\" min=\"01\" max=\"31\" step=\"1\" value=\"1\"><br><br>\n\
		<label for=\"wday\">WEEKDAY(MONDAY IS 1):</label><br>\n\
		<input type=\"number\" id=\"wd\" name=\"wd\" min=\"01\" max=\"7\" step=\"1\" value=\"1\"><br><br>\n\
		<label for=\"hour\">HOUR:</label><br>\n\
		<input type=\"number\" id=\"Hh\" name=\"Hh\" min=\"0\" max=\"12\" step=\"1\" value=\"0\"><br><br>\n\
		<label for=\"minute\">MINUTE:</label><br>\n\
		<input type=\"number\" id=\"Mm\" name=\"Mm\"  min=\"0\" max=\"60\" step=\"1\" value=\"0\"><br><br>\n\
		<label for=\"seconds\">SECONDS:</label><br>\n\
		<input type=\"number\" id=\"Ss\" name=\"Ss\"  min=\"0\" max=\"60\" step=\"1\" value=\"0\"><br><br>\n\
		<input type=\"submit\" value=\"Submit\">\n\
		</form><br><br>\n\
		</body></html>";


char *page2_Top = "<!DOCTYPE html>\n\
		<html>\n\
		<body>\n\
		<h1>PILL DISPENSER</h1>\n\
		<nav style=\"text-decoration: none; color: black;\">\n\
		<a href=\"/Home\">Home</a> |\n\
		<a href=\"/alarmA\">Set Alarm A</a> |\n\
		<a href=\"/data\">Data</a>\n\
		</nav>\n\
		<h2> DATA COLLECTED IS SHOWN BELOW </h2>\n";

char *page2_end = "  </body></html>";

char *table = "<style>table {font-family: arial, sans-serif;\
		border-collapse: collapse;  width: 50%;}\
		td, th {  border: 1px solid #dddddd;\
		text-align: left;  padding: 8px;}tr:nth-child(even)\
		{background-color: #dddddd;}</style><table><tr><th>SN</th><th>DATE AND TIME OF DISPENSE</th><th>PILL DISPENCED IN A</th><th>PILL DISPENCED IN B</th><th>PILL DISPENCED IN C</th><th>PILL DISPENCED IN D</th></tr>";


void sendData(uint8_t data_nun)//sends data compatible with a browser
{

	char outputString[10000], cipsend[50], response[12000];
	memset(outputString, 0, 10000);
	memset(cipsend, 0, 50);
	memset(response, 0, 12000);
	if (data_nun==1){
		char localbuf[5000];
		sprintf(outputString, page2_Top);
		strcat (outputString, table);
		int bufsize = (sizeofentry(entry));
		int k=0;
		for (int i=0; i<bufsize; i++)
		{
			k=i+1;
			sprintf (localbuf, "<tr><td>%d</td> <td>%s</td> <td>%s</td> <td>%s</td> <td>%s</td> <td>%s</td></tr>",k,entry[i].datetime,entry[i].pillA,entry[i].pillB,entry[i].pillC,entry[i].pillD);
			strcat (outputString, localbuf);
		}
		strcat (outputString, "</table>");
		strcat(outputString, page2_end);
	}
	if(data_nun==2){
		sprintf(outputString, Home);
	}
	if(data_nun==3){
		sprintf(outputString, alarmA);
	}
	sprintf(response, "HTTP/1.1 200 OK\r\nContent-Length: %i\r\nContent-Type: text/html\r\n\r\n%s", strlen(outputString), outputString);
	sprintf(cipsend, "AT+CIPSEND=0,%i\r\n", strlen(response));

	HAL_UART_Transmit(&huart1, (uint8_t*)cipsend, strlen(cipsend), 5000);
	HAL_Delay(500);
	HAL_UART_Transmit(&huart1, (uint8_t*)response, strlen(response), 5000);
	HAL_Delay(500);
	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPCLOSE=0\r\n", strlen("AT+CIPCLOSE=0\r\n"), 100);
}


void array_to_string (char arr[],char *str, int nonofchar)
{

	for (int i = 0; i < nonofchar; i++) {
	    sprintf(str + strlen(str), "%d ", arr[i]);
	}
}


void GetDataFromBuffedate (char *buf)
{

    const char outer_delimiters[] = "?";

    char* token;
    char* outer_saveptr = NULL;
    char *array[3];
    int i = 0;

    token = strtok_r(buf, outer_delimiters, &outer_saveptr);

    while (token != NULL) {
        array[i++] = token;
        token = strtok_r(NULL, outer_delimiters, &outer_saveptr);
    }
    //printf("%s\n", array[1]);
    char *p = strtok (array[1], " ");
   // printf("%s\n", p);

    const char final_delimiters[] = "=&";
    char* token1;
    char* final_saveptr = NULL;
    char *array1[100];
    i = 0;
    token1 = strtok_r(p, final_delimiters, &final_saveptr);

    while (token1 != NULL) {
        array1[i++] = token1;
       // printf("%s\n", token1);
        token1 = strtok_r(NULL, final_delimiters, &final_saveptr);
    }

    int num=0;
    sscanf(array1[1], "%d", &num);
    set_mytime_date(num, atoi(array1[3]), atoi(array1[5]), atoi(array1[7]), atoi(array1[9]), atoi(array1[11]), atoi(array1[13]));
    lcd_clear();
    send_a_string_with_location("date and time", 1, 1);
    send_a_string_with_location("has changed ", 1, 2);

    sendData(2);
    HAL_Delay(1500);

    ESP_Clear_Buffer();
}

void GetDataFromBufferalam (char *buf)
{
    const char outer_delimiters[] = "?";

    char* token;
    char* outer_saveptr = NULL;
    char *array[3];
    int i = 0;

    token = strtok_r(buf, outer_delimiters, &outer_saveptr);

    while (token != NULL) {
        array[i++] = token;
        token = strtok_r(NULL, outer_delimiters, &outer_saveptr);
    }
    //printf("%s\n", array[1]);
    char *p = strtok (array[1], " ");
   // printf("%s\n", p);

    const char final_delimiters[] = "=&";
    char* token1;
    char* final_saveptr = NULL;
    char *array1[100];
    i = 0;
    token1 = strtok_r(p, final_delimiters, &final_saveptr);

    while (token1 != NULL) {
        array1[i++] = token1;
        token1 = strtok_r(NULL, final_delimiters, &final_saveptr);
    }
   // sscanf(array1[1], "%d", &num);
    set_myalarm( atoi(array1[1]), atoi(array1[3]), atoi(array1[5]), atoi(array1[7]));
    memcpy(mpillA, array1[9],1);
    //mpillA[0]=atoi(array1[9]);
    memcpy(mpillB, array1[11],1);
   // mpillB=atoi(array1[11]);
    memcpy(mpillC, array1[13],1);
   // mpillC=atoi(array1[13]);
    memcpy(mpillD, array1[15],1);
    //mpillD=atoi(array1[15]);
    rpthrs=atoi(array1[17]);
    lcd_clear();
    send_a_string_with_location("Alarm time", 1, 1);
    send_a_string_with_location("has changed ", 1, 2);
    sendData(3);
    HAL_Delay(3000);
    lcd_clear();
    ESP_Clear_Buffer();
}
