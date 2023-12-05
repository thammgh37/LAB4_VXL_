/*
 * app_scheduler.c
 *
 *  Created on: Nov 27, 2019
 *      Author: VAIO
 */
#include "main.h"

#ifdef SCHE_O1
//#include "Arduino.h"
#include "scheduler.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INTERRUPT_CYCLE			10 // 10 milliseconds
#define PRESCALER				64
#define COUNTER_START 			65536 - INTERRUPT_CYCLE * 1000 * 16 / 64	// 16M is Core Clock

typedef struct{
	void (*pTask)(void);
	uint8_t RunMe;
	uint32_t Period;
	uint32_t Delay;
	uint32_t TaskID;
}sTask;

static sTask SCH_tasks_G[SCH_MAX_TASKS];
//static uint8_t array_Of_Task_ID[SCH_MAX_TASKS];
static uint32_t newTaskID = 0;
//static uint32_t rearQueue = 0;
static uint32_t count_SCH_Update = 0;

void SCH_Init(void){
	HAL_TIM_Base_Start_IT(&htim2);
}
void SCH_Update(void){
	count_SCH_Update ++;
	if (SCH_tasks_G[0].pTask && SCH_tasks_G[0].RunMe == 0){
		if ( SCH_tasks_G[0].Delay >0){
			SCH_tasks_G[0].Delay--;
		}
		if (SCH_tasks_G[0].Delay == 0){
			SCH_tasks_G[0].RunMe = 1;
		}
	}
}
uint32_t SCH_Add_Task(void (* pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	uint8_t newTaskIndex = 0;
	uint32_t sumDelay = 0;
	uint32_t newDelay = 0;
	for (newTaskIndex = 0; newTaskIndex < SCH_MAX_TASKS ; newTaskIndex++){
		sumDelay += SCH_tasks_G[newTaskIndex].Delay;
		if (sumDelay < DELAY){
			newDelay = DELAY - ( sumDelay - SCH_tasks_G[newTaskIndex].Delay);
			SCH_tasks_G[newTaskIndex].Delay = sumDelay - DELAY;
			for (uint8_t i = SCH_MAX_TASKS; i > newTaskIndex ; i--){
				SCH_tasks_G[i].Delay = SCH_tasks_G[i-1].Delay;
				SCH_tasks_G[i].Period = SCH_tasks_G[i-1].Period;
				SCH_tasks_G[i].TaskID = SCH_tasks_G[i-1].TaskID;
				SCH_tasks_G[i].pTask = SCH_tasks_G[i-1].pTask;
			}
			SCH_tasks_G[newTaskIndex].Period = PERIOD;
			SCH_tasks_G[newTaskIndex].TaskID = Get_New_Task_ID();
			SCH_tasks_G[newTaskIndex].pTask = pFunction;
			SCH_tasks_G[newTaskIndex].Delay = newDelay;
			if (newDelay == 0){
				SCH_tasks_G[newTaskIndex].RunMe = 1;
			}
			else {
				SCH_tasks_G[newTaskIndex].RunMe = 0;
			}
			return SCH_tasks_G[newTaskIndex].TaskID;
		}
		else{
			if (SCH_tasks_G[newTaskIndex].pTask == 0x0000){
				SCH_tasks_G[newTaskIndex].Delay = DELAY - sumDelay;
				SCH_tasks_G[newTaskIndex].Period = PERIOD;
				SCH_tasks_G[newTaskIndex].TaskID = Get_New_Task_ID();
				SCH_tasks_G[newTaskIndex].pTask = pFunction;
				if (SCH_tasks_G[newTaskIndex].Delay == 0){
					SCH_tasks_G[newTaskIndex].RunMe = 1;
				}
				else{
					SCH_tasks_G[newTaskIndex].RunMe = 0;
				}
				return SCH_tasks_G[newTaskIndex].TaskID;
			}
		}
	}
	return SCH_tasks_G[newTaskIndex].TaskID;
}
uint8_t SCH_Delete_Task(uint32_t taskID){
	uint8_t Return_code  = 0;
	uint8_t taskIndex;
	uint8_t j;
	if (taskID != NO_TASK_ID){
		for (taskIndex = 0 ; taskIndex < SCH_MAX_TASKS; taskIndex++){
			if (SCH_tasks_G[taskIndex].TaskID == taskID){
				Return_code = 1;
				if (taskIndex != 0 && taskIndex < SCH_MAX_TASKS - 1){
					if ( SCH_tasks_G[taskIndex+1].pTask != 0x0000){
						SCH_tasks_G[taskIndex + 1].Delay += SCH_tasks_G[taskIndex].Delay;
					}
				}
				for (j = taskIndex ; j < SCH_MAX_TASKS - 1 ; j ++){
					SCH_tasks_G[j].Delay = SCH_tasks_G[j+1].Delay;
					SCH_tasks_G[j].Period = SCH_tasks_G[j+1].Period;
					SCH_tasks_G[j].RunMe = SCH_tasks_G[j+1].RunMe;
					SCH_tasks_G[j].TaskID = SCH_tasks_G[j+1].TaskID;
					SCH_tasks_G[j].pTask = SCH_tasks_G[j+1].pTask;
				}
				SCH_tasks_G[j].pTask = 0;
				SCH_tasks_G[j].Period = 0;
				SCH_tasks_G[j].Delay = 0;
				SCH_tasks_G[j].RunMe = 0;
				SCH_tasks_G[j].TaskID = 0;
				return Return_code;
			}
		}
	}
	return Return_code;
}
void SCH_Dispatch_Tasks(void){
	if (SCH_tasks_G[0].pTask){
		if ( SCH_tasks_G[0].RunMe > 0){
			(*SCH_tasks_G[0].pTask)();
			SCH_tasks_G[0].RunMe = 0;
			sTask temp = SCH_tasks_G[0];
			SCH_Delete_Task(temp.TaskID);
			if (temp.Period != 0){
				SCH_Add_Task(temp.pTask, temp.Delay,temp.Period);
			}
		}
	}
}
uint32_t Get_New_Task_ID(void){
	newTaskID++;
	if(newTaskID == NO_TASK_ID){
		newTaskID++;
	}
	return newTaskID;
}
#ifdef __cplusplus
}   
#endif
#else
#include "main.h"

