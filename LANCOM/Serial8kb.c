#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

/* シリアルインターフェースに対応するデバイスファイル */
#define SERIAL_PORT "/dev/ttyUSB0"

/* まず，日時・時刻情報を格納するための変数を宣言 */
// time_t構造体を定義．1970年1月1日からの秒数を格納するもの
struct timeval myTime;
// tm構造体を定義．年月日時分秒をメンバ変数に持つ構造体
struct tm *time_st;

typedef struct DateTime {
	int year;
	int month;
	int day;
	int hour;
	int min;
	int sec;
} DateTime_t;
DateTime_t dateTime;

int main(int argc,char *argv[]) {
    

	/* 受信バッファ */
	char buf[4096];

	/* 送信データ */
	char bufWrite[4096];

	/* 受信データ */
	char bufRead[4096];

	/* ファイルディスクリプタ */
	int fd;

	/* シリアル通信設定 */
    struct termios tio;

	int readIdx;
	int recvFlg = 0;
	int writeIdx = 0;
	int writeSize = 0;
	int writeCnt = 0;
	int loopIdx = 0;

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
	for(writeIdx=0;writeIdx<4096;writeIdx++){
		bufWrite[writeIdx] = (writeIdx & 0x0F) | 0x30;
	}

	/* 受信バッファ、受信データ初期化 */
	for(readIdx=0;readIdx<4096;readIdx++){
		bufRead[readIdx] = 0x00;
		buf[readIdx] = 0x00;
	}
	readIdx = 0;

	/* 時刻取得 */
	// 現在時刻を取得してmyTimeに格納．通常のtime_t構造体とsuseconds_tに値が代入される
	gettimeofday(&myTime, NULL);
	// time_t構造体を現地時間でのtm構造体に変換
	time_st = localtime(&(myTime.tv_sec));

	dateTime.year = time_st->tm_year+1900;
	dateTime.month = time_st->tm_mon+1;
	dateTime.day = time_st->tm_mday;
	dateTime.hour = time_st->tm_hour;
	dateTime.min = time_st->tm_min;
	dateTime.sec = time_st->tm_sec;
	printf("write start:[%d:%d:%d]\n", dateTime.hour, dateTime.min, dateTime.sec);

	/* コマンド送信 */
	printf("write");
	for(loopIdx=0;loopIdx<1000;loopIdx++){
		printf("writeLoop:[%d]\n", loopIdx);
		writeSize = 0;
		while(writeSize<4096){
			writeCnt = write(fd, bufWrite, 4096-writeSize);
			if(writeCnt>0){
				writeSize += writeCnt;
			}
		}
	}
	//printf(":[%02X %02X %02X %02X %02X %02X %02X]\n", bufWrite[0], bufWrite[1], bufWrite[2], bufWrite[3], bufWrite[4], bufWrite[5], bufWrite[6]);

	/* 時刻取得 */
	// 現在時刻を取得してmyTimeに格納．通常のtime_t構造体とsuseconds_tに値が代入される
	gettimeofday(&myTime, NULL);
	// time_t構造体を現地時間でのtm構造体に変換
	time_st = localtime(&(myTime.tv_sec));
	dateTime.year = time_st->tm_year+1900;
	dateTime.month = time_st->tm_mon+1;
	dateTime.day = time_st->tm_mday;
	dateTime.hour = time_st->tm_hour;
	dateTime.min = time_st->tm_min;
	dateTime.sec = time_st->tm_sec;
	printf("write end:[%d:%d:%d]\n", dateTime.hour, dateTime.min, dateTime.sec);

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
