/********************************************************************************/
/*	Object Name		:Header file of sample for CCIEF-BASIC Slave				*/
/*	File Name		:CCIEF_BASIC_SLAVE.h										*/
/*	Data			:2017/03/21													*/
/*	Version			:2.00.0														*/
/*																				*/
/*	COPYRIGHT (C) 2016 CC-Link Partner Association ALL RIGHTS RESERVED			*/
/********************************************************************************/

#ifndef		__CCIEF_BASIC_SLAVE_H__
#define		__CCIEF_BASIC_SLAVE_H__

#include <stdint.h>

/*[ Definition for sample code ]*/
#define	CCIEF_BASIC_SLAVE_ERR_OK								0
#define	CCIEF_BASIC_SLAVE_ERR_NG								(-1)
#define CCIEF_BASIC_SLAVE_ERR_DEVICE_RANGE						(-100)

/*[ Definition for CCIEF-BASIC environment ]*/
#define	CCIEF_BASIC_PROTCOL_VERSION								2			/* Protocol Version */
#define	CCIEF_BASIC_PORT_NUMBER									61450		/* Port number */
#define	CCIEF_BASIC_MAX_FRAME_SIZE								6000		/* Size of MAX Packet [byte] */
#define	CCIEF_BASIC_RX_RY_SIZE									8			/* Size of RX/RY [byte] */
#define	CCIEF_BASIC_RWW_RWR_SIZE								64			/* Size of RWw/RWr [byte] */
#define	CCIEF_BASIC_MAX_NUMBER_OF_OCCUPIED_STATIONS_GROUP		16			/* Number of MAX occupied stations for group */
#define	CCIEF_BASIC_SLMP_COMMAND_CYCLIC_DATA					0x0E70		/* Cyclic Data(0E70h) of SLMP Command */
#define	CCIEF_BASIC_CYCLIC_TRANSMISSION_TIMEOUT					500			/* Default cyclic transmission timeout [ms] */
#define	CCIEF_BASIC_CYCLIC_TRANSMISSION_TIMEOUT_COUNT			3			/* Default count of cyclic transmission timeout */
#define	CCIEF_BASIC_UNIT_INFO_APPLICATION_MASK					0x0001		/* Mask of application for setting the unit info */
#define	CCIEF_BASIC_UNIT_INFO_APPLICATION_STOP					0x0000		/* Stopping application for setting the unit info */
#define	CCIEF_BASIC_UNIT_INFO_APPLICATION_RUNNING				0x0001		/* Running application for setting the unit info */
#define	CCIEF_BASIC_UNIT_INFO_APPLICATION_STOP_FACTOR_MASK		0x0002		/* Mask of an application stop factor for setting the unit info */
#define	CCIEF_BASIC_UNIT_INFO_APPLICATION_STOP_FACTOR_INITIAL	0x0000		/* Application stop factor by initial */
#define	CCIEF_BASIC_UNIT_INFO_APPLICATION_STOP_FACTOR_ERROR		0x0000		/* Application stop factor by error */
#define	CCIEF_BASIC_UNIT_INFO_APPLICATION_STOP_FACTOR_USER		0x0002		/* Application stop factor by user */

/*[ Definition for CCIEF-BASIC End Code ]*/
#define	CCIEF_BASIC_END_OK										0x0000		/* Processing success. */
#define	CCIEF_BASIC_END_DUPLICATE_MASTER						0xCFE0		/* Duplicate of the master. */
#define	CCIEF_BASIC_END_NUMBER_OF_OCCUPIED_STATIONS				0xCFE1		/* Invalid number of occupied stations. */
#define	CCIEF_BASIC_END_SLAVE									0xCFF0		/* Error of the slave. */
#define CCIEF_BASIC_END_DISCONNECTED_REQUEST					0xCFFF		/* Disconnected request to Master. */

