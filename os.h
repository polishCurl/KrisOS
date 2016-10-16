/*******************************************************************************
* File:     	os.h
* Brief:    	os.c header file
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	30/09/2016
* Last mod: 	30/09/2016
*
* Note: 		Method declarations visible to OS users for accessing its 
*				functionality
*******************************************************************************/
#include "svc.h"
#include "heap.h"
#include "fifo.h"



/*-------------------------------------------------------------------------------
* Function:    	os_init
* Purpose:    	KrisOS initialisation method
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
extern void os_init(void);
