/********************************************************************************/
/*	Object Name		:Header file of sample for CCIEF-BASIC Master				*/
/*	File Name		:SOCKET.h													*/
/*	Data			:2016/11/01													*/
/*	Version			:1.00														*/
/*																				*/
/*	COPYRIGHT (C) 2016 CC-Link Partner Association ALL RIGHTS RESERVED			*/
/********************************************************************************/

#ifndef		__SOCKET_H__
#define		__SOCKET_H__

#include <stdint.h>
#ifndef __linux__
#include <winsock2.h>
#else
#include <sys/socket.h>
#endif

/*[ Definition for sample code ]*/
#define	SOCKET_ERR_OK							0
#define	SOCKET_ERR_SOCKET						(-100)
#define	SOCKET_ERR_RECV							(-103)
#define	SOCKET_ERR_SEND							(-104)
#define	SOCKET_ERR_NO_RECEIVABLE				(-200)

/* Definition of function of sample code */
#ifndef __linux__
extern int socket_initialize( SOCKET *sock, uint32_t ulIpAddress, uint16_t usPortNumber );
extern void socket_terminate( SOCKET sock );
extern int socket_recv( SOCKET sock, uint8_t *pucStream, int iLength, uint32_t *pulRecvAddr, uint16_t *pusRecvPortNumber );
extern int socket_send( SOCKET sock, uint8_t *pucStream, int iLength, uint32_t ulSendAddr, uint16_t usSendPortNumber );
#else
extern int socket_initialize( int *sock, uint32_t ulIpAddress, uint16_t usPortNumber );
extern void socket_terminate( int sock );
extern int socket_recv( int sock, uint8_t *pucStream, int iLength, uint32_t *pulRecvAddr, uint16_t *pusRecvPortNumber );
extern int socket_send( int sock, uint8_t *pucStream, int iLength, uint32_t ulSendAddr, uint16_t usSendPortNumber );
#endif

#endif
/*EOF*/
