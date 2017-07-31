#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

/* シリアルインターフェースに対応するデバイスファイル */
#define SERIAL_PORT "/dev/ttyUSB0"

int main(int argc,char *argv[]) {
    
	/* 受信バッファ */
	char buf[255];

	/* 送信データ */
	char bufWrite[255];

	/* 受信データ */
	char bufRead[255];

	/* ファイルディスクリプタ */
	int fd;

	/* シリアル通信設定 */
    struct termios tio;

	int readIdx;
	int recvFlg = 0;

	/* デバイスをオープンする */
	fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);

	/* 現在のシリアルポートの設定を取得 */
	ioctl(fd, TCGETS, &tio);

	/* rawモード */
	tio.c_oflag=0;

	/* 非カノニカル入力 */
	tio.c_lflag=0;

	/* ポートの設定を有効にする */
	ioctl(fd, TCSETS, &tio);

	/* 送信データ初期化 */
	bufWrite[0] = 0x02;
	bufWrite[1] = 0x30;
	bufWrite[2] = 0x41;
	bufWrite[3] = 0x30;
	bufWrite[4] = 0x30;
	bufWrite[5] = 0x03;
	bufWrite[6] = 0x72;

	/* 受信バッファ、受信データ初期化 */
	for(readIdx=0;readIdx<255;readIdx++){
		bufRead[readIdx] = 0x00;
		buf[readIdx] = 0x00;
	}
	readIdx = 0;

	/* コマンド送信 */
	printf("write");
	write(fd, bufWrite, 7);
	printf(":[%02X %02X %02X %02X %02X %02X %02X]\n", bufWrite[0], bufWrite[1], bufWrite[2], bufWrite[3], bufWrite[4], bufWrite[5], bufWrite[6]);


	/* データ受信 */
	recvFlg = 1;
	do{
		if(read(fd, buf, 1)>0){
			bufRead[readIdx]=buf[0];
			readIdx++;
		}
	}while(readIdx<13);

	printf("read[%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X]\n", bufRead[0], bufRead[1], bufRead[2], bufRead[3], bufRead[4], bufRead[5], bufRead[6], bufRead[7], bufRead[8], bufRead[9], bufRead[10], bufRead[11], bufRead[12]);

	/* デバイスのクローズ */
    close(fd);

    return 0;
}
