
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

#include <mosquitto.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

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

char topic[BUFSIZE];
char message[BUFSIZE];
int connect_desire = TRUE;
int is_debug = FALSE;

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


struct RecvData recvDataList[NODESIZE];
struct TrendDataList trendDataList[NODESIZE];

struct RecvData recvData;

float lvPreAlm = DEFAULT_PREALARM;
float lvHighAlm = DEFAULT_HIGHALARM;

float trendTemp[NODESIZE][TRENDRANGE];
float trendMistLv[NODESIZE][TRENDRANGE];
int trendStatus[NODESIZE][TRENDRANGE];
struct TrendTime trendTime[TRENDRANGE];
int addrChar[NODESIZE] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
int enableAddr[NODESIZE] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float factorA[NODESIZE] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
float factorB[NODESIZE] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
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

int preAlarm;
int highAlarm;
int dairyInterval;
int alarmFilter;
int timeoutFilter;
int targetTime[2] = {-1,-1};
int dairyCnt = 0;
int lineCnt;
int avgRange = MAX_AVGRANGE;

int scanOver1s = DEFAULT_SCAN_OVER_1S;
int trendIdol = DEFAULT_TREND_IDOL_1S;

struct mosquitto *mosq = NULL;
char *host          = "192.168.10.125";
int   port          = 1883;
int   keepalive     = 60;

void initData();
void writeCurrentTempGraph();
void writeCurrentMistLvGraph();
void writeTrendTempGraph();
void writeTrendMistLvGraph();
void writeCurrentData();
void makeHistoryData();
void writeDailyTrendData();
void MovingAverage(int addr);
int ReadIni();
int split( char *str, const char *delim, char *outlist[] );
int roundUp(float n);
int roundDown(float n);
int roundOff(float n);
float checkDiskSize();

//--------------------- 
/**
 * Brokerとの接続成功時に実行されるcallback関数
 */
void on_connect(struct mosquitto *mosq, void *obj, int result)
{
    if (isDebug)  printf("%s(%d)\n", __FUNCTION__, __LINE__);
    mosquitto_publish(mosq, NULL, topic, strlen(message), message, 0, false);
}

/**
 * Brokerとの接続を切断した時に実行されるcallback関数
 */
void on_disconnect(struct mosquitto *mosq, void *obj, int rc)
{
    if (isDebug)  printf("%s(%d)\n", __FUNCTION__, __LINE__);
}

/**
 * BrokerにMQTTメッセージ送信後に実行されるcallback関数
 */
static void on_publish(struct mosquitto *mosq, void *userdata, int mid)
{
    if (isDebug)  printf("%s(%d)\n", __FUNCTION__, __LINE__);
    connect_desire = FALSE;
    mosquitto_disconnect(mosq);
}

int mqttStart(){
	int ret = 0;
    int   cmdopt        = 0;
    char *id            = "mqtt/pub";
    char *cafile        = NULL;
    char *certfile      = NULL;
    char *keyfile       = NULL;
    bool  clean_session = true;

	mosquitto_lib_init();
    mosq = mosquitto_new(id, clean_session, NULL);
    if(!mosq){
        fprintf(stderr, "Cannot create mosquitto object\n");
        mosquitto_lib_cleanup();
        return(EXIT_FAILURE);
    }
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_disconnect_callback_set(mosq, on_disconnect);
    mosquitto_publish_callback_set(mosq, on_publish);

    if(cafile != NULL) {
        ret = mosquitto_tls_set(mosq, cafile, NULL, certfile, keyfile, NULL);
        if(ret != MOSQ_ERR_SUCCESS) {
            printf("mosquitto_tls_set function is failed.\n");
        }
        mosquitto_tls_insecure_set(mosq, true);
    }
	return ret;
}

int mqttPublisher(){
	int   ret = 0;
	connect_desire = TRUE;

	if (isDebug) printf("mqttPublisher %s/%s\n",topic,message);
	if(mosquitto_connect(mosq, host, port, keepalive)){
        printf("failed to connect broker.\n");
        mosquitto_lib_cleanup();
        return(EXIT_FAILURE);
    }

    do {
        ret = mosquitto_loop(mosq, 100, 1);
		if (isDebug) printf("mosquitto_loop %d\n",ret);
    } while((ret == MOSQ_ERR_SUCCESS) && (connect_desire != FALSE));
	return ret;
}

void mqttStop(){

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}

