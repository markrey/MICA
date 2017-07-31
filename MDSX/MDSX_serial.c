
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/statvfs.h>
#include <limits.h>

#define SERIAL_PORT "/dev/ttyUSB0"  /* シリアルインターフェースに対応するデバイスファイル */
#define BUFSIZE 255
#define NODESIZE 16
#define MAX_AVGRANGE 100
#define TRENDRANGE 120
#define DAIRYINTERVAL 60
#define WRITECMD 0x41
#define CNTMAX 1023

#define LABEL_DEVICENAME "DEVICENAME"
#define LABEL_BAUDRATE "BAUDRATE"
#define LABEL_DATABIT "DATABIT"
#define LABEL_STOPBIT "STOPBIT"
#define LABEL_PARITY "PARITY"
#define LABEL_MISTUNIT "MISTUNIT"
#define LABEL_TEMPUNIT "TEMPUNIT"
#define LABEL_PREALARM "PREALARM"
#define LABEL_HIGHALARM "HIGHALARM"
#define LABEL_AVGRANGE "AVGRANGE"
#define LABEL_DAIRYINTERVAL "DAIRYINTERVAL"
#define LABEL_DAIRYOUTPUTDIR "DAIRYOUTPUTDIR"
#define LABEL_ALARMFILTER "ALARMFILTER"
#define LABEL_TIMEOUTFILTER "TIMEOUTFILTER"

#define LABEL_NETMODE "NETMODE"
#define LABEL_IPADDRESS "IPADDRESS"
#define LABEL_SUBNETMASK "SUBNETMASK"
#define LABEL_GATEWAYADDRESS "GATEWAYADDRESS"
#define LABEL_DNSADDRESS "DNSADDRESS"
#define LABEL_BROADCAST "BROADCAST"


#define DEFAULT_DEVICENAME "/dev/ttyUSB0"
#define DEFAULT_BAUDRATE "19200"
#define DEFAULT_DATABIT "8"
#define DEFAULT_STOPBIT "1"
#define DEFAULT_PARITY "EVEN"
#define DEFAULT_MISTUNIT "g/L"
#define DEFAULT_TEMPUNIT "℃"
#define DEFAULT_PREALARM 513
#define DEFAULT_HIGHALARM 705
#define DEFAULT_DAIRYINTERVAL 60
#define DEFAULT_DAIRYOUTPUTDIR "/var/www/html/Dairy"
#define DAIRYOUTPUTDIR_SDCARD "/var/www/html/sdcard"
#define DAIRYOUTPUTDIR_FLASHUSB "/var/www/html/flashusb"
#define ID_DAIRYOUTPUTDIR_EMMC "eMMC"
#define ID_DAIRYOUTPUTDIR_SDCARD "sdcard"
#define ID_DAIRYOUTPUTDIR_FLASHUSB "flashusb"
#define PARITY_ODD "ODD"
#define PARITY_EVEN "EVEN"
#define PARITY_NONE "NONE"
#define DATABIT_5 "5"
#define DATABIT_6 "6"
#define DATABIT_7 "7"
#define DATABIT_8 "8"
#define STOPBIT_1 "1"
#define STOPBIT_2 "2"
#define TEMP_OFFSET -5

#define ERRITEMS 9
#define ERRITEM_TIMEOUT 0
#define ERRITEM_11 1
#define ERRITEM_12 2
#define ERRITEM_13 3
#define ERRITEM_14 4
#define ERRITEM_15 5
#define ERRITEM_17 6
#define ERRITEM_PRE 7
#define ERRITEM_HIGH 8
#define ERRCNT_DEFAULT_TIMEOUT 5
#define ERRCNT_DEFAULT_STATUS 1

#define DEFAULT_SCAN_OVER_1S 13
#define DEFAULT_TREND_IDOL_1S 1
#define DEFAULT_TREND_IDOL_5S 1

#define DEFAULT_NETMODE "static"
#define DEFAULT_IPADDRESS "192.168.1.10"
#define DEFAULT_SUBNETMASK "255.255.255.0"
#define DEFAULT_GATEWAYADDRESS "192.168.1.1"
#define DEFAULT_DNSADDRESS "192.168.1.1"
#define DEFAULT_BROADCAST "192.168.1.255"

#define FILE_CURRENT_DATA "/tmp/currentData.dat"

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

struct RecvData {
	int mistLevel;		/* ミストレベル */
	int temp;	/* 温度 */
	char errorFlg[2];
	short errCnt[ERRITEMS];
};

struct TrendDataList {
	int avgMistLv[MAX_AVGRANGE];
	int sumMistLv;
	int avgTemp[MAX_AVGRANGE];
	int sumTemp;
	int setPos;
	int recvCnt;
	char errorFlg[2];
};

struct TrendTime {
	char time[10];
	char date[12];
};

