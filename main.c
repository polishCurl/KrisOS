#include "KrisOS.h"
#include "common.h"


#define TASK1_ID 1
#define TASK2_ID 2
#define TASK3_ID 3
#define TASK4_ID 4


void testTask1(void);
void testTask2(void);
void testTask3(void);
void testTask4(void);

void os_sleep(void);

int main(void) {	
	
	KrisOS_init();
	
	KrisOS_create_task(testTask1, TASK1_ID, 1, 1000);
	KrisOS_create_task(testTask2, TASK2_ID, 2, 500);
	KrisOS_create_task(testTask3, TASK3_ID, 3, 600);
	KrisOS_create_task(testTask4, TASK4_ID, 4, 1000);
	KrisOS_start();
	
	while(1);
}



void testTask1(void) {
	while(1) printf("X");
};

void testTask2(void) {
	while(1) printf("Y");
};


void testTask3(void) {
	while(1) printf("Z");
	
};

void testTask4(void) {
	while(1) printf("W");
}
