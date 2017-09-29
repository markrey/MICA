//<<送信プログラム>>
//
#include    <stdio.h>
#include    <sys/types.h>
#include    <sys/ipc.h>
#include    <sys/shm.h>
#include    <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mosquitto.h>

#define	CCIEF_BASIC_OPERATE_SIZE							8			/* Size of OPERATE [byte] */
#define	CCIEF_BASIC_RX_RY_SIZE								8			/* Size of RX/RY [byte] */
#define	CCIEF_BASIC_RWW_RWR_SIZE							64			/* Size of RWw/RWr [byte] */
#define	CCIEF_BASIC_MAX_SLAVE_NUMBER						64			/* Number of MAX slaves */


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

char *topic   = "cciefw/#";
/* debug mode flag */
int is_debug = FALSE;

uint16_t  sendData;
uint16_t  *ptrSendData;
uint16_t  *ausRY;	/* RY for the master */
uint16_t  *ausRWw;	/* RWw for the master */
uint16_t  *ausOpe;	/* RY for the master */


void writeMemRY(int idx,uint16_t sendData)
{
	*(ausRY+idx) = sendData;
}
void writeMemRWw(int idx,uint16_t sendData)
{
	*(ausRWw+idx) = sendData;
}

void writeOperate(char *cmd1, char *cmd2)
{
	int cmdIdx = 0;
	if(strcmp(cmd1,"Cyclic")==0){
		if(strcmp(cmd2,"Start")==0){
			cmdIdx = 0;
		}else if(strcmp(cmd2,"Stop")==0){
			cmdIdx = 1;
		}
	}else if(strcmp(cmd1,"App")==0){
		if(strcmp(cmd2,"Start")==0){
			cmdIdx = 2;
		}else if(strcmp(cmd2,"Stop")==0){
			cmdIdx = 3;
		}
	}
	*(ausOpe + cmdIdx) = 1;
}



/**
 * Brokerとの接続成功時に実行されるcallback関数
 */
void on_connect(struct mosquitto *mosq, void *obj, int result)
{
    if(is_debug) {
        printf("%s(%d)\n", __FUNCTION__, __LINE__);
    }
    mosquitto_subscribe(mosq, NULL, topic, 0);
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
 * メッセージ受信処理
 */
void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message)
{
	char *s2 = ",";
	char *tok;
	int addr;
	int data;
	char cmd1[100];
	char cmd2[100];

	if(is_debug) {
		printf("%s(%d)\n", __FUNCTION__, __LINE__);
	}

	if(message->payloadlen){
		printf("%s ", message->topic);
		printf("%s ", message->payload);
		printf("\n");

		tok = strtok( message->payload, s2 );
		strcpy(cmd1,tok);
		addr = atoi(tok);
		tok = strtok( NULL, s2 );
		strcpy(cmd2,tok);
		data = atoi(tok);
		if(strcmp(message->topic,"cciefw/RY")==0){
			writeMemRY(addr,data);
		}else if(strcmp(message->topic,"cciefw/RWw")==0){
			writeMemRWw(addr,data);
		}else{
			writeOperate(cmd1,cmd2);
		}
	}else{
		printf("%s (null)\n", message->topic);
	}
	fflush(stdout);
}


/**
 * mqtt_subメイン関数
 */
int main(int argc, char *argv[])
{
    int   ret           = 0;
    int   cmdopt        = 0;
    char *id            = "mqtt/sub";
    char *host          = "192.168.10.55";
    int   port          = 1883;
    int   keepalive     = 60;
    bool  clean_session = true;
    struct mosquitto *mosq = NULL;

	int   cid;

	char adr[256];
	int key;
	int idx;
	int sizeOpe = (CCIEF_BASIC_OPERATE_SIZE / sizeof( uint16_t ));
	int sizeRWw = (CCIEF_BASIC_RWW_RWR_SIZE / sizeof( uint16_t )) * CCIEF_BASIC_MAX_SLAVE_NUMBER;
	int sizeRY = (CCIEF_BASIC_RX_RY_SIZE / sizeof( uint16_t )) * CCIEF_BASIC_MAX_SLAVE_NUMBER;
	
	system("cp /root/02_Master/shm /tmp");

	printf("sizeRY:%d,sizeRWw:%d\n",sizeRY,sizeRWw);

	key = ftok("/tmp/shm", 3); 
	if((cid=shmget(key,512,IPC_CREAT|0666))==-1){
		perror("shmget");
		exit(-1);
	}
	printf("key:%d,id:%d\n",key,cid);
	if((ptrSendData=shmat(cid,0,0))==-1){
		perror("shmat");
	}

	key = ftok("/tmp/shm", 4); 
	if((cid=shmget(key,((CCIEF_BASIC_RX_RY_SIZE / sizeof( uint16_t )) * CCIEF_BASIC_MAX_SLAVE_NUMBER),IPC_CREAT|0666))==-1){
		perror("shmget");
		exit(-1);
	}
	printf("key:%d,id:%d\n",key,cid);
	if((ausRY=shmat(cid,0,0))==-1){
		perror("shmat");
	}

	key = ftok("/tmp/shm", 5); 
	if((cid=shmget(key,((CCIEF_BASIC_RWW_RWR_SIZE / sizeof( uint16_t )) * CCIEF_BASIC_MAX_SLAVE_NUMBER),IPC_CREAT|0666))==-1){
		perror("shmget");
		exit(-1);
	}
	printf("key:%d,id:%d\n",key,cid);
	if((ausRWw=shmat(cid,0,0))==-1){
		perror("shmat");
	}

	key = ftok("/tmp/shm", 6); 
	if((cid=shmget(key,((CCIEF_BASIC_OPERATE_SIZE / sizeof( uint16_t ))),IPC_CREAT|0666))==-1){
		perror("shmget");
		exit(-1);
	}
	printf("key:%d,id:%d\n",key,cid);
	if((ausOpe=shmat(cid,0,0))==-1){
		perror("shmat");
	}


	mosquitto_lib_init();
	mosq = mosquitto_new(id, clean_session, NULL);
	if(!mosq){
		fprintf(stderr, "Cannot create mosquitto object\n");
		mosquitto_lib_cleanup();
		return(EXIT_FAILURE);
	}
	mosquitto_connect_callback_set(mosq, on_connect);
	mosquitto_disconnect_callback_set(mosq, on_disconnect);
	mosquitto_message_callback_set(mosq, on_message);

	if(mosquitto_connect_bind(mosq, host, port, keepalive, NULL)){
		fprintf(stderr, "failed to connect broker.\n");
		mosquitto_lib_cleanup();
		return(EXIT_FAILURE);
	}

	ret = mosquitto_loop_forever(mosq, -1, 1);

	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();

	return(EXIT_SUCCESS);
}
//$ gcc -Wall -g mqtt_sub.c -o mqtt_sub -lmosquitto

void detach(){
	if(shmdt(ptrSendData)==-1){
		perror("shmdt");
	}
	if(shmdt(ausRY)==-1){
		perror("shmdt");
	}
	if(shmdt(ausRWw)==-1){
		perror("shmdt");
	}
}

