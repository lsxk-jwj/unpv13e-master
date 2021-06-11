#include	"unp.h"

void
sig_chld(int signo)
{
	pid_t	pid;
	int		stat;

	//使用waitpid而不是wait函数，指定WNOHANG选项告知waitpid,在没有已终止的子进程时不要阻塞！
	//同时产生了5个sigchld信号，因为信号是不排队的，所以信号处理函数sig_chld只执行一次，但是waitpid函数用了while循环，它可以被执行多次！
	while ( (pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\n", pid);
	return;
}