//-------------------------

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
			fscanf( fp, "%[^,],%f,%f\n", nodeName[idx], &factorB[idx], &factorA[idx] );
			if(enableAddr[idx]==0){
				factorA[idx] = 0.0;
				factorB[idx] = 0.0;
			}else{
				factorA[idx] = factorA[idx] / CNTMAX;
				validAddrCnt++;
			}
			printf( "%s %f %f\n", nodeName[idx], factorA[idx], factorB[idx] );
		}
		fclose( fp );
	}
	rts_ctl = TIOCM_RTS;

	if(validAddrCnt>=DEFAULT_SCAN_OVER_1S){
		trendIdol = DEFAULT_TREND_IDOL_5S;
	}

	if (isDebug) printf("validAddrCnt:%d,trendIdol:%d\n",validAddrCnt,trendIdol);

	initData();

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

	mqttStart();

	
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

							if((recvDataList[addr].mistLevel>=highAlarm)){
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
							}else if((preMistLv[addr]>=highAlarm)){
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
							if((recvDataList[addr].mistLevel>=preAlarm)){
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
							}else if((preMistLv[addr]>=preAlarm)){
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
			/* 時刻取得 */
			// 現在時刻を取得してmyTimeに格納．通常のtime_t構造体とsuseconds_tに値が代入される
			gettimeofday(&myTime, NULL);
			// time_t構造体を現地時間でのtm構造体に変換
			time_st = localtime(&(myTime.tv_sec));

			//printf("%d,%d,%d\n", dateTime.year,dateTime.sec,time_st->tm_sec);
			if((dateTime.year > 0) && 
				(
					(dateTime.sec < time_st->tm_sec) 
					|| (dateTime.min < time_st->tm_min)
					|| (dateTime.hour < time_st->tm_hour)
					|| (dateTime.day < time_st->tm_mday)
					|| (dateTime.month < time_st->tm_mon+1)
					|| (dateTime.year < time_st->tm_year+1900)
				)) {
				if((time_st->tm_sec % trendIdol) == 0){
					if (isDebug) printf("%04d/%02d/%02d %02d:%02d:%02d\n",time_st->tm_year+1900,time_st->tm_mon+1,time_st->tm_mday,time_st->tm_hour,time_st->tm_min,time_st->tm_sec);
					makeHistoryData();
					writeCurrentData();
					writeCurrentTempGraph();
					writeTrendTempGraph();
					writeCurrentMistLvGraph();
					writeTrendMistLvGraph();
					writeDailyTrendData();
				}
			}

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
	
	mqttStop();
	
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
	preAlarm = DEFAULT_PREALARM;
	highAlarm = DEFAULT_HIGHALARM;
	dairyInterval = DEFAULT_DAIRYINTERVAL;
	alarmFilter = ERRCNT_DEFAULT_STATUS;
	timeoutFilter = ERRCNT_DEFAULT_TIMEOUT;
	ReadIni();
}

int split( char *str, const char *delim, char *outlist[] ) {
    char    *tk;
    int     cnt = 0;

    tk = strtok( str, delim );
    while( tk != NULL && cnt < 2 ) {
        outlist[cnt++] = tk;
        tk = strtok( NULL, delim );
    }
    return cnt;
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
		if(strcmp(paramList[0],LABEL_PREALARM)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_PREALARM,paramList[1]);
			preAlarm = atoi(paramList[1]);
		}
		if(strcmp(paramList[0],LABEL_HIGHALARM)==0){
			if (isDebug) printf("SetParam:%s,%s\n",LABEL_HIGHALARM,paramList[1]);
			highAlarm = atoi(paramList[1]);
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
	trendDataList[addr].errorFlg[1] = recvDataList[addr].errorFlg[1];
	trendDataList[addr].sumMistLv -= trendDataList[addr].avgMistLv[trendDataList[addr].setPos];
	trendDataList[addr].avgMistLv[trendDataList[addr].setPos] = recvDataList[addr].mistLevel;
	trendDataList[addr].sumMistLv += trendDataList[addr].avgMistLv[trendDataList[addr].setPos];
	trendDataList[addr].sumTemp -= trendDataList[addr].avgTemp[trendDataList[addr].setPos];
	trendDataList[addr].avgTemp[trendDataList[addr].setPos] = recvDataList[addr].temp;
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
	FILE *fp = fopen("/tmp/currentDataTable.php","w");
	if(fp==NULL){
		//失敗と表示し終了
		printf("memo.txt Open Failed\n");
		return;
	}

	fprintf(fp,"<html><head><title>Current Data</title></head><body><H4>%s %s</H4><table border=\"1\" width=\"90%\"><tr>",trendTime[0].date,trendTime[0].time);
	fprintf(fp,"<th width=\"7%\">NODE</th><th width=\"9%\">MistLv</th><th width=\"9%\">Temp.</th>");
	fprintf(fp,"<th width=\"7%\">NODE</th><th width=\"9%\">MistLv</th><th width=\"9%\">Temp.</th>");
	fprintf(fp,"<th width=\"7%\">NODE</th><th width=\"9%\">MistLv</th><th width=\"9%\">Temp.</th>");
	fprintf(fp,"<th width=\"7%\">NODE</th><th width=\"9%\">MistLv</th><th width=\"9%\">Temp.</th>");
	fprintf(fp,"</tr><tr>");

	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"</tr><tr>");
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"</tr><tr>");
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"</tr><tr>");
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"<td>A%d</td><td align=\"right\">%4.0f</td><td align=\"right\">%3.0f</td>",printAddr+1,trendMistLv[printAddr][0],trendTemp[printAddr++][0]);
	fprintf(fp,"</tr></table></body></html>");

	fclose(fp);

	fp = fopen("/tmp/currentData.dat","wb");
	if(fp==NULL){
		//失敗と表示し終了
		printf("memo.txt Open Failed\n");
		return;
	}
	fwrite(trendMistLv,sizeof(trendMistLv),1,fp);
	fwrite(trendTemp,sizeof(trendTemp),1,fp);
	fclose(fp);
}
void writeCurrentTempGraph(){
	FILE *fpCurrentData;
	//currentData
	fpCurrentData = fopen("/tmp/currentTemp.txt","w");
	if(fpCurrentData==NULL){
		//失敗と表示し終了
		printf("currentTemp.txt Open Failed\n");
		return;
	}
	fprintf(fpCurrentData,"datasets: [{label: \"Normal\",borderWidth:1,backgroundColor: \"#00ff7f\",borderColor: \"#00ff7f\",\n");
	fprintf(fpCurrentData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[0][0],
		trendTemp[1][0],
		trendTemp[2][0],
		trendTemp[3][0],
		trendTemp[4][0],
		trendTemp[5][0],
		trendTemp[6][0],
		trendTemp[7][0],
		trendTemp[8][0],
		trendTemp[9][0],
		trendTemp[10][0],
		trendTemp[11][0],
		trendTemp[12][0],
		trendTemp[13][0],
		trendTemp[14][0],
		trendTemp[15][0]
	);
	fprintf(fpCurrentData,"}]\n");
	fclose(fpCurrentData);
	system("cat /var/www/html/currentGraph_top.txt /tmp/currentTemp.txt /var/www/html/currentGraph_bottom.txt > /tmp/currentGraph.php");
}
void writeTrendTempGraph(){
	FILE *fpTrendData;
	int prtIdx;
	int prtAddr;

	fpTrendData = fopen("/tmp/trendData.txt","w");
	if(fpTrendData==NULL){
		//失敗と表示し終了
		printf("trendData.txt Open Failed\n");
		return;
	}
	fprintf(fpTrendData,"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"><HTML><HEAD><title>Chart.js TEST</title></HEAD><body><div style=\"width: 100%\"><canvas id=\"chart\" height=\"400\" width=\"400\"></canvas></div><script src=\"Chart.min.js\"></script><script>\n");
	fprintf(fpTrendData,"var lineChartData = {\n");

	fprintf(fpTrendData,"labels : [\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",],",
		trendTime[119].time,"","","","",trendTime[114].time,"","","","",
		trendTime[109].time,"","","","",trendTime[104].time,"","","","",
		trendTime[99].time, "","","","",trendTime[94].time, "","","","",
		trendTime[89].time, "","","","",trendTime[84].time, "","","","",
		trendTime[79].time, "","","","",trendTime[74].time, "","","","",
		trendTime[69].time, "","","","",trendTime[64].time, "","","","",
		trendTime[59].time, "","","","",trendTime[54].time, "","","","",
		trendTime[49].time, "","","","",trendTime[44].time, "","","","",
		trendTime[39].time, "","","","",trendTime[34].time, "","","","",
		trendTime[29].time, "","","","",trendTime[24].time, "","","","",
		trendTime[19].time, "","","","",trendTime[14].time, "","","","",
		trendTime[9].time,  "","","","",trendTime[4].time,  "","","",trendTime[0].time
	);
	prtIdx = TRENDRANGE-1;
	prtAddr = 0;

	fprintf(fpTrendData,"datasets : [{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend01","255,0,0,1.0","255,0,0,1.0","255,0,0,1.0");
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend02","0,0,255,1.0","0,0,255,1.0","0,0,255,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 1;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend03","0,255,0,1.0","0,255,0,1.0","0,255,0,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 2;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend04","255,255,0,1.0","255,255,0,1.0","255,255,0,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 3;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend05","0,255,255,1.0","0,255,255,1.0","0,255,255,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 4;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend06","255,0,255,1.0","255,0,255,1.0","255,0,255,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 5;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend07","247,147,30,1.0","247,147,30,1.0","247,147,30,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 6;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend08","115,75,35,1.0","115,75,35,1.0","115,75,35,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 7;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend09","194,153,107,1.0","194,153,107,1.0","194,153,107,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 8;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend10","17,161,150,1.0","17,161,150,1.0","17,161,150,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 9;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend11","220,38,83,1.0","220,38,83,1.0","220,38,83,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 10;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend12","29,43,83,1.0","29,43,83,1.0","29,43,83,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 11;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend13","149,28,90,1.0","149,28,90,1.0","149,28,90,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 12;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend14","208,215,47,1.0","208,215,47,1.0","208,215,47,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 13;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend15","0,101,54,1.0","0,101,54,1.0","0,101,54,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 14;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend16","193,39,45,1.0","193,39,45,1.0","193,39,45,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 15;
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],
		trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--],trendTemp[prtAddr][prtIdx--]
	);
	fprintf(fpTrendData,"}]\n");
	fprintf(fpTrendData,"}\n");
	fprintf(fpTrendData,"var options = { scaleOverlay : true, scaleOverride : false, scaleSteps : 10, scaleStepWidth : 10, scaleStartValue : 0, scaleLineColor : \"rgba(0, 0, 0, 0.1)\", scaleLineWidth : 1, scaleShowLabels : true, scaleLabel : \"  <%=value%>%s\", scaleFontFamily : \"'Arial'\", scaleFontSize : 12, scaleFontStyle : \"normal\", scaleFontColor : \"#666\", scaleShowGridLines : true, scaleGridLineColor : \"rgba(0, 0, 0, 0.05)\", scaleGridLineWidth : 1, bezierCurve : false, pointDot : false, pointDotRadius : 5, pointDotStrokeWidth : 1, datasetStroke : false, datasetStrokeWidth : 2, datasetFill : false, animation : false, animationSteps : 60, animationEasing : \"easeOutQuad\", onAnimationComplete : null }\n",tempUnit);
	fprintf(fpTrendData,"window.onload = function(){ var ctx1 = document.getElementById(\"chart\").getContext(\"2d\"); window.myLine = new Chart(ctx1).Line(lineChartData, options); }\n");
	fprintf(fpTrendData,"</script><?php ?></body></html>\n");
	fclose(fpTrendData);
	system("cp /tmp/trendData.txt /tmp/trendGraph.php");
}

