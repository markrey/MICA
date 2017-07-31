/********************************************************************************/
/*	Object Name		:Header file of sample for the slaves of CCIEF-BASIC Master	*/
/*	File Name		:CCIEF_BASIC_SLAVES.h										*/
/*	Data			:2016/11/01													*/
/*	Version			:1.01														*/
/*																				*/
/*	COPYRIGHT (C) 2016 CC-Link Partner Association ALL RIGHTS RESERVED			*/
/********************************************************************************/

#ifndef		__CCIEF_BASIC_SLAVES_H__
#define		__CCIEF_BASIC_SLAVES_H__

#include <stdint.h>
#include "CCIEF_BASIC_MASTER.h"

/*[ Definition for CCIEF-BASIC Slaves ]*/
#define	CCIEF_BASIC_CYCLIC_STATE_SET_CYCLIC_STOP		0			/* Setting the cyclic state of stop cyclic. */
#define	CCIEF_BASIC_CYCLIC_STATE_SET_CONNECTING			1			/* Setting the cyclic state of connecting. */
#define	CCIEF_BASIC_CYCLIC_STATE_SET_CYCLIC				2			/* Setting the cyclic state of cyclic. */

/*[ Structure for CCIEF-BASIC Slaves ]*/
typedef struct
{
	CCIEF_BASIC_SLAVE_PARAMETER		*pParameter;						/* Parameter */
	int								iNumber;							/* Slave Number */
	uint32_t						ulId;								/* Id number */
	int								iStationNumber;						/* Number of stations */
	int								iGroupStationNumber;				/* Number of stations for the group */
	int								iCyclicStart;						/* Start cyclic of the user operation */
	int								iState;								/* State of Slave */
	int								iCyclicState;						/* Cyclic state */
	int								iCyclicStateSet;					/* Setting of the cyclic state */
	int								iReceiveComplete;					/* State of response receive */
	int								iDuplicateState;					/* State of slave duplication */
	uint16_t						*pusFrameSequenceNumber;			/* Frame sequence number of the master */
	uint16_t						usProtocolVersion;					/* Protocol version of the slave */
	uint16_t						usEndCode;							/* End code of the slave */
	uint16_t						usFrameSequenceNumber;				/* Frame sequence number of the slave */
	CCIEF_BASIC_SLAVE_NOTIFY_INFO	NotifyInfo;							/* Notification information of the slave */
	uint16_t						usCyclicTransmissionTimeoutCount;	/* Count of cyclic transmission timeout */
	uint16_t						usTimeoutCount;						/* Counter of timeout for the cyclic transmission timeout */
	uint16_t						*pusRWw;							/* Pointer of RWw for the packet */
	uint16_t						*pusRY;								/* Pointer of RY for the packet */
	uint16_t						*pusRWr;							/* Pointer of RWr for the packet */
	uint16_t						*pusRX;								/* Pointer of RX for the packet */
	uint16_t						*pusSlaveRWr;						/* Pointer of RWr for the slave */
	uint16_t						*pusSlaveRX;						/* Pointer of RX for the slave */
} CCIEF_BASIC_SLAVES_CYCLIC_DATA_INFO;

/* Definition of function of sample program */
extern void ccief_basic_slaves_initialize( CCIEF_BASIC_SLAVES_CYCLIC_DATA_INFO *pSlave );
extern void ccief_basic_slaves_execute_state( CCIEF_BASIC_SLAVES_CYCLIC_DATA_INFO *pSlave, int iEvent );

#endif
/*EOF*/