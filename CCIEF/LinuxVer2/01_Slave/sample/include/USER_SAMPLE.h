/********************************************************************************/
/*	Object Name		:Header file of sample of user program						*/
/*	File Name		:USER_SAMPLE.h												*/
/*	Data			:2016/08/01													*/
/*	Version			:1.03.0														*/
/*																				*/
/*	COPYRIGHT (C) 2016 CC-Link Partner Association ALL RIGHTS RESERVED			*/
/********************************************************************************/

#ifndef		__USER_SAMPLE_H__
#define		__USER_SAMPLE_H__

#include <stdint.h>

/*[ Definition of CCIEF-BASIC profile for sample code (Please rewrite for user environment) ]*/
#define	USER_PROFILE_VENDOR_CODE				0x1234
#define	USER_PROFILE_MODEL_CODE					0x00010001
#define	USER_PROFILE_MACHINE_VERSION			0x0001

/*[ Definition of prameter for sample code (Please rewrite for user environment) ]*/
#define	USER_PARAMETER_ID_IP_ADDRESS						1
#define	USER_PARAMETER_ID_SUBNET_MASK						2
#define	USER_PARAMETER_ID_DEFAULT_GW_IP_ADDRESS				3
#define	USER_PARAMETER_ID_OCCUPIED_STATION_NUMBER			4
#define	USER_PARAMETER_ID_CYCLIC_RESPONSE_WAIT_TIME			5
#define	USER_PARAMETER_ID_OUTPUT_HOLD_CLEAR_SETTING			6

/*[ Definition of type name for SLMP node (Please rewrite for user environment) ]*/
#define	USER_TYPE_NAME				"SampleCode      "		/* Less than 16 characters, space(20H) is stored for the remaining character */
#define	USER_TYPE_NAME_CODE			0x1234

/*[ Definition of SLMP server information for sample code (Please rewrite for user environment) ]*/
#define	USER_SERVER_PORT_NUMBER		20000
#define	USER_SERVER_HOSTNAME		"SlaveSample"

/*[ Structure of prameter for sample code (Please rewrite for user environment) ]*/
typedef struct 
{
	uint32_t	ulIpAddress;				/* Slave ip address */
	uint32_t	ulSubnetMask;				/* Subnet Mask */
	uint32_t	ulDefaultGatewayIPAddress;	/* Default Gateway IP Address */
	uint16_t	usOccupiedStationNumber;	/* Number of occupied stations */
	uint32_t	ulCyclicResponseWaitTime;	/* Wait for cyclic response time [ms] (0:Not wait) */
	uint16_t	usOutputHoldClearSetting;	/* Output Hold/Clear setting */
} USER_SLAVE_PARAMETER;

/*[ Definition for sample code ]*/
#define	USER_ERR_OK			0
#define	USER_ERR_NG			(-1)
#define	USER_RESET_NONE		0
#define	USER_RESET			1


#endif
/*EOF*/