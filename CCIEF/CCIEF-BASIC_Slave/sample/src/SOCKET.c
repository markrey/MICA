/********************************************************************************/
/*	Object Name		:Sample of user program for CCIEF-BASIC Slave				*/
/*	File Name		:SOCKET.c													*/
/*	Data			:2016/11/01													*/
/*	Version			:1.00														*/
/*																				*/
/*	COPYRIGHT (C) 2016 CC-Link Partner Association ALL RIGHTS RESERVED			*/
/********************************************************************************/

#include "SOCKET.h"

/************************************************************************************/
/* The following is an user defined main program. This main program is one of a		*/
/* sample in the Linux. Please rewrite if necessary.								*/
/*																					*/
/* This sample program for CCIEF-BASIC Slave Application.							*/
/*																					*/
/************************************************************************************/

#include <stdio.h>
#ifndef __linux__
#include <stdint.h>
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <errno.h>

#define SOCKET_ERROR	-1
#define SOCKET_NOT_OPEN	0
#define TRUE			1
#endif
/************************************************************************************/
/* This is an user defined function for initialization of the socket.				*/
/* The following is one of a sample in the Linux. Please rewrite if necessary.		*/
/************************************************************************************/
#ifndef __linux__ 
int socket_initialize( SOCKET *sock, uint32_t ulIpAddress, uint16_t usPortNumber )
#else
int socket_initialize( int *sock, uint32_t ulIpAddress, uint16_t usPortNumber )
#endif
{

#ifndef __linux__
	SOCKET				sock_tmp;
	SOCKADDR_IN			addr;
	int					addr_len = 0;
	BOOL				opt_val = FALSE;
	int					opt_len = sizeof( BOOL );
#else
	struct sockaddr_in	addr;
	int					sock_tmp;
	int					addr_len = 0;
	int					opt_val = 0;
	int					opt_len = sizeof( int );
#endif
	int					iResult;
	u_long				val = 0;

    /* Create a socket */
	sock_tmp = socket( AF_INET, SOCK_DGRAM, 0 );
#ifndef __linux__
	if ( sock_tmp == INVALID_SOCKET )
#else
	if ( sock_tmp < SOCKET_NOT_OPEN )
#endif
	{
#ifndef __linux__
		printf( "socket failed with error: %ld\n", WSAGetLastError() );
#else
		perror( "socket failed with error" );
#endif
		return SOCKET_ERR_SOCKET;
	}

	/* Setup the UDP socket */
	addr.sin_family = AF_INET;					/* Address Famiry : Internet */
	addr.sin_port	= htons(usPortNumber);		/* Port number */
#ifndef __linux__ 
	addr.sin_addr.s_addr = htonl(ulIpAddress);	/* IP address */
#else
	addr.sin_addr.s_addr = INADDR_ANY;			/* It can be bound to all of the local interface */
#endif
	addr_len = sizeof( addr );

#ifndef __linux__
	iResult = bind( sock_tmp, (SOCKADDR *)&addr, (int)addr_len );

#else
	iResult = bind( sock_tmp, (struct sockaddr *)&addr, ( int )addr_len );
#endif
	if ( iResult == SOCKET_ERROR )
	{
#ifndef __linux__
		printf( "bind failed with error: %d\n", WSAGetLastError() );
		closesocket( sock_tmp );
#else
		perror( "bind failed with error" );
		close( sock_tmp );
#endif
		return SOCKET_ERR_SOCKET;
	}

	/* Sets the socket option for broadcast packet. */
	opt_val = TRUE;

	iResult = setsockopt( sock_tmp, SOL_SOCKET, SO_BROADCAST, (char *) &opt_val, opt_len );
	if (iResult == SOCKET_ERROR)
	{
#ifndef __linux__
		printf("ERR : setsockopt : SO_BROADCAST\n");
		closesocket( sock_tmp );
#else
		perror( "ERR : setsockopt : SO_BROADCAST" );
		close( sock_tmp );
#endif
		return SOCKET_ERR_SOCKET;
	}

	/* for Non-blocking operation */
	val = 1;
#ifndef __linux__
	ioctlsocket( sock_tmp, FIONBIO, &val );
#else
	ioctl( sock_tmp, FIONBIO, &val );
#endif

	*sock = sock_tmp;

	return SOCKET_ERR_OK;
}

