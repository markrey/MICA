
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>

#include <mosquitto.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define BUFSIZE 255

#define SERIAL_PORT "/dev/ttyUSB0"  /* シリアルインターフェースに対応するデバイスファイル */

char topic[BUFSIZE];
char message[BUFSIZE];
int connect_desire = TRUE;
int is_debug = FALSE;

int vtRate = 60; //6600V
int ctRate = 200; //100A

struct timeval myTime;
struct tm *time_st;

double calcPower(int measureVal){
	// 電力値[kW] = (計測データ – 10000) × VT 比データ × CT 比データ × 0.1 / 10000
	return (((measureVal) * vtRate * ctRate * 0.1 ) / 10000.0);
}

double calcVoltage(int measureVal){
	// 電圧値[V] = 計測データ × VT 比データ × 150[V] / 10000 
	return ((measureVal* vtRate * 150 ) / 10000.0);
}

double calcCurrent(int measureVal){
	// 電流値[A] = 計測データ × CT 比データ × 5[A] × 0.1 /10000
	return ((measureVal * ctRate * 5 * 0.1 ) / 10000.0);
}

double calcPowreFactor(int measureVal){
	return (measureVal / 10000.0);
}

double calcFrequency(int measureVal) {
	return (measureVal / 100.0);
}

/**
 * Brokerとの接続成功時に実行されるcallback関数
 */
void on_connect(struct mosquitto *mosq, void *obj, int result)
{
    if(is_debug) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
    }
    mosquitto_publish(mosq, NULL, topic, strlen(message), message, 0, false);
}

/**
 * Brokerとの接続を切断した時に実行されるcallback関数
 */
void on_disconnect(struct mosquitto *mosq, void *obj, int rc)
{
    if(is_debug) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
    }
}

/**
 * BrokerにMQTTメッセージ送信後に実行されるcallback関数
 */
static void on_publish(struct mosquitto *mosq, void *userdata, int mid)
{
    connect_desire = FALSE;
    mosquitto_disconnect(mosq);
}

int mqttPublisher(){
	int ret = 0;
    int   cmdopt        = 0;
    char *id            = "mqtt/pub";
    char *host          = "192.168.100.125";
    int   port          = 1883;
    char *cafile        = NULL;
    char *certfile      = NULL;
    char *keyfile       = NULL;
    int   keepalive     = 60;
    bool  clean_session = true;
    struct mosquitto *mosq = NULL;

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

    if(mosquitto_connect_bind(mosq, host, port, keepalive, NULL)){
        fprintf(stderr, "failed to connect broker.\n");
        mosquitto_lib_cleanup();
        return(EXIT_FAILURE);
    }

    do {
        ret = mosquitto_loop_forever(mosq, -1, 1);
    } while((ret == MOSQ_ERR_SUCCESS) && (connect_desire != FALSE));

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
}

