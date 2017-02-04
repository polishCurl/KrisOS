#include "KrisOS.h"

KrisOS_task_define(secondTimer, 200)
KrisOS_task_define(blinky, 200)

Task* primeNumbersTask;
void primeNumbers(void);

Semaphore testSem;

int main(void) {	
	
	KrisOS_init();
	
	KrisOS_stack_usage((uint32_t*) &secondTimerStack[0], secondTimerStackSize);
	KrisOS_task_create_static(&secondTimerTask, secondTimer, &secondTimerStack[secondTimerStackSize], 1);
	
	primeNumbersTask = (Task*) KrisOS_task_create(primeNumbers, 400, 4);
	
	KrisOS_stack_usage((uint32_t*) &blinkyStack[0], blinkyStackSize);
	KrisOS_task_create_static(&blinkyTask, blinky, &blinkyStack[blinkyStackSize], 3);
	
	KrisOS_sem_init(&testSem, 2);
	
	KrisOS_start();
	while(1);
}



void secondTimer(void) {
	
	KrisOS_sem_try_acquire(&testSem);
	KrisOS_sem_release(&testSem);
	KrisOS_sem_acquire(&testSem);
	KrisOS_sem_acquire(&testSem);
	
	KrisOS_task_sleep(5000);
	KrisOS_sem_release(&testSem);
	
	while(1) {
		KrisOS_mutex_lock(&uartMtx);
		fprintf(uart, "\nKurwy cwele i menele\n");
		KrisOS_mutex_unlock(&uartMtx);
		KrisOS_task_sleep(1000);
	}
}



void primeNumbers(void) {
		
	int32_t low, high, i, flag;
	
	KrisOS_sem_acquire(&testSem);
	
	while(1) {
		low = 2;
		high = 30000;
		KrisOS_mutex_lock(&uartMtx);
		fprintf(uart, "\nPrime numbers between %d and %d are: \n", low, high);
		KrisOS_mutex_unlock(&uartMtx);

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
				KrisOS_mutex_lock(&uartMtx);
				fprintf(uart, "%d ", low);
				KrisOS_mutex_unlock(&uartMtx);
			}
			++low;
		}
		KrisOS_task_sleep(40000);
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
	}
}
