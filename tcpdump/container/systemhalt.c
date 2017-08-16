#include <stdlib.h>
#include <stdio.h>

int main(void)
{
	char FileName[] = "/tmp/basehalt.txt";
	FILE *checkFile;
	char hostName[256];
	char haltCmd[512];
	
	if((checkFile = fopen(FileName, "r")) == NULL) {
		printf("スキップします\n");
	} else {
		fgets(hostName,256,checkFile);
		fclose(checkFile);
		//system("/bin/basehalt.sh");
		sprintf(haltCmd,"sshpass -p root rsh -l root %s /sbin/halt",hostName);
		printf("%s",haltCmd);
		system("sshpass -p root rsh -l root 10.10.10.10 /sbin/halt");
		printf("\n終了します\n");
	}

	return 0;
}
