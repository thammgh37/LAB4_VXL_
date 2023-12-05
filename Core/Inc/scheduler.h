/*
 * app_scheduler.h
 *
 *  Created on: Nov 27, 2019
 *      Author: VAIO
 */

#ifndef APP_SCHEDULER_H_
#define APP_SCHEDULER_H_

#include "stdint.h"
#include "main.h"

#define SCH_MAX_TASKS 			10
#define	NO_TASK_ID				0
#define TIME_CYCLE				10
#ifdef __cplusplus
extern "C" {
#endif

#ifdef SCHE_O1
	void SCH_Dispatch_Tasks(void);
	void SCH_Init(void);
	void SCH_Update(void);
	unsigned char SCH_Add_Task(void (*p_function)(), unsigned int DELAY, unsigned int PERIOD);
	void SCH_Dispatch_Tasks(void);
	uint8_t SCH_Delete_Task(uint32_t TASK_ID);
#else
	void SCH_Init(void);
	void SCH_Update(void);
	unsigned char SCH_Add_Task(void (*pFunction)(),unsigned int DELAY,unsigned int PERIOD);
	uint8_t SCH_Delete_Task(uint32_t taskID);
	uint32_t Get_New_Task_ID(void);
#ifdef __cplusplus
}   
#endif

#endif

#endif /* APP_SCHEDULER_H_ */
