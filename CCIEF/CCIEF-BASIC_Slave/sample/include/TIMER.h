/********************************************************************************/
/*	Object Name		:Header file of sample of user program for timer			*/
/*	File Name		:TIMER.h													*/
/*	Data			:2016/08/01													*/
/*	Version			:1.00														*/
/*																				*/
/*	COPYRIGHT (C) 2016 CC-Link Partner Association ALL RIGHTS RESERVED			*/
/********************************************************************************/

#ifndef		__TIMER_H__
#define		__TIMER_H__

#include <stdint.h>

/*[ Definition for timer ]*/
#define	TIMER_OK				0
#define	TIMER_RESOURCE_NONE		(-1)
#define	TIMER_MAX				5		/* Number of timer MAX */

/*[ Structure for sample code ]*/
typedef struct 
{
	uint16_t	usYear;				/* Year */
	uint16_t	usMonth;			/* Month */
	uint16_t	usDay;				/* Day */
	uint16_t	usHour;				/* Hour */
	uint16_t	usMinute;			/* Minute */
	uint16_t	usSecond;			/* Second */
	uint16_t	usMilliseconds;		/* Milliseconds */
} TIMER_TIME_DATA;

/*[ Definition of callback function ]*/
typedef void (*TIMER_CALLBACK)( int iId, void *pCallbackArg );

/* Definition of function of the timer */
extern void timer_initialize( void );
extern void timer_terminate( void );
extern void timer_main( void );
extern int timer_start( uint32_t ulTime, int *piId, TIMER_CALLBACK pCallbackFunc, void *pCallbackArg );
extern void timer_stop( int iId );
extern uint32_t timer_get_time( void );
extern uint32_t timer_broadcast_send_wait_time ( uint32_t ulMaxWaitTime );
extern void timer_analyze_time_data( int64_t ullTime, TIMER_TIME_DATA *pTimeData );

#endif
/*EOF*/