void writeCurrentMistLvGraph(){
	FILE *fpCurrentData;
	float lvNormal[NODESIZE] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	float lvPre[NODESIZE] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	float lvHigh[NODESIZE] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	int addr = 0;
	//currentData
	fpCurrentData = fopen("/tmp/currentMistLv.txt","w");
	if(fpCurrentData==NULL){
		//失敗と表示し終了
		printf("currentMistLv.txt Open Failed\n");
		return;
	}

	lvHighAlm = highAlarm * 1.0;
	lvPreAlm = preAlarm * 1.0;
	for(addr=0; addr<NODESIZE; addr++){
		if(trendMistLv[addr][0]>=highAlarm){//preAlarm
			lvHigh[addr] = factorA[addr]*(trendMistLv[addr][0]-(highAlarm-1))+factorB[addr];
			lvPre[addr] = factorA[addr]*(highAlarm-preAlarm)+factorB[addr];
			lvNormal[addr] = factorA[addr]*(preAlarm-1)+factorB[addr];
		} else if (trendMistLv[addr][0]>=preAlarm){
			lvPre[addr] = factorA[addr]*(trendMistLv[addr][0]-(preAlarm-1))+factorB[addr];
			lvNormal[addr] = factorA[addr]*(preAlarm-1)+factorB[addr];
		} else {
			lvNormal[addr] = factorA[addr]*trendMistLv[addr][0]+factorB[addr];
		}
		if (isDebug) printf("addr[%d],lvHigh[%f],lvPre[%f],lvNormal[%f],highAlarm[%d],preAlarm[%d]\n",addr,lvHigh[addr],lvPre[addr],lvNormal[addr],highAlarm,preAlarm);
	}
	if (isDebug) printf("writeCurrentMistLvGraph\n");

	fprintf(fpCurrentData,"datasets: [{label: \"Normal\",borderWidth:1,backgroundColor: \"#00ff7f\",borderColor: \"#00ff7f\",\n");
	addr=0;
	fprintf(fpCurrentData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++],
		lvNormal[addr++]
	);
	fprintf(fpCurrentData,"},{label: \"PreHigh\",borderWidth:1,backgroundColor: \"#ffa000\",borderColor: \"#ffa000\",\n");
	addr=0;
	fprintf(fpCurrentData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++],
		lvPre[addr++]
	);
	fprintf(fpCurrentData,"},{label: \"HighAlm\",borderWidth:1,backgroundColor: \"#ff0000\",borderColor: \"#ff0000\",\n");
	addr=0;
	fprintf(fpCurrentData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++],
		lvHigh[addr++]
	);
	fprintf(fpCurrentData,"}]\n");

	fclose(fpCurrentData);
	system("cat /var/www/html/currentMistLv_top.txt /tmp/currentMistLv.txt /var/www/html/currentMistLv_bottom.txt > /tmp/currentMistLv.php");
}
void writeTrendMistLvGraph(){
	FILE *fpTrendData;
	//trendData
	int prtIdx;
	int prtAddr;
	float fA = 0.0;
	float fB = 0.0;
	
	fpTrendData = fopen("/tmp/trendMistLv.txt","w");
	if(fpTrendData==NULL){
		//失敗と表示し終了
		printf("trendMistLv.txt Open Failed\n");
		return;
	}
	if (isDebug) printf("writeTrendMistLvGraph\n");
	fprintf(fpTrendData,"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"><HTML><HEAD><title>Chart.js TEST</title></HEAD><body><div style=\"width: 100%\"><canvas id=\"chart\" height=\"400\" width=\"400\"></canvas></div><script src=\"Chart.min.js\"></script><script>\n");
	fprintf(fpTrendData,"var lineChartData = {\n");
	fprintf(fpTrendData,"labels : [\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",],",
		trendTime[119].time,"","","","",trendTime[114].time,"","","","",
		trendTime[109].time,"","","","",trendTime[104].time,"","","","",
		trendTime[99].time, "","","","",trendTime[94].time, "","","","",
		trendTime[89].time, "","","","",trendTime[84].time, "","","","",
		trendTime[79].time, "","","","",trendTime[74].time, "","","","",
		trendTime[69].time, "","","","",trendTime[64].time, "","","","",
		trendTime[59].time, "","","","",trendTime[54].time, "","","","",
		trendTime[49].time, "","","","",trendTime[44].time, "","","","",
		trendTime[39].time, "","","","",trendTime[34].time, "","","","",
		trendTime[29].time, "","","","",trendTime[24].time, "","","","",
		trendTime[19].time, "","","","",trendTime[14].time, "","","","",
		trendTime[9].time,  "","","","",trendTime[4].time,  "","","",trendTime[0].time
	);
	prtIdx = TRENDRANGE-1;
	prtAddr = 0;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"datasets : [{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend01","255,0,0,1.0","255,0,0,1.0","255,0,0,1.0");
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend02","0,0,255,1.0","0,0,255,1.0","0,0,255,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 1;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend03","0,255,0,1.0","0,255,0,1.0","0,255,0,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 2;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend04","255,255,0,1.0","255,255,0,1.0","255,255,0,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 3;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend05","0,255,255,1.0","0,255,255,1.0","0,255,255,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 4;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend06","255,0,255,1.0","255,0,255,1.0","255,0,255,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 5;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend07","247,147,30,1.0","247,147,30,1.0","247,147,30,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 6;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend08","115,75,35,1.0","115,75,35,1.0","115,75,35,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 7;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend09","194,153,107,1.0","194,153,107,1.0","194,153,107,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 8;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend10","17,161,150,1.0","17,161,150,1.0","17,161,150,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 9;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend11","220,38,83,1.0","220,38,83,1.0","220,38,83,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 10;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend12","29,43,83,1.0","29,43,83,1.0","29,43,83,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 11;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend13","149,28,90,1.0","149,28,90,1.0","149,28,90,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 12;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend14","208,215,47,1.0","208,215,47,1.0","208,215,47,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 13;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend15","0,101,54,1.0","0,101,54,1.0","0,101,54,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 14;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"},{\n");
	fprintf(fpTrendData,"type: 'line',label: \"%s\",strokeColor : \"rgba(%s)\",pointColor : \"rgba(%s)\",pointStrokeColor : \"#fff\",pointHighlightFill : \"#fff\",pointHighlightStroke : \"rgba(%s)\",\n","trend16","193,39,45,1.0","193,39,45,1.0","193,39,45,1.0");
	prtIdx = TRENDRANGE-1;
	prtAddr = 15;
	fA = factorA[prtAddr];
	fB = factorB[prtAddr];
	fprintf(fpTrendData,"data : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]\n",
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,
		fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB,fA*trendMistLv[prtAddr][prtIdx--]+fB
	);
	fprintf(fpTrendData,"}]\n");
	fprintf(fpTrendData,"}\n");
	fprintf(fpTrendData,"var options = { scaleOverlay : true, scaleOverride : false, scaleSteps : 10, scaleStepWidth : 10, scaleStartValue : 0, scaleLineColor : \"rgba(0, 0, 0, 0.1)\", scaleLineWidth : 1, scaleShowLabels : true, scaleLabel : \"  <%=value%>%s\", scaleFontFamily : \"'Arial'\", scaleFontSize : 12, scaleFontStyle : \"normal\", scaleFontColor : \"#666\", scaleShowGridLines : true, scaleGridLineColor : \"rgba(0, 0, 0, 0.05)\", scaleGridLineWidth : 1, bezierCurve : false, pointDot : false, pointDotRadius : 5, pointDotStrokeWidth : 1, datasetStroke : false, datasetStrokeWidth : 2, datasetFill : false, animation : false, animationSteps : 60, animationEasing : \"easeOutQuad\", onAnimationComplete : null }\n",mistUnit);
	fprintf(fpTrendData,"window.onload = function(){ var ctx1 = document.getElementById(\"chart\").getContext(\"2d\"); window.myLine = new Chart(ctx1).Line(lineChartData, options);}\n");
	fprintf(fpTrendData,"</script><?php ?></body></html>\n");

	fclose(fpTrendData);
	system("cp /tmp/trendMistLv.txt /tmp/trendMistLv.php");
}