main()
{
    char bufWrite[255];                    /* バッファ */
    char bufRead[255];                    /* バッファ */
    char buf[255];                    /* バッファ */
	int dataList[255];
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
	time_t timeStart;
	time_t timeCurrent;
	long l;
    FILE *fp;
    char str[256];

	rts_ctl = TIOCM_RTS;

	//system("stty parenb -parodd cs8 -cstopb -hupcl ignpar cread -opost -onlcr -icrnl clocal -isig -icanon -iexten -echoctl -echoke -echo -echoe -echok -crtscts ixany ospeed 9600 ispeed 0 -F /dev/ttyUSB0");
	//system("stty parenb -parodd cs8 -cstopb -hupcl ignpar cread -opost -onlcr -icrnl clocal -isig -icanon -iexten -echoctl -echoke -echo -echoe -echok -crtscts ixany ospeed 38400 ispeed 0 -F /dev/ttyUSB0");
	system("stty parenb -parodd cs8 -cstopb -hupcl ignpar cread -opost -onlcr -icrnl clocal -isig -icanon -iexten -echoctl -echoke -echo -echoe -echok -crtscts ixany ospeed 4800 ispeed 0 -F /dev/ttyUSB0");
	system("stty intr ^- quit ^- erase ^- kill ^- eof ^- eol ^- eol2 ^- swtch ^- start ^- stop ^- susp ^- rprnt ^- werase ^- lnext ^- -F /dev/ttyUSB0");
	
    //fd = open(SERIAL_PORT, O_RDWR);   /* デバイスをオープンする */
    fd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK);   /* デバイスをオープンする */
    
    ioctl(fd, TCGETS, &oldtio);       /* 現在のシリアルポートの設定を待避させる */
    newtio = oldtio;                  /* ポートの設定をコピー */
    //newtio.c_cflag = ...;             /* ポートの設定をおこなう 詳細はtermios(3)参照 */
	newtio.c_oflag=0;               /* rawモード */
	newtio.c_lflag=0;               /* 非カノニカル入力 */
    ioctl(fd, TCSETS, &newtio);       /* ポートの設定を有効にする */

	

	//ioctl(fd, TIOCMBIC, &rts_ctl);  //RTSのclr	
	bufWrite[0] = 0x01;
	bufWrite[1] = 0x04;
	bufWrite[2] = 0x00;
	bufWrite[3] = 0x00;
	bufWrite[4] = 0x00;
	bufWrite[5] = 0x20;
	bufWrite[6] = 0xF1;
	bufWrite[7] = 0xD2;

	do{
		time(&timeStart);
		time(&timeCurrent);
		for(readIdx=0;readIdx<255;readIdx++){
			bufRead[readIdx] = 0x00;
			dataList[readIdx] = 0;
		}

		printf("write");
		write(fd, bufWrite, 8);
		printf(":[%02X %02X %02X %02X %02X %02X %02X %02X]\n", bufWrite[0], bufWrite[1], bufWrite[2], bufWrite[3], bufWrite[4], bufWrite[5], bufWrite[6], bufWrite[7]);
		readIdx = 0;
		dataIdx = 0;

		recvFlg = 1;
		do{
			time(&timeCurrent);
			
			if(difftime(timeCurrent,timeStart)>3){
				recvFlg=0;
			}
			if(read(fd, buf, 1)>0){
				bufRead[readIdx]=buf[0];
				if((recvFlg==2) && ((readIdx % 2) ==0) &&(readIdx < recvSize+3)){
					value = bufRead[readIdx-1]*256 + bufRead[readIdx];
					//value = bufRead[readIdx]*256 + bufRead[readIdx-1];
					//printf("readIdx:%d,[%02X %02X],%d\n", readIdx,bufRead[readIdx-1], bufRead[readIdx],value);
					dataList[dataIdx++] = value;
				}

				if((readIdx==0) && (bufRead[readIdx]!=0x01)) {
					recvFlg=0;
				}
				if((readIdx==1) && (bufRead[readIdx]!=0x04)) {
					recvFlg=0;
				}
				if(readIdx==2) {
					recvSize=bufRead[readIdx];
					recvFlg=2;
				}
				readIdx++;
				if((recvFlg==2) && (readIdx >= recvSize+5)){
					recvFlg=0;
				}
				if(difftime(timeCurrent,timeStart)>3){
					recvFlg=0;
				}
			}
		}while(recvFlg!=0);

		dataSize=dataIdx;

		//ファイルを書き込みモードで開く
		fp = fopen("/var/www/html/memo.txt","w");
		//ファイルオープンに失敗した場合
		if(fp!=NULL){
			printf("data:");
			fprintf(fp,"data:");
			for(dataIdx=0;dataIdx<dataSize;dataIdx++){
				printf("%d ", dataList[dataIdx]);
				fprintf(fp,"%d ", dataList[dataIdx]);
				
				switch(dataIdx){
					case 3:
					strcpy(topic,"Voltage1");
					sprintf(message,"%3.1f",calcVoltage(dataList[dataIdx]));
					mqttPublisher();
					break;

					case 4:
					strcpy(topic,"Voltage2");
					sprintf(message,"%3.1f",calcVoltage(dataList[dataIdx]));
					mqttPublisher();
					break;

					case 5:
					strcpy(topic,"Voltage3");
					sprintf(message,"%3.1f",calcVoltage(dataList[dataIdx]));
					mqttPublisher();
					break;

					case 6:
					strcpy(topic,"Current1");
					sprintf(message,"%1.3f",calcCurrent(dataList[dataIdx]));
					mqttPublisher();
					break;

					case 7:
					strcpy(topic,"Current2");
					sprintf(message,"%1.3f",calcCurrent(dataList[dataIdx]));
					mqttPublisher();
					break;

					case 8:
					strcpy(topic,"Current3");
					sprintf(message,"%1.3f",calcCurrent(dataList[dataIdx]));
					mqttPublisher();
					break;

					case 30:
					strcpy(topic,"PowreFactor");
					sprintf(message,"%1.3f",calcPowreFactor(dataList[dataIdx]));
					mqttPublisher();
					break;

					case 31:
					strcpy(topic,"Frequency");
					sprintf(message,"%2.2f",calcFrequency(dataList[dataIdx]));
					mqttPublisher();
					break;

					case 14:
					strcpy(topic,"ActivePower");
					sprintf(message,"%1.3f",calcPower(dataList[dataIdx]));
					mqttPublisher();
					break;

					default:
					break;
				}
			}
			printf("\n");
			fprintf(fp,"\n");

			//ファイルを閉じる
			fclose(fp);
		}else{
			//失敗と表示し終了
			printf("ファイルオープン失敗\n");
			endFlg=0;
		}

		sleep(2);
	} while(endFlg==0);
	
	
    ioctl(fd, TCSETS, &oldtio);       /* ポートの設定を元に戻す */
    close(fd);                        /* デバイスのクローズ */

    return 0;
}
