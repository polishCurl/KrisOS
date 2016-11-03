#include "KrisOS.h"
#include "common.h"


#define TASK1_ID 1
#define TASK2_ID 2
#define TASK3_ID 3


void testTask1(void);
void testTask2(void);
void testTask3(void);

void os_sleep(void);

int main(void) {	
	
	KrisOS_init();
	
	KrisOS_create_task(testTask1, TASK1_ID, 7, 1000);
	KrisOS_create_task(testTask2, TASK2_ID, 5, 301);
	KrisOS_create_task(testTask3, TASK3_ID, 1, 400);
	KrisOS_start();
	
	while(1);
}



void testTask1(void) {
	int32_t low, high, i, flag;
	
	low = 1;
	high = 10000;

    while (low < high)
    {
        flag = 0;

        for(i = 2; i <= low/2; ++i)
        {
            if(low % i == 0)
            {
                flag = 1;
                break;
            }
        }

        if (flag == 0)
            printf("Prime number: %d\n", low);

        ++low;
    }
	KrisOS_delete_task(TASK3_ID);
	KrisOS_delete_task(TASK1_ID);
	while(1);
};

void testTask2(void) {
	while(1) printf("B");
};


void testTask3(void) {
	while(1) printf("C");
};