void makeHistoryData(){
	int idx;
	int addr = 0;
	for(idx=0;idx<(TRENDRANGE-1);idx++){
		strcpy(trendTime[TRENDRANGE - idx - 1].date,trendTime[TRENDRANGE - idx - 2].date);
		strcpy(trendTime[TRENDRANGE - idx - 1].time,trendTime[TRENDRANGE - idx - 2].time);

		for(addr=0; addr<NODESIZE; addr++){
			trendMistLv[addr][TRENDRANGE - idx - 1] = trendMistLv[addr][TRENDRANGE - idx - 2];
			trendTemp[addr][TRENDRANGE - idx - 1] = trendTemp[addr][TRENDRANGE - idx - 2];
			trendStatus[addr][TRENDRANGE - idx - 1] = trendStatus[addr][TRENDRANGE - idx - 2];
		}

	}
	sprintf(trendTime[0].date,"%04d/%02d/%02d",time_st->tm_year+1900,time_st->tm_mon+1,time_st->tm_mday);
	sprintf(trendTime[0].time,"%02d:%02d:%02d",time_st->tm_hour,time_st->tm_min,time_st->tm_sec);
	for(addr=0; addr<NODESIZE; addr++){
		if(trendDataList[addr].recvCnt>0){
			trendMistLv[addr][0] = 1.0 * trendDataList[addr].sumMistLv / trendDataList[addr].recvCnt;
			trendTemp[addr][0] = 1.0 * trendDataList[addr].sumTemp / trendDataList[addr].recvCnt;
		}
		trendStatus[addr][0] = (trendDataList[addr].errorFlg[1] * 256) + trendDataList[addr].errorFlg[0];
	}
}