struct CurrentData {
	int avgMistLv;
	int avgTemp;
	char errorFlg[2];
};


struct RecvData recvDataList[NODESIZE];
struct TrendDataList trendDataList[NODESIZE];
struct CurrentData currentDataList[NODESIZE];

struct RecvData recvData;

float trendTemp[NODESIZE][TRENDRANGE];
float trendMistLv[NODESIZE][TRENDRANGE];
int trendStatus[NODESIZE][TRENDRANGE];
struct TrendTime trendTime[TRENDRANGE];
int addrChar[NODESIZE] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
int enableAddr[NODESIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float factorA[NODESIZE] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float factorB[NODESIZE] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
char analogUnit[NODESIZE][BUFSIZE] = {"","","","","","","","","","","","","","","",""};
int preAlarm[NODESIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int highAlarm[NODESIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int dailyBufCnt = 0;
int isDebug=0;
char deviceName[BUFSIZE];
char baudrate[BUFSIZE];
char dataBit[BUFSIZE];
char stopBit[BUFSIZE];
char parity[BUFSIZE];
char mistUnit[BUFSIZE];
char tempUnit[BUFSIZE];
char dairyOutputDir[BUFSIZE];

int validAddrCnt = 0;

int dairyInterval;
int alarmFilter;
int timeoutFilter;
int targetTime[2] = {-1,-1};
int dairyCnt = 0;
int lineCnt;
int avgRange = MAX_AVGRANGE;

int scanOver1s = DEFAULT_SCAN_OVER_1S;
int trendIdol = DEFAULT_TREND_IDOL_1S;

char netMode[BUFSIZE];
char ipAddress[BUFSIZE];
char subnetMask[BUFSIZE];
char gatewayAddress[BUFSIZE];
char dnsAddress[BUFSIZE];
char broadcast[BUFSIZE];

void initData();
void writeCurrentData();
void makeHistoryData();
void MovingAverage(int addr);
int ReadIni();

//--------------------- 

int main(int argc,char *argv[]) {
    char bufWrite[BUFSIZE];                    /* バッファ */
    char bufRead[BUFSIZE];                    /* バッファ */
    char buf[BUFSIZE];                    /* バッファ */
	char data[BUFSIZE];
	char dataList[BUFSIZE];
	int validAddr[NODESIZE];
    int fd;                           /* ファイルディスクリプタ */
    struct termios oldtio, newtio;    /* シリアル通信設定 */
	int readIdx;
	int rts_ctl;
	int endFlg = 0;
	int recvFlg = 0;
	int recvSize = 0;
	int value = 0;
	int dataIdx = 0;
	int dataSize = 0;
	int addr = 0;
	int addrRange = NODESIZE;
	int idx=0;
	int flgNodeCheck = 1;
	int tempOffset = TEMP_OFFSET;
	int preMistLv[NODESIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

	time_t timeStart;
	time_t timeCurrent;
	long l;
    FILE *fp;
	char tmp[5];
	int printAddr = 0;

	int trendCnt = 0;
	int ret;
	char nodeName[NODESIZE][BUFSIZE];
	char systemCmd[BUFSIZE];
	char bccCode = 0x00;

	struct tm *alarmT_st;
	struct timeval alarmTime;
	FILE *fpAlarmLog;

	system("/var/www/html/bootMDSX.sh");
	
	strcpy(systemCmd,"rm ");
	strcat(systemCmd,FILE_CURRENT_DATA);
	system(systemCmd);

	printf("argc:%d\n",argc);
	if(argc==2){
		isDebug = atoi(argv[1]);
	}

	fp = fopen( "/var/www/html/nodeSetting.ini", "r" );
	if( fp != NULL ){
		fscanf( fp, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", &enableAddr[0], &enableAddr[1], &enableAddr[2], &enableAddr[3], &enableAddr[4], &enableAddr[5], &enableAddr[6], &enableAddr[7], &enableAddr[8], &enableAddr[9], &enableAddr[10], &enableAddr[11], &enableAddr[12], &enableAddr[13], &enableAddr[14], &enableAddr[15]);
		fclose( fp );
	}

	fp = fopen( "/var/www/html/nodeRange.ini", "r" );
	idx = 0;
	if( fp != NULL ){
		for(idx=0; idx<NODESIZE; idx++){
			fscanf( fp, "%[^,],%f,%f,%[^,],%d,%d\n", nodeName[idx], &factorB[idx], &factorA[idx],&analogUnit[idx],&preAlarm[idx],&highAlarm[idx] );
			if(enableAddr[idx]==0){
				factorA[idx] = 0.0;
				factorB[idx] = 0.0;
			}else{
				factorA[idx] = factorA[idx] / CNTMAX;
				validAddrCnt++;
			}
			printf( "%s %f %f %s %d %d \n", nodeName[idx], factorA[idx], factorB[idx],analogUnit[idx],preAlarm[idx],highAlarm[idx]  );
		}
		fclose( fp );
	}
	rts_ctl = TIOCM_RTS;

	if(validAddrCnt>=DEFAULT_SCAN_OVER_1S){
		trendIdol = DEFAULT_TREND_IDOL_5S;
	}

	if (isDebug) printf("validAddrCnt:%d,trendIdol:%d\n",validAddrCnt,trendIdol);

	initData();

	strcpy(systemCmd,"ifconfig eadd1 ");
	strcat(systemCmd,ipAddress);
	strcat(systemCmd," netmask ");
	strcat(systemCmd,subnetMask);
	strcat(systemCmd," broadcast ");
	strcat(systemCmd,broadcast);
	system(systemCmd);

	strcpy(systemCmd,"stty ");
	//[- ]parenb  出力にパリティビット (parity bit) を付加し、 入力にもパリティビットがあるものとする。
	//[- ]parodd  奇数パリティに設定する (- がつくと偶数)。
	if(strcmp(parity,PARITY_ODD)==0){
		strcat(systemCmd,"parenb parodd ");
	}else if(strcmp(parity,PARITY_EVEN)==0){
		strcat(systemCmd,"parenb -parodd ");
	}else{
		strcat(systemCmd,"-parenb ");
	}
	//"cs5 cs6 cs7 cs8"  文字のサイズをそれぞれ 5, 6, 7, 8 ビットに設定する。
	if(strcmp(dataBit,DATABIT_5)==0){
		strcat(systemCmd," cs5");
	}else if(strcmp(dataBit,DATABIT_6)==0){
		strcat(systemCmd," cs6");
	}else if(strcmp(dataBit,DATABIT_7)==0){
		strcat(systemCmd," cs7");
	}else{
		strcat(systemCmd," cs8");
	}
	//[- ]cstopb  文字あたり二つのストップビットを用いる (`-' がつくと一つ)。
	if(strcmp(stopBit,STOPBIT_2)==0){
		strcat(systemCmd," cstopb");
	}else{
		strcat(systemCmd," -cstopb");
	}
	strcat(systemCmd," -hupcl ignpar cread -opost -onlcr -icrnl clocal -isig -icanon -iexten -echoctl -echoke -echo -echoe -echok -crtscts ixany ospeed ");
	strcat(systemCmd,baudrate);
	strcat(systemCmd," ispeed 0 -F ");
	strcat(systemCmd,deviceName);
	if (isDebug) printf("%s\n",systemCmd);
	system(systemCmd);
	strcpy(systemCmd,"stty intr ^- quit ^- erase ^- kill ^- eof ^- eol ^- eol2 ^- swtch ^- start ^- stop ^- susp ^- rprnt ^- werase ^- lnext ^- -F ");
	strcat(systemCmd,deviceName);
	if (isDebug) printf("%s\n",systemCmd);
	system(systemCmd);
	
	dateTime.year = 0;
	dateTime.month = 0;
	dateTime.day = 0;
	dateTime.hour = 0;
	dateTime.min = 0;
	dateTime.sec = 0;

	//fd = open(SERIAL_PORT, O_RDWR);   /* デバイスをオープンする */
    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);   /* デバイスをオープンする */
    
    ioctl(fd, TCGETS, &oldtio);       /* 現在のシリアルポートの設定を待避させる */
    newtio = oldtio;                  /* ポートの設定をコピー */
    //newtio.c_cflag = ...;             /* ポートの設定をおこなう 詳細はtermios(3)参照 */
	newtio.c_oflag=0;               /* rawモード */
	newtio.c_lflag=0;               /* 非カノニカル入力 */
    ioctl(fd, TCSETS, &newtio);       /* ポートの設定を有効にする */

	for(idx=0;idx<NODESIZE;idx++){
		//validAddr[idx]=-1;
		validAddr[idx]=idx; //起動時のnodeチェックを無効化
	}
	flgNodeCheck=0; //起動時のnodeチェックを無効化
	
	gettimeofday(&alarmTime, NULL);
	alarmT_st = localtime(&(alarmTime.tv_sec));
	fpAlarmLog = fopen("/tmp/alarm.log","a");
	if(fpAlarmLog!=NULL){
		fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,--,Comm Start.\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec);
		fclose(fpAlarmLog);
	}

	do{
		if(((flgNodeCheck==1) || ((flgNodeCheck==0) && (validAddr[addr]>-1))) && (enableAddr[addr]==1)){
			time(&timeStart);
			time(&timeCurrent);
			for(readIdx=0;readIdx<BUFSIZE;readIdx++){
				bufRead[readIdx] = 0x00;
				data[readIdx] = 0;
			}

			bufWrite[6] = 0;
			bufWrite[0] = 0x02;
			bufWrite[1] = addrChar[addr]; bufWrite[6] ^= bufWrite[1];
			bufWrite[2] = WRITECMD;       bufWrite[6] ^= bufWrite[2];
			bufWrite[3] = 0x30;           bufWrite[6] ^= bufWrite[3];
			bufWrite[4] = 0x30;           bufWrite[6] ^= bufWrite[4];
			bufWrite[5] = 0x03;           bufWrite[6] ^= bufWrite[5];

			if (isDebug) printf("write(%d)",addr);
			write(fd, bufWrite, 7);
			if (isDebug) printf("[%02X %02X %02X %02X %02X %02X %02X]\n", bufWrite[0], bufWrite[1], bufWrite[2], bufWrite[3], bufWrite[4], bufWrite[5], bufWrite[6]);
			readIdx = 0;
			dataIdx = 0;

			recvFlg = 1;
			
			if (isDebug) printf("read[");
			do{
				time(&timeCurrent);
				
				if(difftime(timeCurrent,timeStart)>1){
					recvFlg=0;
					if(recvDataList[addr].errCnt[ERRITEM_TIMEOUT]<SHRT_MAX) recvDataList[addr].errCnt[ERRITEM_TIMEOUT]++;
					if (isDebug) printf("ERRITEM_TIMEOUT:%d",recvDataList[addr].errCnt[ERRITEM_TIMEOUT]);
					if(recvDataList[addr].errCnt[ERRITEM_TIMEOUT] == timeoutFilter){
						gettimeofday(&alarmTime, NULL);
						alarmT_st = localtime(&(alarmTime.tv_sec));
						recvDataList[addr].errorFlg[1] = recvDataList[addr].errorFlg[1] | 0x80;
						trendDataList[addr].errorFlg[1] = recvDataList[addr].errorFlg[1];
						fpAlarmLog = fopen("/tmp/alarm.log","a");
						if(fpAlarmLog!=NULL){
							fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,Timeout Error.\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
							if (isDebug) printf("Timeout Error:%d",recvDataList[addr].errCnt[ERRITEM_TIMEOUT]);
							fclose(fpAlarmLog);
						}
					}
				}
				if(read(fd, buf, 1)>0){
					bufRead[readIdx]=buf[0];
					if (isDebug) printf("%02X ", bufRead[readIdx]);
					if(recvFlg==2){
						value = bufRead[readIdx];
						data[dataIdx++] = value;
						bccCode ^= value;
					}
					if((recvFlg==3)){
						if (isDebug) printf("bcc[%02X] ", bccCode);
						if(bccCode != bufRead[readIdx]){
							gettimeofday(&alarmTime, NULL);
							alarmT_st = localtime(&(alarmTime.tv_sec));
							recvDataList[addr].errorFlg[1] = recvDataList[addr].errorFlg[1] | 0x80;
							trendDataList[addr].errorFlg[1] = recvDataList[addr].errorFlg[1];
							fpAlarmLog = fopen("/tmp/alarm.log","a");
							if(fpAlarmLog!=NULL){
								fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,BCC Check Error.\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
								fclose(fpAlarmLog);
							}
						}else{
							recvDataList[addr].errorFlg[1] = recvDataList[addr].errorFlg[1] & 0x7F;
							recvDataList[addr].mistLevel = (data[2] & 0x0F)*1000 + (data[3] & 0x0F)*100 + (data[4] & 0x0F)*10 + (data[5] & 0x0F);
							recvDataList[addr].temp = ((data[8] & 0x0F)*10) + (data[9] & 0x0F) + tempOffset;
							recvDataList[addr].errorFlg[0] = ((data[6] & 0x0F)*16) + (data[7] & 0x0F);
							recvDataList[addr].errCnt[ERRITEM_TIMEOUT]=0;

							if((recvDataList[addr].mistLevel>=highAlarm[addr])){
								if(recvDataList[addr].errCnt[ERRITEM_HIGH]<SHRT_MAX) recvDataList[addr].errCnt[ERRITEM_HIGH]++;
								if (isDebug) printf("ERRITEM_HIGH:%d,alarmFileter:%d\n",recvDataList[addr].errCnt[ERRITEM_HIGH],alarmFilter);
								if(recvDataList[addr].errCnt[ERRITEM_HIGH] == alarmFilter){
									recvDataList[addr].errorFlg[1] = recvDataList[addr].errorFlg[1] | 0x01;
									fpAlarmLog = fopen("/tmp/alarm.log","a");
									if(fpAlarmLog!=NULL){
										gettimeofday(&alarmTime, NULL);
										alarmT_st = localtime(&(alarmTime.tv_sec));
										fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,High-Alarm [%d]\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1,recvDataList[addr].mistLevel);
										if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,High-Alarm [%d]\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1,recvDataList[addr].mistLevel);
										fclose(fpAlarmLog);
									}
								}
							}else if((preMistLv[addr]>=highAlarm[addr])){
								recvDataList[addr].errorFlg[1] = recvDataList[addr].errorFlg[1] & 0xFE;
								fpAlarmLog = fopen("/tmp/alarm.log","a");
								if(fpAlarmLog!=NULL){
									gettimeofday(&alarmTime, NULL);
									alarmT_st = localtime(&(alarmTime.tv_sec));
									fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,High-Alarm RETURN [%d]\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1,recvDataList[addr].mistLevel);
									if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,High-Alarm RETURN [%d]\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1,recvDataList[addr].mistLevel);
									fclose(fpAlarmLog);
								}
								recvDataList[addr].errCnt[ERRITEM_HIGH] = 0;
							}
							if((recvDataList[addr].mistLevel>=preAlarm[addr])){
								if(recvDataList[addr].errCnt[ERRITEM_PRE]<SHRT_MAX) recvDataList[addr].errCnt[ERRITEM_PRE]++;
								if (isDebug) printf("ERRITEM_PRE:%d,alarmFileter:%d\n",recvDataList[addr].errCnt[ERRITEM_PRE],alarmFilter);
								if(recvDataList[addr].errCnt[ERRITEM_PRE] == alarmFilter){
									recvDataList[addr].errorFlg[1] = recvDataList[addr].errorFlg[1] | 0x02;
									fpAlarmLog = fopen("/tmp/alarm.log","a");
									if(fpAlarmLog!=NULL){
										gettimeofday(&alarmTime, NULL);
										alarmT_st = localtime(&(alarmTime.tv_sec));
										fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,Pre-Alarm [%d]\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1,recvDataList[addr].mistLevel);
										if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,Pre-Alarm [%d]\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1,recvDataList[addr].mistLevel);
										fclose(fpAlarmLog);
									}
								}
							}else if((preMistLv[addr]>=preAlarm[addr])){
								recvDataList[addr].errorFlg[1] = recvDataList[addr].errorFlg[1] & 0xFD;
								fpAlarmLog = fopen("/tmp/alarm.log","a");
								if(fpAlarmLog!=NULL){
									gettimeofday(&alarmTime, NULL);
									alarmT_st = localtime(&(alarmTime.tv_sec));
									fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,Pre-Alarm RETURN [%d]\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1,recvDataList[addr].mistLevel);
									if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,Pre-Alarm RETURN [%d]\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1,recvDataList[addr].mistLevel);
									fclose(fpAlarmLog);
								}
								recvDataList[addr].errCnt[ERRITEM_PRE] = 0;
							}

							preMistLv[addr] = recvDataList[addr].mistLevel;
							if(flgNodeCheck==0){
								MovingAverage(addr);
							}
						}
						recvFlg=0;
					}

					if((recvFlg==2) && (bufRead[readIdx]==0x03)) {
						recvFlg=3;
					}
					if((recvFlg==1) && (bufRead[readIdx]==0x02)) {
						bccCode = 0x00;
						recvFlg=2;
					}
					readIdx++;
					if(difftime(timeCurrent,timeStart)>1){
						recvFlg=0;
					}
				}
			}while(recvFlg!=0);
			if (isDebug) printf("]\n");

			if(flgNodeCheck==1){
				if(readIdx==13){
					validAddr[addr]=addr;
				}
			} else {
				dataSize=dataIdx;
				strcat(dataList,data);
				strcat(dataList,"\n");
			}
		}

		if((flgNodeCheck==0)){
			writeCurrentData();

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
		}

		addr++;
		if(addr>=addrRange){

			for(idx=0;idx<BUFSIZE;idx++){
				dataList[idx]=0x00;
			}

			addr=0;
			flgNodeCheck=0;
			//sleep(1);
		}
	} while(endFlg==0);
	
    ioctl(fd, TCSETS, &oldtio);       /* ポートの設定を元に戻す */
    close(fd);                        /* デバイスのクローズ */

    return 0;
}

