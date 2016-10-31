#include "KrisOS.h"
#include "common.h"


void testTask1(void);
void testTask2(void);
void testTask3(void);



int main(void) {	
	
	KrisOS_init();
	
	KrisOS_create_task(testTask1, 30, 7, 1000);
	KrisOS_create_task(testTask2, 2, 5, 301);
	KrisOS_start();
	
	while(1);
}



void testTask1(void) {
	uint32_t del;
	for (del = 0; del < 10000; del++) printf("A");
	KrisOS_create_task(testTask3, 3, 1, 400);
	while(1) printf("A");
};

void testTask2(void) {
	while(1) printf("B");
};


void testTask3(void) {
	while(1) printf("C");
};
