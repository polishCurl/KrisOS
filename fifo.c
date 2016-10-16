/*******************************************************************************
* File:     	fifo.c
* Brief:    	Dynamically allocated FIFO implementation
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	28/09/2016
* Last mod: 	28/09/2016
*
* Note: 		FIFO build macro allowing specification of FIFOs with
* 				different names, datatypes and sizes
*******************************************************************************/
#include "common.h"
#include "KrisOS.h"


typedef struct {
	void **data;
	uint32_t tail;
	uint32_t head;
	size_t size;
} FIFO;


FIFO* KrisOS_request_fifo(size_t size) {
	FIFO* ptr = KrisOS_malloc(size); 
	
	return ptr;
}
