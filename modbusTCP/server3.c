#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <ctype.h>
#include <errno.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
/* 最大同時処理数 */
#define MAX_CHILD (20)

int send_recv(int acc, int child_no);


/* サーバソケットの準備 */
int
server_socket(const char *portnm)
{
	char nbuf[NI_MAXHOST],sbuf[NI_MAXSERV];
	struct addrinfo hints, *res0;
	int soc, opt, errcode;
	socklen_t opt_len;
	
	/* アドレス情報のヒントをゼロクリア\ */
	(void) memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	/* アドレス情報の決定 */
	if ((errcode = getaddrinfo(NULL, portnm, &hints, &res0)) != 0) {
		(void) fprintf(stderr, "getaddrinfo():%s\n", gai_strerror(errcode));
		return (-1);
	}
	if ((errcode = getnameinfo(res0->ai_addr, res0->ai_addrlen,
								nbuf,sizeof(nbuf),
								sbuf,sizeof(sbuf),
								NI_NUMERICHOST | NI_NUMERICSERV)) != 0) {
		(void) fprintf(stderr, "getnameinfo():%s\n", gai_strerror(errcode));
		freeaddrinfo(res0);
		return (-1);
	}
	(void) fprintf(stderr, "port=%s\n", sbuf);
	/* ソケットの生成 */
	if ((soc = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol)) == -1) {
		perror("socket");
		freeaddrinfo(res0);
		return (-1);
	}
	/* ソケットオプション(再利用フラグ)設定 */
	opt = 1;
	opt_len = sizeof(opt);
	if ((setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, &opt, opt_len)) == -1) {
		perror("setsockopt");
		(void) close(soc);
		freeaddrinfo(res0);
		return (-1);
	}
	/* ソケットにアドレスを指定 */
	if ((bind(soc, res0->ai_addr, res0->ai_addrlen)) == -1) {
		perror("bind");
		(void) close(soc);
		freeaddrinfo(res0);
		return (-1);
	}
	/* アクセスバックログの指定 */
	if (listen(soc, SOMAXCONN) == -1) {
		perror("listen");
		(void) close(soc);
		freeaddrinfo(res0);
		return (-1);
	}
	freeaddrinfo(res0);
	return (soc);
}

/* アクセプトループ */
void
accept_loop(int soc)
{
	char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
	int child[MAX_CHILD];
	struct sockaddr_storage from;
	int acc, child_no, i, j, count, pos, ret;
	socklen_t len;
	struct pollfd targets[MAX_CHILD + 1];
	/* child配列の初期化 */
	for (i = 0; i < MAX_CHILD; i++){
		child[i] = -1;
	}
	child_no = 0;
	
	for(;;) {
		/* poll()用データの作成 */
		count = 0;
		targets[count].fd = soc;
		targets[count].events = POLLIN;
		count++;
		for (i = 0; i < child_no; i++){
			if (child[i] != -1) {
				targets[count].fd = child[i];
				targets[count].events = POLLIN;
				count++;
			}
		}
		(void) fprintf(stderr, "<<child count:%d>>\n", count - 1);
		switch (poll(targets, count, 10 * 1000)) {
		case -1:
			/* エラー */
			perror("select");
			break;
		case 0:
			/* タイムアウト */
			break;
		default:
			if (targets[0].revents & POLLIN) {
				/* サーバソケットレディ */
				len = (socklen_t) sizeof(from);
				/* 接続受付 */
				if((acc = accept(soc, (struct sockaddr *) &from, &len)) == -1){
					if(errno != EINTR) {
						perror("accept");
					}
				} else {
					(void) getnameinfo((struct sockaddr *) &from, len, hbuf,sizeof(hbuf), sbuf,sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
					(void) fprintf(stderr, "accept:%s:%s\n",hbuf,sbuf);
					/* childの空きを検索 */
					pos = -1;
					for (i = 0; i < child_no; i++){
						if (child[i] == -1) {
							pos = i;
							break;
						}
					}
					if (pos == -1) {
						/* 空きが無い */
						if (child_no + 1 >= MAX_CHILD) {
							/* childにこれ以上格納できない */
							(void) fprintf(stderr, "child is full : cannot accept\n");
							/* クローズしてしまう */
							(void) close(acc);
						} else {
							child_no++;
							pos = child_no - 1;
						}
					}
					if (pos != -1) {
						/* childに格納 */
						child[pos] = acc;
					}
				}
			}
			/* アクセプトしたソケットがレディ */
			for (i = 1; i < count; i++) {
				if (targets[i].revents & (POLLIN | POLLERR)) {
					/* 送受信 */
					if ((ret = send_recv(targets[i].fd, i - 1)) == -1) {
						/* エラーまたは切断 */
						/* クローズ */
						(void) close(targets[i].fd);
						/* childを空に */
						for (j = 0; j < child_no; j++){
							if (child[j] == targets[i].fd) {
								child[j] = -1;
								break;
							}
						}
					}
				}
			}
			break;
		}
	}
}

/* サイズ指定文字列連結 */
size_t
mystrlcat(char *dst, const char *src, size_t size)
{
	const char *ps;
	char *pd, *pde;
	size_t dlen, lest;
	
	for(pd = dst, lest = size; *pd != '\0' && lest != 0; pd++, lest--);
	dlen = pd - dst;
	if (size - dlen == 0) {
		return (dlen + strlen(src));
	}
	pde = dst + size - 1;
	for (ps = src; *ps != '\0' && pd < pde; pd++, ps++) {
		*pd = *ps;
	}
	for (; pd <= pde; pd++){
		*pd = '\0';
	}
	while (*ps++);
	return (dlen + (ps - src - 1));
}

/* 送受信 */
int 
send_recv(int acc, int child_no)
{
	char buf[512], *ptr;
	ssize_t len;

	/* 受信 */
	if ((len = recv(acc, buf, sizeof(buf), 0)) == -1) {
		/* エラー */
		perror("recv");
		return (-1);
	}
	if (len == 0) {
		/* エンド・オブ・ファイル */
		(void) fprintf(stderr,"[child%d]recv:EOF\n",child_no);
		return (-1);
	}
	/* 文字列化・表示 */
	buf[len]='\0';
	if ((ptr = strpbrk(buf, "\r\n")) != NULL) {
		*ptr = '\0';
	}
	(void)fprintf(stderr, "[child%d]%s\n", child_no, buf);
	/* 応答文字列作成 */
	(void) mystrlcat(buf, ":OK\r\n", sizeof(buf));
	len = (ssize_t) strlen(buf);
	/* 応答 */
	if ((len = send(acc, buf, (size_t) len, 0)) == -1) {
		/* エラー */
		perror("send");
		return (-1);
	}
	return (0);
}

int
main(int argc, char *argv[])
{
	int soc;
	if (argc<=1) {
		(void) fprintf(stderr,"servere port\n");
		return (EX_USAGE);
	}
	/* サーバソケットの準備 */
	if ((soc = server_socket(argv[1])) == -1) {
		(void) fprintf(stderr,"server_socket(%s):error\n",argv[1]);
		return (EX_UNAVAILABLE);
	}
	(void) fprintf(stderr, "ready for accept\n");
	/* アクセプトループ */
	accept_loop(soc);
	/* ソケットクローズ */
	(void) close(soc);
	return (EX_OK);
}
