/********************************************************************************/
/*	Object Name		:Sample of user program for timer							*/
/*	File Name		:TIMER.c													*/
/*	Data			:2016/08/01													*/
/*	Version			:1.00														*/
/*																				*/
/*	COPYRIGHT (C) 2016 CC-Link Partner Association ALL RIGHTS RESERVED			*/
/********************************************************************************/

#ifndef __linux__ 
#include <stdint.h>
#include <stdlib.h>
#else
#endif
#include <time.h>
#include <string.h>
#include "TIMER.h"

/*[ Structure for the timer ]*/
typedef struct 
{
	int				iId;			/* Timer id */
	uint32_t		ulTime;			/* Period of time [ms] */
	uint32_t		ulStart;		/* Start of time [ms] */
	TIMER_CALLBACK	pCallbackFunc;	/* Callback function */
	void			*pCallbackArg;	/* Callback argument */
} TIMER;

static TIMER Timer[TIMER_MAX];
static int iTimerId = 0;

/************************************************************************************/
/* This is an user defined function for initialize the timer.						*/
/************************************************************************************/
void timer_initialize( void )
{
	/* Initialize the timer environment */
	memset( &Timer, 0, sizeof( Timer ) );

	return;
}

/************************************************************************************/
/* This is an user defined function for terminate the timer.						*/
/************************************************************************************/
void timer_terminate( void )
{
	/* Terminate the timer environment */
	/* if needed */

	return;
}

/************************************************************************************/
/* This is an user defined function for main the timer.								*/
/************************************************************************************/
void timer_main( void )
{
	int i, iStopId;
	uint32_t ulCurrent;

	/* main loop for the timer */
	for( i = 0; i < TIMER_MAX; i++ )
	{
		if ( Timer[i].iId != 0 )
		{
			/* Get the difference time */
			ulCurrent = timer_get_time();
			/* Timeout the timer */
			if ( Timer[i].ulTime <= ( ulCurrent - Timer[i].ulStart ))
			{
				iStopId = Timer[i].iId;
				if ( Timer[i].pCallbackFunc != NULL )
				{
					/* Execute of the callback function */
					Timer[i].pCallbackFunc( Timer[i].iId, Timer[i].pCallbackArg );
				}
				/* Initialize of the timer */
				if ( iStopId == Timer[i].iId )
				{
					Timer[i].iId = 0;
				}
			}
		}
	}

	return;
}

/************************************************************************************/
/* This is an user defined function for starting the timer.[ms]						*/
/************************************************************************************/
int timer_start( uint32_t ulTime, int *piId, TIMER_CALLBACK pCallbackFunc, void *pCallbackArg )
{
	int i;

	/* Check the unusing timer */
	for( i = 0; i < TIMER_MAX; i++ )
	{
		if ( Timer[i].iId == 0 )
		{
			break;
		}
	}
	if ( i == TIMER_MAX )
	{
		/* No free timer */
		return TIMER_RESOURCE_NONE;
	}

	iTimerId ++;
	if ( iTimerId == 0 )
	{
		iTimerId = 1;
	}
	Timer[i].iId = iTimerId;

	Timer[i].ulTime = ulTime;
	Timer[i].ulStart = timer_get_time();
	Timer[i].pCallbackFunc = pCallbackFunc;
	Timer[i].pCallbackArg = pCallbackArg;

	if ( piId != NULL )
	{
		*piId = Timer[i].iId;
	}

	return TIMER_OK;
}

/************************************************************************************/
/* This is an user defined function for stoping the timer.							*/
/************************************************************************************/
void timer_stop( int iId )
{
	int i;

	/* Check the using timer */
	for( i = 0; i < TIMER_MAX; i++ )
	{
		if ( Timer[i].iId == iId )
		{
			/* Initialize of the timer */
			Timer[i].iId = 0;
			break;
		}
	}

	return;
}

