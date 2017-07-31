#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){
	struct sockaddr_in server;
	int sock;
	char bufRead[256];
	char bufWrite[256];
	char buff[256];
	int n;
	int cmdIdx = 0;
	char opt[16];
	int ret = 0;
	int rcvCnt = 0;
	int idx = 0;
	struct timeval tv;
	int status;

	tv.tv_sec = 2;
	tv.tv_usec = 0;

	printf ("Socket Start\n");
	/* ソケットの作成 */
	sock = socket(AF_INET, SOCK_STREAM, 0);

	/** 使用するポートを指定 標準:[eth0] 拡張:[eadd1] **/
	strcpy(opt,"eadd1");
	setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, opt, 4);

	/* 接続先指定用構造体の準備 */
	server.sin_family = PF_INET;
	server.sin_port = htons(502);
	server.sin_addr.s_addr = inet_addr("192.168.10.136");

	/* サーバに接続 */
	ret = connect(sock, (struct sockaddr *)&server, sizeof(server));
	printf ("connect : %d\n",ret);

	memset(bufWrite, 0, sizeof(bufWrite));
	memset(bufRead, 0, sizeof(bufRead));

	/* データ送信 */
	bufWrite[0] = 0x00;
	bufWrite[1] = 0x00;
	bufWrite[2] = 0x00;
	bufWrite[3] = 0x00;
	bufWrite[4] = 0x00;
	bufWrite[5] = 0x06;
	bufWrite[6] = 0x01;
	bufWrite[7] = 0x03;
	bufWrite[8] = 0x04;
	bufWrite[9] = 0x20;
	bufWrite[10] = 0x00;
	bufWrite[11] = 0x40;
	ret = send(sock, bufWrite, 12,0);
	printf ("send : %d\n",ret);

	/* データ受信 */
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));
	while(1) {
		ret = recv(sock, buff, 1, 0); 
		if(ret == 0 || ret == -1) {
			break;
		}
		bufRead[rcvCnt] = buff[0];
		rcvCnt++;
	}
	printf("received: %d [", rcvCnt);
	for(idx=0;idx<rcvCnt;idx++){
		printf("%02X ", bufRead[idx]);
	}
	printf("]\n", rcvCnt);

	/* socketの終了 */
	close(sock);

	return 0;
}