void initData(){
	int idx;
	int printAddr;

	for(printAddr=0;printAddr<NODESIZE;printAddr++){
		recvDataList[printAddr].mistLevel = 0;
		recvDataList[printAddr].temp = 0;
		recvDataList[printAddr].errorFlg[0] = 0;
		for(idx=0; idx<ERRITEMS; idx++){
			recvDataList[printAddr].errCnt[idx] = 0;
		}
		
		trendDataList[printAddr].sumMistLv = 0;
		trendDataList[printAddr].sumTemp = 0;
		trendDataList[printAddr].setPos = 0;
		trendDataList[printAddr].recvCnt = 0;
		for(idx=0; idx<avgRange; idx++){
			trendDataList[printAddr].avgMistLv[idx] = 0;
			trendDataList[printAddr].avgTemp[idx] = 0;
		}
		currentDataList[printAddr].avgMistLv = 0;
		currentDataList[printAddr].avgTemp = 0;
		strcpy(currentDataList[printAddr].errorFlg,"00");
	}
	for(idx=0; idx<TRENDRANGE; idx++){
		strcpy(trendTime[idx].date,"");
		strcpy(trendTime[idx].time,"");
		trendTemp[0][idx] = 0.0;
		trendTemp[1][idx] = 0.0;
		trendTemp[2][idx] = 0.0;
		trendTemp[3][idx] = 0.0;
		trendTemp[4][idx] = 0.0;
		trendTemp[5][idx] = 0.0;
		trendTemp[6][idx] = 0.0;
		trendTemp[7][idx] = 0.0;
		trendTemp[8][idx] = 0.0;
		trendTemp[9][idx] = 0.0;
		trendTemp[10][idx] = 0.0;
		trendTemp[11][idx] = 0.0;
		trendTemp[12][idx] = 0.0;
		trendTemp[13][idx] = 0.0;
		trendTemp[14][idx] = 0.0;
		trendTemp[15][idx] = 0.0;
		trendMistLv[0][idx] = 0.0;
		trendMistLv[1][idx] = 0.0;
		trendMistLv[2][idx] = 0.0;
		trendMistLv[3][idx] = 0.0;
		trendMistLv[4][idx] = 0.0;
		trendMistLv[5][idx] = 0.0;
		trendMistLv[6][idx] = 0.0;
		trendMistLv[7][idx] = 0.0;
		trendMistLv[8][idx] = 0.0;
		trendMistLv[9][idx] = 0.0;
		trendMistLv[10][idx] = 0.0;
		trendMistLv[11][idx] = 0.0;
		trendMistLv[12][idx] = 0.0;
		trendMistLv[13][idx] = 0.0;
		trendMistLv[14][idx] = 0.0;
		trendMistLv[15][idx] = 0.0;
		trendStatus[0][idx] = 0;
		trendStatus[1][idx] = 0;
		trendStatus[2][idx] = 0;
		trendStatus[3][idx] = 0;
		trendStatus[4][idx] = 0;
		trendStatus[5][idx] = 0;
		trendStatus[6][idx] = 0;
		trendStatus[7][idx] = 0;
		trendStatus[8][idx] = 0;
		trendStatus[9][idx] = 0;
		trendStatus[10][idx] = 0;
		trendStatus[11][idx] = 0;
		trendStatus[12][idx] = 0;
		trendStatus[13][idx] = 0;
		trendStatus[14][idx] = 0;
		trendStatus[15][idx] = 0;
	}

	strcpy(deviceName,DEFAULT_DEVICENAME);
	strcpy(baudrate,DEFAULT_BAUDRATE);
	strcpy(dataBit,DEFAULT_DATABIT);
	strcpy(stopBit,DEFAULT_STOPBIT);
	strcpy(parity,DEFAULT_PARITY);
	strcpy(mistUnit,DEFAULT_MISTUNIT);
	strcpy(tempUnit,DEFAULT_TEMPUNIT);
	strcpy(dairyOutputDir,DEFAULT_DAIRYOUTPUTDIR);
	dairyInterval = DEFAULT_DAIRYINTERVAL;
	alarmFilter = ERRCNT_DEFAULT_STATUS;
	timeoutFilter = ERRCNT_DEFAULT_TIMEOUT;

	strcpy(netMode,DEFAULT_NETMODE);
	strcpy(ipAddress,DEFAULT_IPADDRESS);
	strcpy(subnetMask,DEFAULT_SUBNETMASK);
	strcpy(gatewayAddress,DEFAULT_GATEWAYADDRESS);
	strcpy(dnsAddress,DEFAULT_DNSADDRESS);
	strcpy(broadcast,DEFAULT_BROADCAST);

	ReadIni();
}

