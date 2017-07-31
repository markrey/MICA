#include <sys/file.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#define NUM_CHILD 2
#define LOCK_FILE "./server7.lock"
int g_lock_fd = -1;

void send_recv_loop(int acc);


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
	struct sockaddr_storage from;
	int acc;
	socklen_t len;

	for(;;) {
		/* ロック獲得 */
		(void) fprintf(stderr, "<%d>ロック獲得開始\n", getpid());
		(void) lockf(g_lock_fd, F_LOCK, 0);
		(void) fprintf(stderr, "<%d>ロック獲得!\n", getpid());
		len = (socklen_t) sizeof(from);
		/* 接続受付 */
		if((acc = accept(soc, (struct sockaddr *) &from, &len)) == -1){
			if(errno != EINTR) {
				perror("accept");
			}
			(void) fprintf(stderr, "<%d>ロック解放\n", getpid());
			/* ロック解放 */
			(void) lockf(g_lock_fd, F_ULOCK, 0);
		} else {
			(void) getnameinfo((struct sockaddr *) &from, len, hbuf,sizeof(hbuf), sbuf,sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
			(void) fprintf(stderr, "<%d>accept:%s:%s\n",getpid(),hbuf,sbuf);
			(void) fprintf(stderr, "<%d>ロック解放\n", getpid());
			/* ロック解放 */
			(void) lockf(g_lock_fd, F_ULOCK, 0);
			/* 送受信ループ */
			send_recv_loop(acc);
			/* アクセプトソケットクローズ */
			(void) close(acc);
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

/* 送受信ループ */
void 
send_recv_loop(int acc)
{
	char buf[512], *ptr;
	ssize_t len;
	for (;;) {
		/* 受信 */
		if ((len = recv(acc, buf, sizeof(buf), 0)) == -1) {
			/* エラー */
			perror("recv");
			break;
		}
		if (len == 0) {
			/* エンド・オブ・ファイル */
			(void) fprintf(stderr,"<%d>recv:EOF\n", getpid());
			break;
		}
		/* 文字列化・表示 */
		buf[len]='\0';
		if ((ptr = strpbrk(buf, "\r\n")) != NULL) {
			*ptr = '\0';
		}
		(void)fprintf(stderr, "<%d>[client]%s\n", getpid(), buf);
		/* 応答文字列作成 */
		(void) mystrlcat(buf, ":OK\r\n", sizeof(buf));
		len = (ssize_t) strlen(buf);
		/* 応答 */
		if ((len = send(acc, buf, (size_t) len, 0)) == -1) {
			/* エラー */
			perror("send");
			break;
		}
	}
}

int
main(int argc, char *argv[])
{
	int i, soc;
	pid_t pid;
	
	/* 引数にポート番号が設定されているか？ */
	if (argc<=1) {
		(void) fprintf(stderr,"server7 port\n");
		return (EX_USAGE);
	}
	/* サーバソケットの準備 */
	if ((soc = server_socket(argv[1])) == -1) {
		(void) fprintf(stderr,"server_socket(%s):error\n",argv[1]);
		return (EX_UNAVAILABLE);
	}
	/* ロックファイルの生成 */
	if ((g_lock_fd = open(LOCK_FILE, O_RDWR | O_CREAT, 0666)) == -1) {
		perror("open");
		return (EX_UNAVAILABLE);
	}
	/* ファイル名の削除 */
	(void) unlink(LOCK_FILE);
	(void) fprintf(stderr,"start %d children\n", NUM_CHILD);
	/* 子プロセスの生成 */
	for (i = 0; i < NUM_CHILD; i++){
		if ((pid = fork()) == 0) {
			/* 子プロセス */
			/* アクセプトループ */
			accept_loop(soc);
			_exit(1);
		} else if (pid > 0) {
			/* fork()成功：親プロセス */
		} else {
			/* fork()失敗 */
			perror("fork");
		}
	}
	(void) fprintf(stderr, "ready for accept\n");
	/* 待ち・ロック状態の表示 */
	for (;;) {
		(void) sleep(10);
		(void) fprintf(stderr, "<<%d>>ロック状態：%d\n", getpid(), lockf(g_lock_fd, F_TEST, 0));
	}
	/* ソケットクローズ */
	(void) close(soc);
	(void) close(g_lock_fd);
	return (EX_OK);
}
