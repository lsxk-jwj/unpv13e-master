/*
1 多线程pthread_create() VS 多进程fork()
2 可以使用两个线程同时处理两个系统调用的阻塞，避免了select的使用
3 注意struct的用法和class有些不同！

*/
//#include	"unp.h"
#include	"unpthread.h"

//将线程执行函数的参数封装在dstruct结构中，此方法可以避免使用全局变量！
struct dstruct
{
	int sockfd;
	FILE *fp;
};

int
main(int argc, char **argv)
{
	int		sockfd;

	if (argc != 3)
		err_quit("usage: tcpcli <hostname> <service>");

	//使用的是协议无关的tcp字节流套接字版本！
	sockfd = Tcp_connect(argv[1], argv[2]);

	str_cli(stdin, sockfd);		/* do it all */

	exit(0);
}

void *copyto(void *);

void str_cli(FILE *fp_arg, int sockfd_arg)
{
	char		recvline[MAXLINE];
	pthread_t	tid;

	struct dstruct struc;
	struc.sockfd = sockfd_arg;
	struc.fp = fp_arg;

	Pthread_create(&tid, NULL, &copyto, &struc);

	//主线程会阻塞在readline上，等待该套接字变成可读状态！
	while (Readline(sockfd_arg, recvline, MAXLINE) > 0)
		Fputs(recvline, stdout);
}

//copyto线程负责处理标准输入，会阻塞在标准输入上
void* copyto(void *arg)
{
	char	sendline[MAXLINE];
	struct dstruct *copyds = (struct dstruct *)arg;//通用指针使用前要强制转换；

	while (Fgets(sendline, MAXLINE, copyds->fp) != NULL)
		Writen(copyds->sockfd, sendline, strlen(sendline));

	Shutdown(copyds->sockfd, SHUT_WR);	/* EOF on stdin, send FIN */

	return(NULL);
		/* 4return (i.e., thread terminates) when EOF on stdin */
}