void writeDailyTrendData(){
	int idx = 0;
	char fileDate[12];
	char fileTime[12];
	char fileName[256];
	char tmpChar[12];
	char delFileName[1024];
	char delCommand[1024];
	FILE *fpTrendDaily;
	FILE *fpDelFile;
	float freeSpace = 0.0;
	struct tm *alarmT_st;
	struct timeval alarmTime;
	FILE *fpAlarmLog;

	dailyBufCnt++;
	if(dailyBufCnt < DAIRYINTERVAL){
		return;
	}

	strcpy(fileDate,trendTime[DAIRYINTERVAL-1].date);
	strcpy(fileTime,trendTime[DAIRYINTERVAL-1].time);
	
	targetTime[0] = ((fileTime[0] & 0x0F)*10)+(fileTime[1] & 0x0F); 
	if(((targetTime[0]==8) && (targetTime[1]==7)) || ((targetTime[0]==16) && (targetTime[1]==15))) {
		targetTime[1] = -1;
		dairyCnt++;
	}else if((targetTime[0]==0) && (targetTime[1]==23)){
		targetTime[1] = -1;
		dairyCnt = 0;
	}
	sprintf(fileTime,"%02d",dairyCnt);

	tmpChar[0] = fileDate[2];
	tmpChar[1] = fileDate[3];
	tmpChar[2] = fileDate[5];
	tmpChar[3] = fileDate[6];
	tmpChar[4] = fileDate[8];
	tmpChar[5] = fileDate[9];
	tmpChar[6] = 0x00;
	strcpy(fileName,dairyOutputDir);
	strcat(fileName,"/");
	strcat(fileName,tmpChar);
	strcat(fileName,".csv");
	sprintf(fileName,"%s/%s%02d.csv",dairyOutputDir,tmpChar,dairyCnt);
	if(targetTime[1]==-1){
		while((fpTrendDaily = fopen(fileName, "r")) != NULL){
			fclose(fpTrendDaily);
			dairyCnt++;
			sprintf(fileName,"%s/%s%02d.csv",dairyOutputDir,tmpChar,dairyCnt);
		};
	}

	fpTrendDaily = fopen(fileName,"a");
	if(fpTrendDaily==NULL){
		//失敗と表示し終了
		printf("DailyFile Open Failed\n");
		return;
	}
	if(targetTime[1]==-1){
		lineCnt=1;
		fprintf(fpTrendDaily,"%s,,,#01,#01,#01,#01,#02,#02,#02,#02,#03,#03,#03,#03,#04,#04,#04,#04,#05,#05,#05,#05,#06,#06,#06,#06,#07,#07,#07,#07,#08,#08,#08,#08,#09,#09,#09,#09,#10,#10,#10,#10,#11,#11,#11,#11,#12,#12,#12,#12,#13,#13,#13,#13,#14,#14,#14,#14,#15,#15,#15,#15,#16,#16,#16,#16\r\n",fileName);
		fprintf(fpTrendDaily,"No.,DAY,TIME,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS\r\n");
	}
	targetTime[1] = targetTime[0];
	freeSpace = checkDiskSize();
	if (isDebug) printf("free area: %10.3f MB \n", freeSpace);
	// 空き容量が50MBを下回る場合は一番古いファイルを削除
	if(freeSpace<50.0){
		strcpy(delCommand,"ls -l ");
		strcat(delCommand,dairyOutputDir);
		strcat(delCommand,"/*.csv | head -n 1 | gawk '{print $9}' > /tmp/removeFile.txt");
		system(delCommand);
		fpDelFile = fopen("/tmp/removeFile.txt","r");
		if(fpDelFile!=NULL){
			if(fgets(delFileName,1024,fpDelFile)!=NULL){
				strcpy(delCommand,"rm -f ");
				strcat(delCommand,delFileName);
				system(delCommand);
				printf("Delete Command: %s\n",delFileName);
			}
			fclose(fpDelFile);

			gettimeofday(&alarmTime, NULL);
			alarmT_st = localtime(&(alarmTime.tv_sec));
			fpAlarmLog = fopen("/tmp/alarm.log","a");
			if(fpAlarmLog!=NULL){
				fprintf(fpAlarmLog,"%04d/%02d/%02d %02d:%02d:%02d,--,free area not enough.\n",alarmT_st->tm_year+1900,alarmT_st->tm_mon+1,alarmT_st->tm_mday,alarmT_st->tm_hour,alarmT_st->tm_min,alarmT_st->tm_sec);
				fclose(fpAlarmLog);
			}
		}
	}

	if (isDebug) printf("Write DailyFile[%s]\n",fileName);
	for(idx=(DAIRYINTERVAL-1); idx>=0; idx--){
		strcpy(fileTime,trendTime[idx].time);
		targetTime[0] = ((fileTime[0] & 0x0F)*10)+(fileTime[1] & 0x0F); 
		if(((targetTime[0]==8) && (targetTime[1]==7)) || ((targetTime[0]==16) && (targetTime[1]==15))) {
			targetTime[1] = -1;
			dairyCnt++;
		}else if((targetTime[0]==0) && (targetTime[1]==23)){
			targetTime[1] = -1;
			dairyCnt = 0;
		}
		if((strcmp(fileDate,trendTime[idx].date)!=0) || (targetTime[1] == -1)){
			fclose(fpTrendDaily);

			strcpy(fileDate,trendTime[idx].date);

			tmpChar[0] = fileDate[2];
			tmpChar[1] = fileDate[3];
			tmpChar[2] = fileDate[5];
			tmpChar[3] = fileDate[6];
			tmpChar[4] = fileDate[8];
			tmpChar[5] = fileDate[9];
			tmpChar[6] = 0x00;
			dairyCnt=0;
			sprintf(fileName,"%s/%s%02d.csv",dairyOutputDir,tmpChar,dairyCnt);
			while((fpTrendDaily = fopen(fileName, "r")) != NULL){
				fclose(fpTrendDaily);
				dairyCnt++;
				sprintf(fileName,"%s/%s%02d.csv",dairyOutputDir,tmpChar,dairyCnt);
			};
			if(dairyCnt>99){
				//失敗と表示し終了
				printf("DailyFile Open Failed\n");
				return;
			}

			fpTrendDaily = fopen(fileName,"a");
			if(fpTrendDaily==NULL){
				//失敗と表示し終了
				printf("DailyFile Open Failed\n");
				return;
			}
		}

		if(targetTime[1]==-1){
			lineCnt=1;
			fprintf(fpTrendDaily,"%s,,,#01,#01,#01,#01,#02,#02,#02,#02,#03,#03,#03,#03,#04,#04,#04,#04,#05,#05,#05,#05,#06,#06,#06,#06,#07,#07,#07,#07,#08,#08,#08,#08,#09,#09,#09,#09,#10,#10,#10,#10,#11,#11,#11,#11,#12,#12,#12,#12,#13,#13,#13,#13,#14,#14,#14,#14,#15,#15,#15,#15,#16,#16,#16,#16\r\n",fileName);
			fprintf(fpTrendDaily,"No.,DAY,TIME,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS,LEVEL,DATA,TEMP,STATUS\r\n");
		}

		fprintf(fpTrendDaily,"%d,%s,%s,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X,0,%d,%d,%04X\r\n",
			lineCnt++,
			trendTime[idx].date,
			trendTime[idx].time,
			roundOff(trendMistLv[0][idx]),
			roundOff(trendTemp[0][idx]),
			roundOff(trendStatus[0][idx]),
			roundOff(trendMistLv[1][idx]),
			roundOff(trendTemp[1][idx]),
			roundOff(trendStatus[1][idx]),
			roundOff(trendMistLv[2][idx]),
			roundOff(trendTemp[2][idx]),
			roundOff(trendStatus[2][idx]),
			roundOff(trendMistLv[3][idx]),
			roundOff(trendTemp[3][idx]),
			roundOff(trendStatus[3][idx]),
			roundOff(trendMistLv[4][idx]),
			roundOff(trendTemp[4][idx]),
			roundOff(trendStatus[4][idx]),
			roundOff(trendMistLv[5][idx]),
			roundOff(trendTemp[5][idx]),
			roundOff(trendStatus[5][idx]),
			roundOff(trendMistLv[6][idx]),
			roundOff(trendTemp[6][idx]),
			roundOff(trendStatus[6][idx]),
			roundOff(trendMistLv[7][idx]),
			roundOff(trendTemp[7][idx]),
			roundOff(trendStatus[7][idx]),
			roundOff(trendMistLv[8][idx]),
			roundOff(trendTemp[8][idx]),
			roundOff(trendStatus[8][idx]),
			roundOff(trendMistLv[9][idx]),
			roundOff(trendTemp[9][idx]),
			roundOff(trendStatus[9][idx]),
			roundOff(trendMistLv[10][idx]),
			roundOff(trendTemp[10][idx]),
			roundOff(trendStatus[10][idx]),
			roundOff(trendMistLv[11][idx]),
			roundOff(trendTemp[11][idx]),
			roundOff(trendStatus[11][idx]),
			roundOff(trendMistLv[12][idx]),
			roundOff(trendTemp[12][idx]),
			roundOff(trendStatus[12][idx]),
			roundOff(trendMistLv[13][idx]),
			roundOff(trendTemp[13][idx]),
			roundOff(trendStatus[13][idx]),
			roundOff(trendMistLv[14][idx]),
			roundOff(trendTemp[14][idx]),
			roundOff(trendStatus[14][idx]),
			roundOff(trendMistLv[15][idx]),
			roundOff(trendTemp[15][idx]),
			roundOff(trendStatus[15][idx])
		);
		targetTime[1] = targetTime[0];
	}
	fclose(fpTrendDaily);
	dailyBufCnt=0;

}

