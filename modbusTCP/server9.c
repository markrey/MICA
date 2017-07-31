#include <sys/epoll.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <ctype.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

/* 最大同時処理数 */
#define MAX_CHILD (20)

#define MAXQUEUESZ 4096
#define MAXSENDER 2
#define QUEUE_NEXT(i_) (((i_) +1) % MAXQUEUESZ)

struct queue_data {
	int acc;
	char buf[512];
	ssize_t len;
};
struct queue {
	int front;
	int last;
	struct queue_data data[MAXQUEUESZ];
	pthread_mutex_t mutex;
	pthread_cond_t cond;
};
struct queue g_queue[MAXSENDER];
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
	struct sockaddr_storage from;
	int acc, count, i, qi, epollfd, nfds;
	socklen_t flen;
	struct epoll_event ev, events[MAX_CHILD + 1];
	
	if ((epollfd = epoll_create(1)) == -1) {
		perror("epoll_create");
		return;
	}
	/* EPOLL用データの作成 */
	ev.data.fd = soc;
	ev.events = EPOLLIN;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, soc, &ev) == -1) {
		perror("epoll_ctl");
		(void) close(epollfd);
		return;
	}
	count = 0;
	for(;;) {
		(void) fprintf(stderr, "<<child count:%d>>\n", count);
		switch ((nfds = epoll_wait(epollfd, events, MAX_CHILD+1, 10 * 1000))){
		case -1:
			/* エラー */
			perror("epoll_wait");
			break;
		case 0:
			/* タイムアウト */
			break;
		default:
			/* ソケットがレディ */
			for (i = 0; i < nfds; i++){
				if (events[i].data.fd == soc){
					/* サーバソケットレディ */
					flen = (socklen_t) sizeof(from);
					/* 接続受付 */
					if ((acc = accept(soc, (struct sockaddr * )&from, &flen)) == -1) {
						if(errno!=EINTR){
							perror("accept");
						}
					} else {
						(void) getnameinfo((struct sockaddr *) &from, flen, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
						(void) fprintf(stderr, "accept:%s:%s\n", hbuf, sbuf);
						/* 空きが無い */
						if (count + 1 >= MAX_CHILD) {
							/* これ以上接続できない */
							(void) fprintf(stderr, "connection is full : cannot accept\n");
							/* クローズしてしまう */
							(void) close(acc);
						} else {
							ev.data.fd = acc;
							ev.events = EPOLLIN;
							if (epoll_ctl(epollfd, EPOLL_CTL_ADD, acc, &ev) == -1){
								perror("epoll_ctl");
								return;
							}
							count++;
						}
					}
				} else {
					qi = events[i].data.fd % MAXSENDER;
					g_queue[qi].data[g_queue[qi].last].acc = events[i].data.fd;
					g_queue[qi].data[g_queue[qi].last].len = recv(g_queue[qi].data[g_queue[qi].last].acc, g_queue[qi].data[g_queue[qi].last].buf, sizeof(g_queue[qi].data[g_queue[qi].last].buf), 0);
					/* 受信 */
					switch (g_queue[qi].data[g_queue[qi].last].len) {
					case -1:
						/* エラー */
						perror("recv");
						break;
					case 0:
						/* エンド・オブ・ファイル */
						(void) fprintf(stderr, "[child%d]recv:EOF\n", events[i].data.fd);
						/* エラーまたは切断 */
						if (epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &ev) == -1) {
							perror("epoll_ctl");
							return;
						}
						/* クローズ */
						(void) close(events[i].data.fd);
						count--;
						break;
					default:
						(void) pthread_mutex_lock(&g_queue[qi].mutex);
						g_queue[qi].last = QUEUE_NEXT(g_queue[qi].last);
						(void) pthread_cond_signal(&g_queue[qi].cond);
						(void) pthread_mutex_unlock(&g_queue[qi].mutex);
						break;
					}
				}
			}
			break;
		}
	}
	(void) close(epollfd);
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
void * 
send_thread(void *arg)
{
	char *ptr;
	ssize_t len;
	int i, qi;
	qi = (int) arg;

	for (;;) {
		(void) pthread_mutex_lock(&g_queue[qi].mutex);
		if(g_queue[qi].last != g_queue[qi].front) {
			i = g_queue[qi].front;
			g_queue[qi].front = QUEUE_NEXT(g_queue[qi].front);
			(void) pthread_mutex_unlock(&g_queue[qi].mutex);
		} else {
			(void) pthread_cond_wait(&g_queue[qi].cond, &g_queue[qi].mutex);
			(void) pthread_mutex_unlock(&g_queue[qi].mutex);
			continue;
		}
		/* g_queue[qi] */
		/* 文字列化・表示 */
		g_queue[qi].data[i].buf[g_queue[qi].data[i].len]='\0';
		if ((ptr = strpbrk(g_queue[qi].data[i].buf, "\r\n")) != NULL) {
			*ptr = '\0';
		}
		(void)fprintf(stderr, "[child%d]%s\n", g_queue[qi].data[i].acc, g_queue[qi].data[i].buf);
		/* 応答文字列作成 */
		(void) mystrlcat(g_queue[qi].data[i].buf, ":OK\r\n", sizeof(g_queue[qi].data[i].buf));
		g_queue[qi].data[i].len = (ssize_t) strlen(g_queue[qi].data[i].buf);
		/* 応答 */
		if ((len = send(g_queue[qi].data[i].acc, g_queue[qi].data[i].buf, (size_t) g_queue[qi].data[i].len, 0)) == -1) {
			/* エラー */
			perror("send");
		}
	}
	pthread_exit((void *) 0);
	
	/* NOT REACHED */
	return ((void *) 0);
}

int
main(int argc, char *argv[])
{
	int soc, i;
	pthread_t id;
	/* 引数にポート番号が指定されているか？ */
	if (argc<=1) {
		(void) fprintf(stderr,"server9 port\n");
		return (EX_USAGE);
	}
	for (i = 0; i < MAXSENDER; i++) {
		(void) pthread_mutex_init(&g_queue[i].mutex, NULL);
		(void) pthread_cond_init(&g_queue[i].cond, NULL);
		(void) pthread_create(&id, NULL, (void *) send_thread, (void *) i);
	}
	/* サーバソケットの準備 */
	if ((soc = server_socket(argv[1])) == -1) {
		(void) fprintf(stderr,"server_socket(%s):error\n",argv[1]);
		return (EX_UNAVAILABLE);
	}
	(void) fprintf(stderr, "ready for accept\n");
	/* アクセプトループ */
	accept_loop(soc);
	pthread_join(id, NULL);
	/* ソケットクローズ */
	(void) close(soc);
	return (EX_OK);
}
