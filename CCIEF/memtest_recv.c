//<<受信プログラム>>
#include    <stdio.h>
#include    <sys/types.h>
#include    <sys/ipc.h>
#include    <sys/shm.h>


void main()
{
int   id;
char  *adr;
int key;
	key = ftok("/tmp/shm", 1); 
	printf("key:%d",key);

	//if((id=shmget(IPC_PRIVATE,512,IPC_CREAT|0666))==-1){
	if((id=shmget(key,512,IPC_CREAT|0666))==-1){
        perror("shmget");
        exit(-1);
    }
//	id = key;

    printf("共有メモリID=%d\n",id);

    if((adr=shmat(id,0,0))==-1){
        perror("shmat");
    }
    else{
        strcpy(adr,"Initial");

        while(1){
            printf("%s\n",adr);
            if(strcmp(adr,"end")==0){
                break;
            }
            sleep(1);
        }

        if(shmdt(adr)==-1){
            perror("shmdt");
        }
    }
    if(shmctl(id,IPC_RMID,0)==-1){
        perror("shmctl");
    }
}