int ReadIni()
{
	FILE *fp;
	char line[BUFSIZE];
	char stemp[BUFSIZE];
	char *paramList[2];
	int cnt;
	const char *delim = "=";
	char *p;

	if((fp = fopen("/var/www/html/mdsxConfig.ini", "rt")) == NULL){
		printf("mdsxConfig.ini cannot open.\n");
		return 0;
	}

	while(fgets(line, BUFSIZE, fp) != NULL){
		p = strchr(line, '\n');
		if(p != NULL) *p = '\0';
		strcpy(stemp,line);
		if(strcmp(stemp,"=")<0){
			continue;
		}

		paramList[0] = strtok(stemp, delim);
		paramList[1] = strtok(NULL, delim);

		if (isDebug) printf("paramList:%s,%s\n",paramList[0],paramList[1]);
		if(strcmp(paramList[0],LABEL_DEVICENAME)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_DEVICENAME,paramList[1]);
			strcpy(deviceName,paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_BAUDRATE)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_BAUDRATE,paramList[1]);
			strcpy(baudrate,paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_DATABIT)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_DATABIT,paramList[1]);
			strcpy(dataBit,paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_STOPBIT)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_STOPBIT,paramList[1]);
			strcpy(stopBit,paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_PARITY)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_PARITY,paramList[1]);
			strcpy(parity,paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_MISTUNIT)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_MISTUNIT,paramList[1]);
			strcpy(mistUnit,paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_TEMPUNIT)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_TEMPUNIT,paramList[1]);
			strcpy(tempUnit,paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_DAIRYOUTPUTDIR)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_DAIRYOUTPUTDIR,paramList[1]);
			if(strcmp(paramList[1],ID_DAIRYOUTPUTDIR_SDCARD)==0){
				strcpy(dairyOutputDir,DAIRYOUTPUTDIR_SDCARD);
			}else if(strcmp(paramList[1],ID_DAIRYOUTPUTDIR_FLASHUSB)==0){
				strcpy(dairyOutputDir,DAIRYOUTPUTDIR_FLASHUSB);
			}else{
				strcpy(dairyOutputDir,DEFAULT_DAIRYOUTPUTDIR);
			}
			if (isDebug) printf("dairyOutputDir:%s\n",dairyOutputDir);
		}
		if(strcmp(paramList[0],LABEL_AVGRANGE)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_AVGRANGE,paramList[1]);
			avgRange = atoi(paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_DAIRYINTERVAL)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_DAIRYINTERVAL,paramList[1]);
			dairyInterval = atoi(paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_ALARMFILTER)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_ALARMFILTER,paramList[1]);
			alarmFilter = atoi(paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_TIMEOUTFILTER)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_TIMEOUTFILTER,paramList[1]);
			timeoutFilter = atoi(paramList[1]);
		}

		if(strcmp(paramList[0],LABEL_NETMODE)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_NETMODE,paramList[1]);
			strcpy(netMode,paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_IPADDRESS)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_IPADDRESS,paramList[1]);
			strcpy(ipAddress,paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_SUBNETMASK)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_SUBNETMASK,paramList[1]);
			strcpy(subnetMask,paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_GATEWAYADDRESS)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_GATEWAYADDRESS,paramList[1]);
			strcpy(gatewayAddress,paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_DNSADDRESS)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_DNSADDRESS,paramList[1]);
			strcpy(dnsAddress,paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_BROADCAST)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_BROADCAST,paramList[1]);
			strcpy(broadcast,paramList[1]);
		}
	}

	fclose(fp);

	return 1;
}


