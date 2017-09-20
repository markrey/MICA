//<<送信プログラム>>
//
#include    <stdio.h>
#include    <sys/types.h>
#include    <sys/ipc.h>
#include    <sys/shm.h>
#include    <stdint.h>

#define	CCIEF_BASIC_RX_RY_SIZE								8			/* Size of RX/RY [byte] */
#define	CCIEF_BASIC_RWW_RWR_SIZE							64			/* Size of RWw/RWr [byte] */
#define	CCIEF_BASIC_MAX_SLAVE_NUMBER						64			/* Number of MAX slaves */

void main()
{
	int   id;
	uint16_t  sendData;
	uint16_t  *ptrSendData;
	uint16_t  *ausRY;	/* RY for the master */
	uint16_t  *ausRWw;	/* RWw for the master */

	char adr[256];
	int key;
	int idx;
	int sizeRWw = (CCIEF_BASIC_RWW_RWR_SIZE / sizeof( uint16_t )) * CCIEF_BASIC_MAX_SLAVE_NUMBER;
	int sizeRY = (CCIEF_BASIC_RX_RY_SIZE / sizeof( uint16_t )) * CCIEF_BASIC_MAX_SLAVE_NUMBER;

	printf("sizeRY:%d,sizeRWw:%d\n",sizeRY,sizeRWw);

	key = ftok("/tmp/shm", 3); 
	if((id=shmget(key,512,IPC_CREAT|0666))==-1){
        perror("shmget");
        exit(-1);
    }
	printf("key:%d\n",key);
    if((ptrSendData=shmat(id,0,0))==-1){
        perror("shmat");
    }

	key = ftok("/tmp/shm", 4); 
	if((id=shmget(key,((CCIEF_BASIC_RX_RY_SIZE / sizeof( uint16_t )) * CCIEF_BASIC_MAX_SLAVE_NUMBER),IPC_CREAT|0666))==-1){
        perror("shmget");
        exit(-1);
    }
	printf("key:%d\n",key);
    if((ausRY=shmat(id,0,0))==-1){
        perror("shmat");
    }

	key = ftok("/tmp/shm", 5); 
	if((id=shmget(key,((CCIEF_BASIC_RWW_RWR_SIZE / sizeof( uint16_t )) * CCIEF_BASIC_MAX_SLAVE_NUMBER),IPC_CREAT|0666))==-1){
        perror("shmget");
        exit(-1);
    }
	printf("key:%d\n",key);	
    if((ausRWw=shmat(id,0,0))==-1){
        perror("shmat");
    }

	else{
        while(1){
            gets(adr);
            if(strcmp(adr,"end")==0){
                break;
            }
        	sendData = atoi(adr);
        	//sendData = 1;
        	*ptrSendData = sendData ;
        	for(idx=0;idx<sizeRY;idx++){
        		*(ausRY+idx) = sendData + idx;
        		printf("%d ",*(ausRY+idx));
        	}
			printf("\n");

        	for(idx=0;idx<sizeRWw;idx++){
        		*(ausRWw+idx) = sendData + idx;
        		printf("%d ",*(ausRWw+idx));
        	}
			printf("\n");
        }

    	if(shmdt(ptrSendData)==-1){
            perror("shmdt");
        }

    }
}

