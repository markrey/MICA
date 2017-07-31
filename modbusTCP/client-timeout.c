#include <sys/param.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

void send_recv_loop(int acc);

/* ブロッキングモードのセット */
int
set_block(int fd, int flag)
{
	int flags;
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
		perror("fcntl");
		return (-1);
	}
	if (flag == 0) {
		/* ノンブロッキング */
		(void) fcntl(fd, F_SETFL, flags |O_NONBLOCK);
	} else if (flag == 1) {
		/* ブロッキング */
		(void) fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
	}
	return (0);
}

int
client_socket_with_timeout(const char *hostnm, const char *portnm, int timeout_sec)
{
	char nbuf[NI_MAXHOST],sbuf[NI_MAXSERV];
	struct addrinfo hints, *res0;
	struct timeval timeout;
	int soc, errcode, width, val;
	socklen_t len;
	fd_set mask, write_mask, read_mask;

	/* アドレス情報のヒントをゼロクリア\ */
	(void) memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	/* アドレス情報の決定 */
	if ((errcode = getaddrinfo(hostnm, portnm, &hints, &res0)) != 0) {
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
	(void) fprintf(stderr, "addr=%s\n", nbuf);
	(void) fprintf(stderr, "port=%s\n", sbuf);
	/* ソケットの生成 */
	if ((soc = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol)) == -1) {
		perror("socket");
		freeaddrinfo(res0);
		return (-1);
	}

	if (timeout_sec < 0) {
		/* タイムアウト無し */
		/* コネクト */
		if ((connect(soc, res0->ai_addr, res0->ai_addrlen)) == -1) {
			perror("connect");
			(void) close(soc);
			freeaddrinfo(res0);
			return (-1);
		}
		freeaddrinfo(res0);
		return(soc);
	} else {
		/* タイムアウト有り */
		/* ノンブロッキングモードに */
		(void) set_block(soc, 0);
		/* コネクト */
		if ((connect(soc, res0->ai_addr, res0->ai_addrlen)) == -1) {
			if (errno != EINPROGRESS) {
				/* 進行中以外:エラー */
				perror("connect");
				(void) close(soc);
				freeaddrinfo(res0);
				return (-1);
			}
		} else {
			/* コネクト完了 */
			(void) set_block(soc, 1);
			freeaddrinfo(res0);
			return(soc);
			/* NOT REACHED */
		}
		/* コネクト結果待ち */
		width = 0;
		FD_ZERO(&mask);
		FD_SET(soc, &mask);
		width = soc + 1;
		timeout.tv_sec = timeout_sec;
		timeout.tv_usec = 0;
		for (;;) {
			write_mask = mask;
			read_mask = mask;
			switch (select(width, &read_mask, &write_mask, NULL, &timeout)) {
			case -1:
				if (errno != EINTR) {
					/* SELECTエラー */
					perror("select");
					(void) close(soc);
					freeaddrinfo(res0);
					return (-1);
				}
				break;
			case 0:
				/* SELECTタイムアウト */
				(void) fprintf(stderr, "select:timeout\n");
				(void) close(soc);
				freeaddrinfo(res0);
				return (-1);
				break;
			default:
				if (FD_ISSET(soc,&write_mask) || FD_ISSET(soc,&read_mask)) {
					len = sizeof(len);
					if (getsockopt(soc,SOL_SOCKET, SO_ERROR, &val, &len) != -1) {
						if (val == 0){
							/* connect成功 */
							(void) set_block(soc, 1);
							freeaddrinfo(res0);
							return(soc);
						} else {
							/* connect失敗 */
							(void) fprintf(stderr,"getsockopt:%d:%s\n",val,strerror(val));
							(void) close(soc);
							freeaddrinfo(res0);
							return (-1);
						}
					} else {
						/* getsockoptエラー */
						perror("getsockopt");
						(void) close(soc);
						freeaddrinfo(res0);
						return (-1);
					}
				}
				break;
			}
		}
	}
}

