#include <signal.h>
#include <stdio.h>
#include <unistd.h>

/* コマンドライン引数、環境変数のアドレス保持用 */
int *argc_;
char ***argv_;
char ***envp_;

void
sig_hangup_handler(int sig)
{
	(void) fprintf(stderr, "sig_hangup_handler(%d)\n", sig);
	/* 自プロセスの上書き再実行 */
	if (execve((*argv_)[0],(*argv_),(*envp_)) == -1) {
		perror("execve");
	}
}

int
main(int argc, char *argv[], char *envp[])
{
	struct sigaction sa;
	int i;
	/* コマンドライン引数、環境変数のアドレスをグローバルに保持 */
	argc_=&argc;
	argv_=&argv;
	envp_=&envp;
	/* コマンドライン引数、環境変数の表示 */
	(void) fprintf(stderr, "start pid=%d\n", getpid());
	(void) fprintf(stderr, "argc=%d\n", argc);
	for (i = 0; argv[i] != NULL; i++) {
		(void) fprintf(stderr, "argv[%d]=%s\n", i, argv[i]);
	}
	for (i = 0; envp[i] != NULL; i++) {
		(void) fprintf(stderr, "envp[%d]=%s\n", i, envp[i]);
	}
	/* sleep()によるSIGALRMを解除:Linuxでは行わなくても問題ないが移植性のため */
	(void) signal(SIGALRM, SIG_IGN);
#ifdef USE_SIGNAL
	(void) signal(SIGHUP, sig_hangup_handler);
	/* 現状の表示 */
	(void) sigaction(SIGHUP, (struct sigaction *) NULL, &sa);
	(void) fprintf(stderr, "SA_ONSTACK=%d\n",(sa.sa_flags&SA_ONSTACK?1:0));
	(void) fprintf(stderr, "SA_RESETHAND=%d\n",(sa.sa_flags&SA_RESETHAND?1:0));
	(void) fprintf(stderr, "SA_NODEFER=%d\n",(sa.sa_flags&SA_NODEFER?1:0));
	(void) fprintf(stderr, "SA_RESTART=%d\n",(sa.sa_flags&SA_RESTART?1:0));
	(void) fprintf(stderr, "SA_SIGINFO=%d\n",(sa.sa_flags&SA_SIGINFO?1:0));
	(void) fprintf(stderr, "signal():end\n");
#else
	(void) sigaction(SIGHUP, (struct sigaction *) NULL, &sa);
	sa.sa_handler = sig_hangup_handler;
	sa.sa_flags = SA_NODEFER;
	(void) sigaction(SIGHUP, &sa, (struct sigaction *) NULL);
	/* 現状の表示 */
	(void) sigaction(SIGHUP, (struct sigaction *) NULL, &sa);
	(void) fprintf(stderr, "sigaction():end\n");
	(void) fprintf(stderr, "SA_ONSTACK=%d\n",(sa.sa_flags&SA_ONSTACK?1:0));
	(void) fprintf(stderr, "SA_RESETHAND=%d\n",(sa.sa_flags&SA_RESETHAND?1:0));
	(void) fprintf(stderr, "SA_NODEFER=%d\n",(sa.sa_flags&SA_NODEFER?1:0));
	(void) fprintf(stderr, "SA_RESTART=%d\n",(sa.sa_flags&SA_RESTART?1:0));
	(void) fprintf(stderr, "SA_SIGINFO=%d\n",(sa.sa_flags&SA_SIGINFO?1:0));
#endif
	/* 5秒おきにカウント表示 */
	for (i = 0;; i++) {
		(void) fprintf(stderr, "count=%d\n",i);
		
		sleep(5);
	}
}
/*  */
/*  */
/*  */
/*  */
/*  */