/************************************************************************************/
/* This is an user defined function for getting the elapsed time from the start		*/
/* of the program.[ms]  Please rewrite for user environment.						*/
/************************************************************************************/
uint32_t timer_get_time( void )
{
	uint32_t ulTime;

	ulTime = (uint32_t)(((int64_t)clock() * 1000 ) / CLOCKS_PER_SEC );

	return ulTime;
}

/************************************************************************************/
/* This is an user defined function for calculate the broad cast wait time.[ms]		*/
/************************************************************************************/
uint32_t timer_broadcast_send_wait_time ( uint32_t ulMaxWaitTime )
{
	uint32_t ulWaitTime;

	/* Change of random number sequence */
	srand( (unsigned)time( NULL ) );

	/* Gets a wait time  */
	ulWaitTime = rand() % ulMaxWaitTime;

	return ulWaitTime;
}

/************************************************************************************/
/* This is an user defined function for analyze the Time Data (Time of UNIX).		*/
/************************************************************************************/
void timer_analyze_time_data( int64_t llTime, TIMER_TIME_DATA *pTimeData )
{
	/* Number of days table of each month (for the year-round, for a leap year) */
	static const uint8_t    aucDaysTable[2][13] = {
		{ 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
		{ 0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
	};
	uint32_t ulElapsedDays;
	uint32_t ulTotalDays;
	uint32_t ulThisYear;
	uint32_t ulLastYearDays;
	uint32_t ulThisYearDays;
	uint32_t ulTempDays;
	uint16_t usThisMonth;
	uint32_t ulThisMinute;
	uint16_t usIndex;

	/* Calculate the number of days elapsed */
	ulElapsedDays = (uint32_t)((( llTime / 1000) / 86400 ) + 719162);

	/* It calculates the total number of days, including the day to the number of days elapsed */
	ulTotalDays = ulElapsedDays + 1;

	/* Initialized with the previous year */
	ulThisYear = ulTotalDays / 365;
	/* Calculate the year */
	while ( 1 )
	{
		/* Calculate the previous year the number of days */
		ulLastYearDays = ( ulThisYear * 365 ) + ( ulThisYear / 4 ) - ( ulThisYear / 100 ) + ( ulThisYear / 400 );
		if ( ulLastYearDays >= ulTotalDays )
		{
			ulThisYear --;
		}
		else
		{
			ulThisYear ++;
			break;
		}
	}

	/* Calculate the number of days from January 1 */
	ulThisYearDays = ulTotalDays - ulLastYearDays;

	/* Calculating the index for the number of days table of each month or the full year or a leap year */
	if (( ulThisYear % 4 == 0 ) && ( ulThisYear % 100 != 0 ) || ( ulThisYear % 400 == 0 ))
	{
		usIndex = 1;
	}
	else
	{
		usIndex = 0;
	}

	/* Calculate the month */
	ulTempDays = 0;
	for ( usThisMonth = 0; ( ulTempDays < ulThisYearDays ) && ( usThisMonth < 12 ); usThisMonth++ )
	{
		ulTempDays += aucDaysTable[ usIndex ][ usThisMonth + 1 ];
	}

	/* Results to the argument */
	pTimeData->usYear  = (uint16_t)ulThisYear;
	pTimeData->usMonth = usThisMonth;
	pTimeData->usDay = (uint16_t)( ulThisYearDays - ( ulTempDays - aucDaysTable[ usIndex ][ usThisMonth ]));
	ulThisMinute = (uint32_t)(( llTime / 1000) % 86400 );
	pTimeData->usHour = (uint16_t)( ulThisMinute / 3600 );
	pTimeData->usMinute = (uint16_t)(( ulThisMinute % 3600 ) / 60 );
	pTimeData->usSecond = (uint16_t)(( ulThisMinute % 3600 ) % 60 );
	pTimeData->usMilliseconds = (uint16_t)( llTime % 1000 );
	
	return;
}
