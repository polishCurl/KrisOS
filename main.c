#include "kris_embed_os.h"
#include "common.h"
#include "fifo.h"

add_fifo(int16_t, 4, test)

int main(void) {
	int16_t last_item;

	test_fifo_init();
	printf("START OF TEST\n");
	printf("%d\n", test_fifo_put(0x2222));
	printf("%d\n", test_fifo_put(0x3333));
	
	printf("%d\n", test_fifo_get(&last_item));
	printf("%d\n", test_fifo_put(0x4444));
	printf("%d\n", test_fifo_put(0x6666));
	printf("%d\n", test_fifo_get(&last_item));
	printf("%d\n", test_fifo_put(0x7777));
	printf("%d\n", test_fifo_put(0x8888));
	
	
	/*
	int16_t* arr;
	int32_t index;
	
	arr = (int16_t*) malloc(12 * sizeof(int16_t));
	
	printf("Address allocated: %p\n", arr);
	
	for (index = 0; index < 12; index++)
		arr[index] = 0x1FFF;
	*/
	while(1);
}
