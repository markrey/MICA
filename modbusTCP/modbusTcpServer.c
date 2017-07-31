#include <stdio.h>
#include <stdlib.h>     /* exit() */
#include <string.h>     /* bzero() */
#include <sys/types.h>
#include <sys/socket.h> /* socket(), bind(), listen(), accept(), recv() */
#include <netinet/in.h> /* htons() */
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#define PORT    502    /* Listenするポート */
#define MAXDATA 1024    /* 受信バッファサイズ */

int main(void){
	struct sockaddr_in saddr; /* サーバ用アドレス格納構造体 */
	struct sockaddr_in caddr; /* クライアント用アドレス格納構造体 */

	int listen_fd;
	int conn_fd;

	int len = sizeof(struct sockaddr_in);
	int rsize;
	char buf[MAXDATA]; /* 受信バッファ */
	char opt[16];

	/* ソケットの生成 */
	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	/*
	* saddrの中身を0にしておかないと、bind()でエラーが起こることがある
	*/
	bzero((char *)&saddr, sizeof(saddr));

	/** 使用するポートを指定 標準:[eth0] 拡張:[eadd1] **/
	strcpy(opt,"eadd1");
	setsockopt(listen_fd, SOL_SOCKET, SO_BINDTODEVICE, opt, 4);

	/* ソケットにアドレスとポートを結びつける */
	saddr.sin_family        = PF_INET;
	saddr.sin_addr.s_addr   = INADDR_ANY;
	saddr.sin_port          = htons(PORT);
	if (bind(listen_fd, (struct sockaddr *)&saddr, len) < 0) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	/* ポートをListenする */
	if (listen(listen_fd, SOMAXCONN) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}
	printf("Start Listening Port : %d...\n", PORT);

	/* 接続要求を受け付ける */
	if ((conn_fd = accept(listen_fd, (struct sockaddr *)&caddr, &len)) < 0) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

	/* Listeningソケットを閉じる */
	close(listen_fd);

	memset(buf, 0, sizeof(buf));
	do {
		/* データ受信 */
		rsize = read(conn_fd, buf, MAXDATA);
		if (rsize == 0) { /* クラアイントが接続を切ったとき */
			break;
		} else if (rsize == -1) {
			perror("recv");
			exit(EXIT_FAILURE);
		} else {
			printf("%d, %s\n", rsize, buf);
			/* データ送信 */
			write(conn_fd, buf, rsize);
		}
	} while (1);

	if ( close(conn_fd) < 0) {
		perror("close");
		exit(EXIT_FAILURE);
	}

	printf("Connection closed.\n");
	return 0;
}