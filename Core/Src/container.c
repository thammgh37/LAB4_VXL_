/*
 * container.c
 *
 *  Created on: Nov 26, 2023
 *      Author: ADMIN
 */
#include "container.h"
uint32_t timestamp = 0;
char str[30];
void Blinking_Led_INIT(){
	HAL_GPIO_WritePin(LED1_GPIO_Port,LED1_Pin, 1);
	HAL_GPIO_WritePin(LED2_GPIO_Port,LED2_Pin, 1);
	HAL_GPIO_WritePin(LED3_GPIO_Port,LED3_Pin, 1);
	HAL_GPIO_WritePin(LED4_GPIO_Port,LED4_Pin, 1);
	HAL_GPIO_WritePin(LED5_GPIO_Port,LED5_Pin, 1);
}
void Blinking_Led_500ms(){
	HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
}

void Blinking_Led_1000ms(){
	HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
}

void Blinking_Led_1500ms(){
	HAL_GPIO_TogglePin(LED3_GPIO_Port,LED3_Pin);
}

void Blinking_Led_2000ms(){
	HAL_GPIO_TogglePin(LED4_GPIO_Port,LED4_Pin);
}

void Blinking_Led_2500ms(){
	HAL_GPIO_TogglePin(LED5_GPIO_Port,LED5_Pin);
}

void Print_TimeStamp(){
	timestamp += 10;
	uint8_t lengofstr = sprintf(str,"%d\r\n",timestamp);
	HAL_UART_Transmit(&huart2, str, lengofstr, 50);
}
