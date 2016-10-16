#include "KrisOS.h"
#include "common.h"


int main(void) {	
	int16_t* ptr;
	KrisOS_init();
	printf("dkjaksdakjdalskdsakd");
	
	
	ptr = KrisOS_malloc(20 * sizeof(int16_t));
	ptr = KrisOS_malloc(13 * sizeof(uint16_t));
	printf("%p\n", ptr);
	KrisOS_free(ptr);
	while(1);
}