void MovingAverage(int addr){
	struct tm *alarmT_st;
	struct timeval alarmTime;
	FILE *fpAlarmLog;
	char errVal;
	char errDiff;

	errDiff = trendDataList[addr].errorFlg[0] ^ recvDataList[addr].errorFlg[0];
	errVal = 0xF3 & recvDataList[addr].errorFlg[0];
	if(((errDiff & 0xF3) > 0) || ((errVal & 0xF3) > 0)){
		/*
		Err code1	
		0	:Nomal	
		1	:The abnormalities in thermistor temperature ERRITEM_11
		2	:The abnormalities in thermistor circuit ERRITEM_12
		4	:EEROM error ERRITEM_13
		8	:The abnormalities in a circuit of sensor unit ERRITEM_14

		Err code2
		0	:Nomal
		1	:Arithmetic operation error ERRITEM_15
		2	:Clearning the sensor ERRITEM_17
		4	:None
		8	:None
		*/
		
		if (isDebug) printf("PreFlg:%02X CurFlg:%02X errVal:%02X errDiff:%02X\n",trendDataList[addr].errorFlg[0], recvDataList[addr].errorFlg[0], errVal, errDiff);
		gettimeofday(&alarmTime, NULL);
		alarmT_st = localtime(&(alarmTime.tv_sec));
		fpAlarmLog = fopen("/tmp/alarm.log","a");
		if(fpAlarmLog!=NULL){
			if((errVal & 0x10)==0x10){
				if(recvDataList[addr].errCnt[ERRITEM_11]<SHRT_MAX) recvDataList[addr].errCnt[ERRITEM_11]++;
				if(recvDataList[addr].errCnt[ERRITEM_11] == alarmFilter){
					fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 11\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
					if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 11\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				}
			}else if(((errDiff & 0x10)==0x10) && (recvDataList[addr].errCnt[ERRITEM_11]>=alarmFilter)){
				fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 11 RETURN\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 11 RETURN\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				recvDataList[addr].errCnt[ERRITEM_11] = 0;
			}

			if((errVal & 0x20)==0x20){
				if(recvDataList[addr].errCnt[ERRITEM_12]<SHRT_MAX) recvDataList[addr].errCnt[ERRITEM_12]++;
				if(recvDataList[addr].errCnt[ERRITEM_12] == alarmFilter){
					fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 12\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
					if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 12\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				}
			}else if(((errDiff & 0x20)==0x20) && (recvDataList[addr].errCnt[ERRITEM_12]>=alarmFilter)){
				fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 12 RETURN\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 12 RETURN\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				recvDataList[addr].errCnt[ERRITEM_12] = 0;
			}

			if((errVal & 0x40)==0x40){
				if(recvDataList[addr].errCnt[ERRITEM_13]<SHRT_MAX) recvDataList[addr].errCnt[ERRITEM_13]++;
				if(recvDataList[addr].errCnt[ERRITEM_13] == alarmFilter){
					fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 13\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
					if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 13\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				}
			}else if(((errDiff & 0x40)==0x40) && (recvDataList[addr].errCnt[ERRITEM_13]>=alarmFilter)){
				fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 13 RETURN\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 13 RETURN\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				recvDataList[addr].errCnt[ERRITEM_13] = 0;
			}

			if((errVal & 0x80)==0x80){
				if(recvDataList[addr].errCnt[ERRITEM_14]<SHRT_MAX) recvDataList[addr].errCnt[ERRITEM_14]++;
				if(recvDataList[addr].errCnt[ERRITEM_14] == alarmFilter){
					fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 14\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
					if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 14\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				}
			}else if(((errDiff & 0x80)==0x80) && (recvDataList[addr].errCnt[ERRITEM_14]>=alarmFilter)){
				fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 14 RETURN\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 14 RETURN\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				recvDataList[addr].errCnt[ERRITEM_14] = 0;
			}

			if((errVal & 0x01)==0x01){
				if(recvDataList[addr].errCnt[ERRITEM_15]<SHRT_MAX) recvDataList[addr].errCnt[ERRITEM_15]++;
				if(recvDataList[addr].errCnt[ERRITEM_15] == alarmFilter){
					fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 15\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
					if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 15\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				}
			}else if(((errDiff & 0x01)==0x01) && (recvDataList[addr].errCnt[ERRITEM_15]>=alarmFilter)){
				fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 15 RETURN\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,SENSOR FAIL 15 RETURN\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				recvDataList[addr].errCnt[ERRITEM_15] = 0;
			}

			if((errVal & 0x02)==0x02){
				if(recvDataList[addr].errCnt[ERRITEM_17]<SHRT_MAX) recvDataList[addr].errCnt[ERRITEM_17]++;
				if(recvDataList[addr].errCnt[ERRITEM_17] == alarmFilter){
					fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,CLEANING THE SENSOR 17\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
					if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,CLEANING THE SENSOR 17\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				}
			}else if(((errDiff & 0x02)==0x02) && (recvDataList[addr].errCnt[ERRITEM_17]>=alarmFilter)){
				fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,A%d,CLEANING THE SENSOR 17 RETURN\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d,A%d,CLEANING THE SENSOR 17 RETURN\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec,addr+1);
				recvDataList[addr].errCnt[ERRITEM_17] = 0;
			}
			fclose(fpAlarmLog);
		}
		
	}
	trendDataList[addr].errorFlg[0] = recvDataList[addr].errorFlg[0];
	currentDataList[addr].errorFlg[0] = trendDataList[addr].errorFlg[0];
	trendDataList[addr].errorFlg[1] = recvDataList[addr].errorFlg[1];
	currentDataList[addr].errorFlg[1] = trendDataList[addr].errorFlg[1];
	trendDataList[addr].sumMistLv -= trendDataList[addr].avgMistLv[trendDataList[addr].setPos];
	trendDataList[addr].avgMistLv[trendDataList[addr].setPos] = recvDataList[addr].mistLevel;
	currentDataList[addr].avgMistLv = recvDataList[addr].mistLevel;
	trendDataList[addr].sumMistLv += trendDataList[addr].avgMistLv[trendDataList[addr].setPos];
	trendDataList[addr].sumTemp -= trendDataList[addr].avgTemp[trendDataList[addr].setPos];
	trendDataList[addr].avgTemp[trendDataList[addr].setPos] = recvDataList[addr].temp;
	currentDataList[addr].avgTemp = recvDataList[addr].temp;
	trendDataList[addr].sumTemp += trendDataList[addr].avgTemp[trendDataList[addr].setPos];
	trendDataList[addr].setPos++;
	if(trendDataList[addr].setPos >= avgRange){
		trendDataList[addr].setPos=0;
	}
	trendDataList[addr].recvCnt++;
	if(trendDataList[addr].recvCnt >= avgRange){
		trendDataList[addr].recvCnt=avgRange;
	}
}

void writeCurrentData(){
	int printAddr = 0;
	int idx = 0;
	FILE *fp;

	fp = fopen(FILE_CURRENT_DATA,"wb");
	if(fp==NULL){
		//失敗と表示し終了
		printf("memo.txt Open Failed\n");
		return;
	}
	fwrite(currentDataList,sizeof(currentDataList),1,fp);
	fclose(fp);
}

