/********************************************************************************/
/*	Object Name		:Header file of sample of user program						*/
/*	File Name		:USER_SAMPLE.h												*/
/*	Data			:2016/08/01													*/
/*	Version			:2.00.0														*/
/*																				*/
/*	COPYRIGHT (C) 2016 CC-Link Partner Association ALL RIGHTS RESERVED			*/
/********************************************************************************/

#ifndef		__USER_SAMPLE_H__
#define		__USER_SAMPLE_H__

#include <stdint.h>

/*[ Definition of profile of the CCIEF-BASIC Slave (Please rewrite for user environment) ]*/
#define	USER_PROFILE_VENDOR_CODE					0x1234
#define	USER_PROFILE_MODEL_CODE						0x00000001
#define	USER_PROFILE_MACHINE_VERSION				0x0001

/*[ Definition for user environment ]*/
#define	USER_ERR_OK									0
#define	USER_ERR_NG									(-1)
#define	USER_EXIT									1

/*[ Definition the state for master application ]*/
#define	USER_APPLICATION_STATE_INITIAL				0		/* Initial */
#define	USER_APPLICATION_STATE_STOP					1		/* Stop */
#define	USER_APPLICATION_STATE_RUNNING				2		/* Running */
#define	USER_APPLICATION_STATE_ERROR				3		/* Error */

/*[ Definition the event for master application ]*/
#define	USER_APPLICATION_EVENT_INITIAL				0		/* Application Initial */
#define	USER_APPLICATION_EVENT_STOP					1		/* Application Stop */
#define	USER_APPLICATION_EVENT_START				2		/* Application Start */
#define	USER_APPLICATION_EVENT_ERROR				3		/* Application Error */
#define	USER_APPLICATION_EVENT_CANCEL_ERROR			4		/* Cancel for Application Error */

/*[ Definition of prameter for sample code (Please rewrite for user environment) ]*/
/* Group */
#define	USER_PARAMETER_GROUP_ID_TOTAL_NUMBER						1
#define	USER_PARAMETER_GROUP_ID_GROUP_NUMBER						2
#define	USER_PARAMETER_GROUP_ID_DISCONNECTION_DETECT_TIME			3
#define	USER_PARAMETER_GROUP_ID_DISCONNECTION_DETECT_TIME_COUNT		4
#define	USER_PARAMETER_GROUP_ID_CONSTANT_LINK_SCAN_TIME				5
/* Parameter */
#define	USER_PARAMETER_SLAVE_ID_TOTAL_NUMBER						1
#define	USER_PARAMETER_SLAVE_ID_IP_ADDRESS							2
#define	USER_PARAMETER_SLAVE_ID_OCCUPIED_STATION_NUMBER				3
#define	USER_PARAMETER_SLAVE_ID_GROUP_NUMBER						4

#endif
/*EOF*/