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

#define LABEL_DEVICENAME "DEVICENAME"
#define LABEL_BAUDRATE "BAUDRATE"
#define LABEL_DATABIT "DATABIT"
#define LABEL_STOPBIT "STOPBIT"
#define LABEL_PARITY "PARITY"
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
#define DEFAULT_NETMODE "static"
#define DEFAULT_IPADDRESS "192.168.1.10"
#define DEFAULT_SUBNETMASK "255.255.255.0"
#define DEFAULT_GATEWAYADDRESS "192.168.1.1"
#define DEFAULT_DNSADDRESS "192.168.1.1"
#define DEFAULT_BROADCAST "192.168.1.255"

#define PARITY_ODD "ODD"
#define PARITY_EVEN "EVEN"
#define PARITY_NONE "NONE"
#define DATABIT_5 "5"
#define DATABIT_6 "6"
#define DATABIT_7 "7"
#define DATABIT_8 "8"
#define STOPBIT_1 "1"
#define STOPBIT_2 "2"




int isDebug=0;

char systemCmd[BUFSIZE];

char deviceName[BUFSIZE];
char baudrate[BUFSIZE];
char dataBit[BUFSIZE];
char stopBit[BUFSIZE];
char parity[BUFSIZE];

char netMode[BUFSIZE];
char ipAddress[BUFSIZE];
char subnetMask[BUFSIZE];
char gatewayAddress[BUFSIZE];
char dnsAddress[BUFSIZE];
char broadcast[BUFSIZE];

void initData();
int ReadIni();

//--------------------- 

int main(int argc,char *argv[]) {
	printf("argc:%d\n",argc);
	if(argc==2){
		isDebug = atoi(argv[1]);
	}

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
	
	strcpy(systemCmd,"ifconfig eadd1 ");
	strcat(systemCmd,ipAddress);
	strcat(systemCmd," netmask ");
	strcat(systemCmd,subnetMask);
	strcat(systemCmd," broadcast ");
	strcat(systemCmd,broadcast);
	system(systemCmd);

    return 0;
}

void initData(){
	int idx;
	int printAddr;


	strcpy(deviceName,DEFAULT_DEVICENAME);
	strcpy(baudrate,DEFAULT_BAUDRATE);
	strcpy(dataBit,DEFAULT_DATABIT);
	strcpy(stopBit,DEFAULT_STOPBIT);
	strcpy(parity,DEFAULT_PARITY);

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

	if((fp = fopen("/var/www/html/lancomConfig.ini", "rt")) == NULL){
		printf("lancomConfig.ini cannot open.\n");
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


