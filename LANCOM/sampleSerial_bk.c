
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define SERIAL_PORT "/dev/ttyUSB0"  /* シリアルインターフェースに対応するデバイスファイル */

int main(int argc,char *argv[]) {
    char bufWrite[255];                    /* バッファ */
    char bufRead[255];                    /* バッファ */
    char buf[255];                    /* バッファ */
    int fd;                           /* ファイルディスクリプタ */
    struct termios tio;    /* シリアル通信設定 */
	int readIdx;
	int recvFlg = 0;
	time_t timeStart;
	time_t timeCurrent;
	int isDebug=0;

	printf("argc:%d\n",argc);
	if(argc==2){
		isDebug = atoi(argv[1]);
	}

    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);   /* デバイスをオープンする */
    
    ioctl(fd, TCGETS, &tio);       /* 現在のシリアルポートの設定を待避させる */
	tio.c_oflag=0;               /* rawモード */
	tio.c_lflag=0;               /* 非カノニカル入力 */
    ioctl(fd, TCSETS, &tio);       /* ポートの設定を有効にする */

	bufWrite[0] = 0x02;
	bufWrite[1] = 0x31;
	bufWrite[2] = 0x41;
	bufWrite[3] = 0x30;
	bufWrite[4] = 0x30;
	bufWrite[5] = 0x03;
	bufWrite[6] = 0x73;

	for(readIdx=0;readIdx<255;readIdx++){
		bufRead[readIdx] = 0x00;
	}

	if (isDebug) printf("write");
	write(fd, bufWrite, 7);
	if (isDebug) printf(":[%02X %02X %02X %02X %02X %02X %02X]\n", bufWrite[0], bufWrite[1], bufWrite[2], bufWrite[3], bufWrite[4], bufWrite[5], bufWrite[6]);
	readIdx = 0;

	recvFlg = 1;
	time(&timeStart);
	time(&timeCurrent);

	do{
		time(&timeCurrent);
		
		if(difftime(timeCurrent,timeStart)>3){
			recvFlg=0;
		}
		if(read(fd, buf, 1)>0){
			bufRead[readIdx]=buf[0];

			if(recvFlg==3) {
				//BCCチェック省略
				recvFlg=0;
			}
			if((recvFlg==2) && (bufRead[readIdx]==0x03)) {
				recvFlg=3;
			}
			if((recvFlg==1) && (bufRead[readIdx]==0x02)) {
				recvFlg=2;
			}

			readIdx++;
			if(difftime(timeCurrent,timeStart)>3){
				printf("time out.\n");
				recvFlg=0;
			}
		}
	}while(recvFlg!=0);

	if (isDebug) printf("read[%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X]\n", bufRead[0], bufRead[1], bufRead[2], bufRead[3], bufRead[4], bufRead[5], bufRead[6], bufRead[7], bufRead[8], bufRead[9], bufRead[10], bufRead[11], bufRead[12]);

	/* デバイスのクローズ */
    close(fd);

    return 0;
}