typedef struct {
	void (*pTask)(void);
	uint32_t Delay;
	uint32_t Period;
	uint8_t RunMe;
	uint8_t TaskID;
}sTask;
sTask SCH_tasks_G[SCH_MAX_TASKS];

/*
* @brief : Initialize all task in the array
* @para : none
* @retval : none
* */
void SCH_Init(void){
	unsigned char i;
	for (i = 0;i < SCH_MAX_TASKS; i++){
		SCH_Delete_Task(i);
	}
	HAL_TIM_Base_Start_IT(&htim2);
}


/*
* @brief : Update the delay of the task
* @para : none
* @retval : none
* */
void SCH_Update(void){
	unsigned char index;
	for (index = 0 ; index < SCH_MAX_TASKS; index++){
		if (SCH_tasks_G[index].pTask){
			if (SCH_tasks_G[index].Delay == 0){
				SCH_tasks_G[index].RunMe += 1;
				if (SCH_tasks_G[index].Period){
					SCH_tasks_G[index].Delay = SCH_tasks_G[index].Period;
				}
			}
			else{
				SCH_tasks_G[index].Delay--;
			}
		}
	}
}

1 /*
1 /*
2 * @brief : Add the task to the array
3 * @para : pFunction : Pointer that point to the function need to run
4 * DELAY : the Delay value of the task
5 * PERIOD : the Period value of the task
6 * @retval : -1 - Cannot add to the array
7 * TaskID - Add successfully and return the ID of the task
8 * */
unsigned char SCH_Add_Task(void (*pFunction)(),unsigned int DELAY,unsigned int PERIOD){
	unsigned char index = 0;
	while ( (SCH_tasks_G[index].pTask != 0) && (index < SCH_MAX_TASKS)){
		index++;
	}
	if (index == SCH_MAX_TASKS){
		return SCH_MAX_TASKS;
	}
	SCH_tasks_G[index].Delay = DELAY;
	SCH_tasks_G[index].Period = PERIOD;
	SCH_tasks_G[index].pTask = pFunction;
	SCH_tasks_G[index].RunMe = 0;
	SCH_tasks_G[index].TaskID = index;
	return index;
}
void SCH_Dispatch_Tasks(void){
	unsigned char index;
	for (index = 0; index < SCH_MAX_TASKS ; index++ ){
		if (SCH_tasks_G[index].RunMe > 0){
			(*SCH_tasks_G[index].pTask)();
			SCH_tasks_G[index].RunMe -= 1;
			if (SCH_tasks_G[index].Period == 0){
				SCH_Delete_Task(index);
			}
		}
	}
}
uint8_t SCH_Delete_Task(uint32_t taskID){
	if (SCH_tasks_G[taskID].pTask){
		SCH_tasks_G[taskID].pTask = 0x0000;
		SCH_tasks_G[taskID].Period = 0;
		SCH_tasks_G[taskID].Delay = 0;
		SCH_tasks_G[taskID].RunMe = 0;
	}
	return 1;
}
#endif
