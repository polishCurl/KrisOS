#include "KrisOS.h"
#include "common.h"


int32_t task1ID, task2ID, task3ID;

uint32_t seconds_elapsed;
uint32_t s10_elapsed;

void testTask1(void);
void testTask2(void);
void testTask3(void);


const size_t task2StackSize = 304;
uint8_t task2Stack[task2StackSize];
Task task2;



int main(void) {	
	
	KrisOS_init();
	
	task1ID = KrisOS_create_task(testTask1, 300, 0);
	task2ID = KrisOS_declare_task(&task2, testTask2, &task2Stack[task2StackSize], 1);
	task3ID = KrisOS_create_task(testTask3, 300, 2);
	KrisOS_start();
	
	while(1);
}



void testTask1(void) {
	seconds_elapsed = 0;
	while(1) {
		KrisOS_delay_task(1000);
		seconds_elapsed--;
	}
}

void testTask2(void) {
	
	while(1) {
		printf("w");
		KrisOS_delay_task(247);
		
	}		
}

void testTask3(void) {
	seconds_elapsed = 0;
	while(1) {
		KrisOS_delay_task(1000);
		seconds_elapsed++;
	}		
}


