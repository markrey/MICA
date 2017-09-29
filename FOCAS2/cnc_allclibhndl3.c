#include "fwlib32.h"
#include <stdio.h>

void exampleCncAllclibhndl3( void )
{
	unsigned short h;
	short ret;
	ODBST buf;
	printf("start cnc_allclibhndl3.\n");
	ret = cnc_allclibhndl3( "192.168.10.11", 8193, 10, &h ) ;
	printf("cnc_allclibhndl3 result:%d.\n",ret);
	if ( !ret ) {
			printf("cnc_statinfo\n");
			cnc_statinfo( h, &buf ) ;
			printf("cnc_freelibhndl\n");
			cnc_freelibhndl( h ) ;
	} else {
			printf( "ERROR!(%d)\n", ret ) ;
	}
}

void main() {
	cnc_startupprocess(3, "/root/focas2.log");
	printf("test\n");
	exampleCncAllclibhndl3();
	cnc_exitprocess();
}
