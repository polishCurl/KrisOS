/*******************************************************************************
* File:     	task.c
* Brief:    	
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	21/10/2016
* Last mod: 	21/10/2016
*
* Note: 		
*******************************************************************************/
#include "common.h"
#include "KrisOS.h"


/*-------------------------------------------------------------------------------
* Task control block
*------------------------------------------------------------------------------*/
typedef struct Task {
	void *sp; 					// Stack pointer value
	int8_t id; 					// Task ID
	int8_t priority; 			// Task priority
	struct Task *next; 			// Next task to run
} Task;

Task tcb[MAX_TASKS];


/*-------------------------------------------------------------------------------
* Stack frame
*------------------------------------------------------------------------------*/
uint64_t StackFrame[MAX_TASKS][TASK_STACK_SIZE];



