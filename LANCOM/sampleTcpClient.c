#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(){
	struct sockaddr_in server;
	int sock;
	char bufRead[32];
	char bufWrite[32];
	int n;
	int cmdIdx = 0;
	char opt[16];

	/* ソケットの作成 */
	sock = socket(AF_INET, SOCK_STREAM, 0);

	/** 使用するポートを指定 標準:[eth0] 拡張:[eadd1] **/
	strcpy(opt,"eadd1");
	setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, opt, 4);

	/* 接続先指定用構造体の準備 */
	server.sin_family = PF_INET;
	server.sin_port = htons(12345);
	server.sin_addr.s_addr = inet_addr("192.168.0.101");

	/* サーバに接続 */
	connect(sock, (struct sockaddr *)&server, sizeof(server));

	memset(bufWrite, 0, sizeof(bufWrite));
	memset(bufRead, 0, sizeof(bufRead));

	/* データ送信 */
	bufWrite[0] = 0x31;
	bufWrite[1] = 0x32;
	bufWrite[2] = 0x33;
	bufWrite[3] = 0x34;
	bufWrite[4] = 0x35;
	write(sock, bufWrite, 5);

	/* データ受信 */
	n = read(sock, bufRead, sizeof(bufRead));

	printf("%d, %s\n", n, bufRead);

	/* socketの終了 */
	close(sock);

	return 0;
}
