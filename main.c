#include "KrisOS.h"
#include "common.h"


int main(void) {	
	
	int16_t *number;
	int16_t index;
	KrisOS_init();
	
	number = KrisOS_malloc(20 * sizeof(int16_t));
	printf("%p ", number);
	KrisOS_free(number);
	number = KrisOS_malloc(20 * sizeof(int16_t));
	printf("%p ", number);
	while(1);
}