//切り上げ
int roundUp(float n){
    if(n >= 0){ 
        return (int)n + 1;
    }else{
        return (int)n - 1;
    }
}

//切り捨て
int roundDown(float n){
    return (int)n;
}

//四捨五入
int roundOff(float n){
    float decimal = 0;

    decimal = n - (int)n;

    if(decimal >= 0.5 || decimal <= -0.5){
        return roundUp(n);
    }else{
        return roundDown(n);
    }
}

float checkDiskSize() {

	struct statvfs vfs;
	//static char s_path[256] = "/tmp/sdcard";
	//static char s_path[256] = "/tmp/flashusb";
	static char s_path[256] = "/var/www/html/Dairy";

	strcpy(s_path,dairyOutputDir);
	float ttl, ttl_free;
	float r_free;

	if( statvfs( s_path, &vfs ) < 0 ){
		printf("Error path=[%s]\n",s_path); exit(1);
	} else {
		if (isDebug) {
			printf("Path        : [%s]\n",s_path);
			printf("block size  : %lu\n",  vfs.f_bsize);
			printf("total block : %lu\n", vfs.f_blocks);
			printf("free  blocks: %lu\n", vfs.f_bfree);
		}
		ttl      = ((float)vfs.f_bsize)*((float)vfs.f_blocks);
		ttl_free = ((float)vfs.f_bsize)*((float)vfs.f_bfree);
		r_free = ttl_free/ttl*100.0;
		if (isDebug) {
			printf("ttl=%10.0f ttl_free=%10.0f  r_free=%10.1f\n", ttl,   ttl_free,    r_free );
			printf("Mega: ttl=%10.3f ttl_free=%10.3f\n", ttl/1024.0/1024.0, ttl_free/1024.0/1024.0 );
			printf("GIGA: ttl=%10.3f ttl_free=%10.3f\n", ttl/1024.0/1024.0/1024, ttl_free/1024.0/1024.0/1024);
		}
	}
	return ttl_free/1024.0/1024.0;
}