/************************************************************************************/
/* This is an user defined function for termination of the socket.					*/
/* The following is one of a sample in the Linux. Please rewrite if necessary.		*/
/************************************************************************************/
#ifndef __linux__
void socket_terminate( SOCKET sock )
#else
void socket_terminate( int sock )
#endif
{
	/* Close a socket */
#ifndef __linux__
	closesocket( sock );
#else
	close( sock );
#endif

	return;
}

/************************************************************************************/
/* This is an user defined function for receiving packet. The following is one of a	*/
/* sample in the Linux. Please rewrite if necessary.								*/
/************************************************************************************/
#ifndef __linux__
int socket_recv( SOCKET sock, uint8_t *pucStream, int iLength, uint32_t *pulRecvAddr, uint16_t *pusRecvPortNumber )
#else
int socket_recv( int sock, uint8_t *pucStream, int iLength, uint32_t *pulRecvAddr, uint16_t *pusRecvPortNumber )
#endif
{
	int				iErrCode = 0;
	int				iAddrSize = 0;
#ifndef __linux__
	SOCKADDR_IN		addrSender;
	iAddrSize = sizeof( SOCKADDR_IN );
#else
	struct sockaddr_in		addrSender;
	iAddrSize = sizeof( addrSender );
#endif

	/* Packet receiving */
#ifndef __linux__
	iErrCode = recvfrom( sock, (char *)pucStream, iLength, 0, (SOCKADDR *)&addrSender, &iAddrSize );
#else
	iErrCode = recvfrom( sock, (char *)pucStream, iLength, 0, (struct sockaddr *)&addrSender, &iAddrSize );
#endif

	if( iErrCode == SOCKET_ERROR )
	{
		/*No received data*/
#ifndef __linux__
		errno = WSAGetLastError();
		if ( errno == WSAEWOULDBLOCK )
#else
		if( errno == EAGAIN )
#endif
		{
			return SOCKET_ERR_NO_RECEIVABLE;
		}
		else
		{
			printf( "ERR : recvfrom failed with error: %ld\n", errno );
			return SOCKET_ERR_RECV;
		}
	}
	else if ( iErrCode == 0 )
	{
		printf( "ERR : recvfrom failed with error: disconnected\n" );
		return SOCKET_ERR_RECV;
	}

	(*pulRecvAddr) = ntohl(addrSender.sin_addr.s_addr);
	(*pusRecvPortNumber) = ntohs(addrSender.sin_port);

	return SOCKET_ERR_OK;
}

/************************************************************************************/
/* This is an user defined function for sending packet. The following is one of a	*/
/* sample in the Linux. Please rewrite if necessary.								*/
/************************************************************************************/
#ifndef __linux__
int socket_send( SOCKET sock, uint8_t *pucStream, int iLength, uint32_t ulSendAddr, uint16_t usSendPortNumber )
#else
int socket_send( int sock, uint8_t *pucStream, int iLength, uint32_t ulSendAddr, uint16_t usSendPortNumber )
#endif
{
	int				iErrCode = 0;
	int				iAddrSize = 0;
#ifndef __linux__
	SOCKADDR_IN		addrServer;
#else
	struct sockaddr_in	addrServer;
#endif

	addrServer.sin_family = AF_INET;				/* Address Famiry	: Internet */
	addrServer.sin_port	= htons(usSendPortNumber);	/* Port Number */
	addrServer.sin_addr.s_addr = htonl(ulSendAddr);
	iAddrSize = sizeof( addrServer );

	/* Packet Sending */
#ifndef __linux__
	iErrCode = sendto( sock, (char *)pucStream, iLength, 0, (LPSOCKADDR)&addrServer, iAddrSize );
#else
	iErrCode = sendto( sock, (char *)pucStream, iLength, 0, (struct sockaddr *)&addrServer, iAddrSize );
#endif
	if( iErrCode == SOCKET_ERROR )
	{
#ifndef __linux__
		printf( "sendto failed with error: %ld\n", WSAGetLastError() );
#else
		perror("send to failed with error");
#endif
		return SOCKET_ERR_SEND;
	}

	return SOCKET_ERR_OK;
}
