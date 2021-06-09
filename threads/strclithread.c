#include	"unpthread.h"

void	*copyto(void *);

//static关键字可以防止其他文件访问这些变量！可以供本源文件的所有函数（分别可以在不同的线程里执行）访问
//必要时全局变量需要解决同步问题！
static int	sockfd;		/* global for both threads to access */
static FILE	*fp;

void
str_cli(FILE *fp_arg, int sockfd_arg)
{
	char		recvline[MAXLINE];
	pthread_t	tid;

	sockfd = sockfd_arg;	/* copy arguments to externals */
	fp = fp_arg;

	Pthread_create(&tid, NULL, copyto, NULL);

	//主线程会阻塞在readline上，等待该套接字变成可读状态！
	while (Readline(sockfd, recvline, MAXLINE) > 0)
		Fputs(recvline, stdout);
}

//copyto线程负责处理标准输入，会阻塞在标准输入上
void *
copyto(void *arg)
{
	char	sendline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != NULL)
		Writen(sockfd, sendline, strlen(sendline));
	
	//多线程里sockfd的引用计数还是1，但此时调用close()还是不对why
	//Close(sockfd);
	Shutdown(sockfd, SHUT_WR);	/* EOF on stdin, send FIN */
	//exit(0);
		/* 4return (i.e., thread terminates) when EOF on stdin */
}
