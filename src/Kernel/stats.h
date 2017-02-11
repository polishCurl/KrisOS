/*******************************************************************************
* File:     	stats.h
* Brief:    	Header file for stats.c
* Author: 		Krzysztof Koch
* Version:		V1.00
* Date created:	07/02/2017
* Last mod: 	07/02/2017
*
* Note: 		
*******************************************************************************/



#ifdef SHOW_DIAGNOSTIC_DATA
/*-------------------------------------------------------------------------------
* Function:		stats_init
* Purpose:    	Initialise the statistics task
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void stats_init(void);



/*-------------------------------------------------------------------------------
* Function:		stats_task
* Purpose:    	KrisOS performance statistics task. Displays diagnostic data. 
* Arguments:	-
* Returns: 		-
--------------------------------------------------------------------------------*/
void stats_task(void);

#endif
