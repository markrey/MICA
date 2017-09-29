/********************************************************************************/
/*	Object Name		:Header file of sample for CCIEF-BASIC Slave				*/
/*	File Name		:SOCKET.h													*/
/*	Data			:2017/03/21													*/
/*	Version			:1.02.4														*/
/*																				*/
/*	COPYRIGHT (C) 2016 CC-Link Partner Association ALL RIGHTS RESERVED			*/
/********************************************************************************/

#ifndef		__SOCKET_H__
#define		__SOCKET_H__

#include <stdint.h>
#ifdef _WIN32
#include <winsock2.h>
#elif __linux__
#include <sys/socket.h>
#endif

/*[ Definition for sample code ]*/
#define	SOCKET_ERR_OK							0
#define	SOCKET_ERR_SOCKET						(-100)
#define	SOCKET_ERR_RECV							(-103)
#define	SOCKET_ERR_SEND							(-104)
#define	SOCKET_ERR_NO_RECEIVABLE				(-200)

/* Definition of function of sample code */
#ifdef _WIN32
extern int socket_initialize( SOCKET *sock, uint32_t ulIpAddress, uint16_t usPortNumber );
extern void socket_terminate( SOCKET sock );
extern int socket_recv( SOCKET sock, uint8_t *pucStream, int iLength, uint32_t *pulRecvAddr, uint16_t *pusRecvPortNumber );
extern int socket_send( SOCKET sock, uint8_t *pucStream, int iLength, uint32_t ulSendAddr, uint16_t usSendPortNumber );
#elif __linux__
extern int socket_initialize( int *sock, uint32_t ulIpAddress, uint16_t usPortNumber );
extern void socket_terminate( int sock );
extern int socket_recv( int sock, uint8_t *pucStream, int iLength, uint32_t *pulRecvAddr, uint16_t *pusRecvPortNumber );
extern int socket_send( int sock, uint8_t *pucStream, int iLength, uint32_t ulSendAddr, uint16_t usSendPortNumber );
#endif

#endif
/*EOF*/
