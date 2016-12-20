#include "KrisOS.h"
#include "tm4c123gh6pm.h"



Task *counterDecrTask; 
void counterDecr(void);

Task* primeNumbersTask;
void primeNumbers(void);

Task* suspendingTask;
void suspending(void);

void secondTimer(void);
const size_t secondTimerStackSize = 300;
uint8_t secondTimerStack[secondTimerStackSize];
Task secondTimerTask;


void counterIncr(void);
const size_t counterIncrStackSize = 200;
uint8_t counterIncrStack[counterIncrStackSize];
Task counterIncrTask;

uint32_t counter;


void blinky(void);
const size_t blinkyStackSize = 324;
uint8_t blinkyStack[blinkyStackSize];
Task blinkyTask;



int main(void) {	
	
	KrisOS_init();
	
	KrisOS_declare_task(&secondTimerTask, secondTimer, &secondTimerStack[secondTimerStackSize], 0);
	KrisOS_declare_task(&counterIncrTask, counterIncr, &counterIncrStack[counterIncrStackSize], 0);
	counterDecrTask = (Task*) KrisOS_create_task(counterDecr, 300, 0);
	primeNumbersTask = (Task*) KrisOS_create_task(primeNumbers, 500, 3);
	KrisOS_declare_task(&blinkyTask, blinky, &blinkyStack[blinkyStackSize], 3);

	KrisOS_start();
	
	while(1);
}



void secondTimer(void) {
	while(1) {
		printf("\n");
		KrisOS_delay_task(1000);
	}
}

void counterIncr(void) {
	counter = 0;
	while(1) {
		counter++;
		KrisOS_delay_task(1);		
	}		
}

void counterDecr(void) {
	counter = 0;
	while(1) {
		counter--;
		KrisOS_delay_task(1);	
	}		
}


void primeNumbers(void) {
	
	int32_t low, high, i, flag;
		
	while(1) {
		low = 2;
		high = 50000;
		printf("\nPrime numbers between %d and %d are: \n", low, high);

		while (low < high)
		{
			flag = 0;
			for(i = 2; i <= low/2; ++i)
				if(low % i == 0)
				{
					flag = 1;
					break;
				}

			if (flag == 0)
				printf("%d ", low);

			++low;
		}
	}		
}


void portFInit(void) {
	SYSCTL->RCGCGPIO |= 0x00000020;
	while ((SYSCTL->RCGCGPIO & 0x00000020) == 0);
	GPIOF->LOCK = 0x4C4F434B;
	GPIOF->CR = 0x1F;
	GPIOF->DIR = 0x0E;
	GPIOF->PUR = 0x11;
	GPIOF->DEN = 0x1F;
}

uint32_t portFInput(void) {
	return (GPIOF->DATA & 0x11);
}

void portFOutput(uint32_t data) {
	GPIOF->DATA = data;
}


void blinky(void) {
	portFInit();
	
	while(1) {
		portFOutput(0x02);
		KrisOS_delay_task(300);
		portFOutput(0x04);
		KrisOS_delay_task(300);
		portFOutput(0x08);
		KrisOS_delay_task(300);
		portFOutput(0x00);
		KrisOS_delay_task(715);
		portFOutput(0x07);
		KrisOS_delay_task(715);
		portFOutput(0x00);
		KrisOS_delay_task(715);
	}
}