/* サーバにソケット接続 */
int
client_socket(const char *hostnm, const char *portnm)
{
	char nbuf[NI_MAXHOST],sbuf[NI_MAXSERV];
	struct addrinfo hints, *res0;
	int soc, errcode;

	/* アドレス情報のヒントをゼロクリア\ */
	(void) memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	/* アドレス情報の決定 */
	if ((errcode = getaddrinfo(hostnm, portnm, &hints, &res0)) != 0) {
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
	(void) fprintf(stderr, "addr=%s\n", nbuf);
	(void) fprintf(stderr, "port=%s\n", sbuf);
	/* ソケットの生成 */
	if ((soc = socket(res0->ai_family, res0->ai_socktype, res0->ai_protocol)) == -1) {
		perror("socket");
		freeaddrinfo(res0);
		return (-1);
	}
	/* ソケットにアドレスを指定 */
	if ((connect(soc, res0->ai_addr, res0->ai_addrlen)) == -1) {
		perror("connect");
		(void) close(soc);
		freeaddrinfo(res0);
		return (-1);
	}
	freeaddrinfo(res0);
	return (soc);
}

/* 送受信ループ */
void 
send_recv_loop(int soc)
{
	char buf1[512],buf2[512];
	struct timeval timeout;
	int end, width;
	ssize_t len;
	fd_set mask, ready;
	/* select()用マスク */
	FD_ZERO(&mask);
	/* ソケットディスクリプタをセット */
	FD_SET(soc, &mask);
	/* 標準入力をセット */
	FD_SET(0, &mask);
	width = soc + 1;
	/* 送受信 */
	for (end = 0;;) {
		/* マスクの代入 */
		ready = mask;
		/* タイムアウト値のセット */
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		switch (select(width, (fd_set *) &ready, NULL, NULL, &timeout)) {
		case -1:
			/* エラー */
			perror("select");
			break;
		case 0:
			/* タイムアウト */
			break;
		default :
			/* レディ有り */
			/* ソケットレディ */
			if (FD_ISSET(soc, &ready)) {
				/* 受信 */
				if ((len = recv(soc, buf1, sizeof(buf1), 0)) == -1) {
					/* エラー */
					perror("recv");
					end = 1;
					break;
				}
				if (len == 0) {
					/* エンド・オブ・ファイル */
					(void) fprintf(stderr, "recv:EOF\n");
					end = 1;
					break;
				}
				/* 文字列化・表示 */
				buf1[len]='\0';
				(void) fprintf(stderr,"> %s", buf1);
			}
			/* 標準入力レディ */
			if (FD_ISSET(0, &ready)) {
				/* 標準入力から1行読み込み */
				(void) fgets(buf2,sizeof(buf2),stdin);
				if (feof(stdin)) {
					end = 1;
					break;
				}
			}
			/* 送信 */
			if ((len = send(soc, buf2, strlen(buf2), 0)) == -1) {
				/* エラー */
				perror("send");
				end = 1;
				break;
			}
			buf2[0] = '\0';
			break;
		}
		if (end) {
			break;
		}
	}
}

int
main(int argc, char *argv[])
{
	int soc;
	/* 引数にホスト名、ポート番号、タイムアウトが指定されているか？ */
	if (argc<=3) {
		(void) fprintf(stderr,"client-timeout server-host port timeout-sec(-1:to-timeout)");
		return (EX_USAGE);
	}
	/* サーバにソケット接続 */
	if ((soc = client_socket_with_timeout(argv[1], argv[2], atoi(argv[3]))) == -1) {
		(void) fprintf(stderr,"client_socket_with_timeout():error\n");
		return (EX_UNAVAILABLE);
	}
	/* 送受信処理 */
	send_recv_loop(soc);
	/* ソケットクローズ */
	(void) close(soc);
	return (EX_OK);
}
