#include "KrisOS.h"


Task* primeNumbersTask;
void primeNumbers(void);

Task* startTask;
void starter(void);


void secondTimer(void);
const size_t secondTimerStackSize = 300;
uint8_t secondTimerStack[secondTimerStackSize];
Task secondTimerTask;

void blinky(void);
const size_t blinkyStackSize = 324;
uint8_t blinkyStack[blinkyStackSize];
Task blinkyTask;

Mutex serialMonitorMux;



int main(void) {	
	
	KrisOS_init();
	startTask = (Task*) KrisOS_task_create(starter, 200, 0);
	KrisOS_start();
	while(1);
}



void starter(void) {
	KrisOS_task_create_static(&secondTimerTask, secondTimer, &secondTimerStack[secondTimerStackSize], 1);
	primeNumbersTask = (Task*) KrisOS_task_create(primeNumbers, 500, 3);
	KrisOS_task_create_static(&blinkyTask, blinky, &blinkyStack[blinkyStackSize], 3);
	KrisOS_mutex_init(&serialMonitorMux);
}



void secondTimer(void) {
	while(1) {
		KrisOS_mutex_lock(&serialMonitorMux);
		fprintf(uart, "\nKurwy cwele i menele\n");
		KrisOS_mutex_unlock(&serialMonitorMux);
		KrisOS_task_sleep(1000);
	}
}



void primeNumbers(void) {
	
	int32_t low, high, i, flag;
		
	while(1) {
		low = 2;
		high = 50000;
		KrisOS_mutex_lock(&serialMonitorMux);
		fprintf(uart, "\nPrime numbers between %d and %d are: \n", low, high);
		KrisOS_mutex_unlock(&serialMonitorMux);

		while (low < high)
		{
			flag = 0;
			for(i = 2; i <= low/2; ++i)
				if(low % i == 0)
				{
					flag = 1;
					break;
				}

			if (flag == 0) {
				KrisOS_mutex_lock(&serialMonitorMux);
				fprintf(uart, "%d ", low);
				KrisOS_mutex_unlock(&serialMonitorMux);
			}
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
		KrisOS_task_sleep(300);
		portFOutput(0x04);
		KrisOS_task_sleep(300);
		portFOutput(0x08);
		KrisOS_task_sleep(300);
		portFOutput(0x00);
		KrisOS_task_sleep(715);
		portFOutput(0x07);
		KrisOS_task_sleep(715);
		portFOutput(0x00);
		KrisOS_task_sleep(715);
		KrisOS_mutex_lock(&serialMonitorMux);
		fprintf(uart, " WWWWWWWWWWWWWWWWWWWWWWWWWW ");
		KrisOS_mutex_unlock(&serialMonitorMux);
	}
}