/*[ Definition for CCIEF-BASIC sample code ]*/
#define	CCIEF_BASIC_BIT_ON										1			/* Bit ON */
#define	CCIEF_BASIC_BIT_OFF										0			/* Bit OFF */
#define	CCIEF_BASIC_CYCLIC_DATA_VALID							1			/* Valid of the cyclic data */
#define	CCIEF_BASIC_CYCLIC_DATA_INVALID							0			/* Invalid of the cyclic data */
#define	CCIEF_BASIC_SLAVE_ID_OCCUPIED_STATIONS					0xFFFFFFFF	/* Slave Id of occupied stations. */
#define	CCIEF_BASIC_DEVICE_TYPE_RX								1			/* Type of device for RX. */
#define	CCIEF_BASIC_DEVICE_TYPE_RY								2			/* Type of device for RY. */
#define	CCIEF_BASIC_DEVICE_TYPE_RWW								3			/* Type of device for RWw. */
#define	CCIEF_BASIC_DEVICE_TYPE_RWR								4			/* Type of device for RWr. */
#define	CCIEF_BASIC_OUTPUT_DATA_CLEAR							0			/* Clear of output data */
#define	CCIEF_BASIC_OUTPUT_DATA_HOLD							1			/* Hold of output data */
#define	CCIEF_BASIC_OUTPUT_DATA_HOLD_USER_STOP					2			/* Hold of output data only user stop */

/*[ Structure for sample code ]*/
#ifdef _WIN32
typedef struct 
{
	uint16_t	usVenderCode;				/* Vender code */
	uint32_t	ulModelCode;				/* Model code */
	uint16_t	usMachineVersion;			/* Machine version */
	uint32_t	ulIpAddress;				/* Slave ip address */
	int			iOccupiedStationNumber;		/* Number of occupied stations */
	int			iOutputHoldClearSetting;	/* Output Hold/Clear setting */
} CCIEF_BASIC_SLAVE_INFO;
#elif __linux__
typedef struct 
{
	uint16_t	usVenderCode;				/* Vender code */
	uint32_t	ulModelCode;				/* Model code */
	uint16_t	usMachineVersion;			/* Machine version */
	uint32_t	ulIpAddress;				/* Slave ip address */
	uint32_t	ulSubnetMask;				/* Slave subnet mask */
	int			iOccupiedStationNumber;		/* Number of occupied stations */
	int			iOutputHoldClearSetting;	/* Output Hold/Clear setting */
} CCIEF_BASIC_SLAVE_INFO;
#endif

typedef struct 
{
	uint16_t	usUnitInfo;			/* Information of the unit */
	uint16_t	usReserve;			/* Reserve */
	uint8_t		aucTimeData[8];		/* Time of the master */
} CCIEF_BASIC_MASTER_NOTIFY_INFO;

typedef struct
{
	uint16_t						usProtocolVersion;				/* Protocol version */
	uint32_t						ulId;							/* Id of the master */
	uint8_t							ucGroupNumber;					/* Group number */
	uint16_t						usParameterIdNumber;			/* Parameter id number */
	CCIEF_BASIC_MASTER_NOTIFY_INFO	NotifyInfo;						/* Notify information of the master */
} CCIEF_BASIC_SLAVE_MASTER_INFO;

/*[ Definition of callback function ]*/
typedef void(*CCIEF_BASIC_SLAVE_CALLBACK_RECV_CYCLIC_DATA)( int iCyclicState, int iOccupiedStationNumber );
typedef void(*CCIEF_BASIC_SLAVE_CALLBACK_CYCLIC_DISCONNECTION)( void );

/* Definition of function of sample code */
extern int ccief_basic_slave_initialize( CCIEF_BASIC_SLAVE_INFO *pInitialInfo, 
										 CCIEF_BASIC_SLAVE_CALLBACK_RECV_CYCLIC_DATA pRecvCyclicDataFunc,
										 CCIEF_BASIC_SLAVE_CALLBACK_CYCLIC_DISCONNECTION pCyclicDisconnectionFunc );
extern void ccief_basic_slave_terminate( void );
extern int ccief_basic_slave_main( void );
extern int ccief_basic_slave_set_rx( int iNumber, int iValue );
extern int ccief_basic_slave_get_ry( int iNumber, int *piValue );
extern int ccief_basic_slave_get_rww( int iNumber, uint16_t *pusValue );
extern int ccief_basic_slave_set_rwr( int iNumber, uint16_t usValue );
extern uint16_t *ccief_basic_slave_get_pointer( int iDeviceType );
extern void ccief_basic_slave_set_unit_info( uint16_t usUnitInfo );
extern void ccief_basic_slave_set_err_code( uint16_t usErrCode );
extern void ccief_basic_slave_set_unit_data( uint32_t ulUnitData );
extern void ccief_basic_slave_get_master_info( CCIEF_BASIC_SLAVE_MASTER_INFO *pInfo );

#endif
/*EOF*/