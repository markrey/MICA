//全軸の絶対座標、機械座標、相対座標、残移動量を取得し表示します。

#include "fwlib32.h"
#include <stdio.h>

unsigned short exampleCncAllclibhndl3( void )
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
	return h;
}

void exampleCncRdaxisdata( unsigned short h )
{
	struct odbdy buf ;
	unsigned int idx ;
	int MAX = 1;

	ODBAXDT pos[4*MAX_AXIS];
	short types[4] = {0, 1, 2, 3};
	short num = sizeof(types) / sizeof(types[0]);
	short len = MAX_AXIS;
	short ret = cnc_rdaxisdata(h, 1, types, 4, &len, pos);

	printf("cnc_rdaxisdata result:%d:\n",ret);
	if(!ret) {
		int i;
		printf("ABSOLUTE:\n");
		for(i = 0 * MAX_AXIS ; i < 0 * MAX_AXIS + len ; i++) {
			printf("%s = %d\n", pos[i].name, pos[i].data);
		}
		printf("MACHINE:\n");
		for(i = 1 * MAX_AXIS ; i < 1 * MAX_AXIS + len ; i++) {
			printf("%s = %d\n", pos[i].name, pos[i].data);
		}
		printf("RELATIVE:\n");
		for(i = 2 * MAX_AXIS ; i < 2 * MAX_AXIS + len ; i++) {
			printf("%s = %d\n", pos[i].name, pos[i].data);
		}
		printf("DISTANCE TO GO:\n");
		for(i = 3 * MAX_AXIS ; i < 3 * MAX_AXIS + len ; i++) {
			printf("%s = %d\n", pos[i].name, pos[i].data);
		}
	}

	cnc_rddynamic( h, -1, sizeof(buf), &buf ) ;
	printf( "Current program = %d   Main program = %d\n", buf.prgnum, buf.prgmnum ) ;
	printf( "Sequence number = %ld\n", buf.seqnum ) ;
	printf( "actf = %ld   acts = %ld\n", buf.actf, buf.acts ) ;
	printf( "Alarm status = %d\n", buf.alarm ) ;
	printf( "AXIS Absolute  Relative  Machine   Distance\n" ) ;
	printf( "----+---------+---------+---------+--------\n" ) ;
	for ( idx = 0 ; idx < MAX ; idx++ ) {
		printf( "  %u  %8ld  %8ld  %8ld  %8ld\n", idx,
			buf.pos.faxis.absolute[idx], buf.pos.faxis.relative[idx], buf.pos.faxis.machine[idx], buf.pos.faxis.distance[idx] ) ;
	}

//以下のプログラムは指定された番号のプログラムをサーチし、結果を表示します。

	ret = cnc_search( h, num ) ;
	switch ( ret ) {
	case EW_OK:
		printf( "PROGRAM O%d has been searched.\n", num ) ;
		break;
	case EW_DATA:
		printf( "PROGRAM O%d doesn't exist.\n", num ) ;
		break;
	case EW_PROT:
		printf( "PROTECTED.\n" ) ;
		break;
	case EW_BUSY:
		printf( "REJECTED.\n" ) ;
		break;
	default:
		printf( "ERROR:%d.\n" ,ret) ;
		break;
	
	}
}

void main() {
	unsigned short h;
	cnc_startupprocess(3, "/root/focas2.log");
	printf("test\n");
	h = 0;
	h = exampleCncAllclibhndl3();
	exampleCncRdaxisdata(h);
	cnc_exitprocess();
}

