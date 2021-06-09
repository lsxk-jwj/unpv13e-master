/* include child_make */
#include	"unp.h"

pid_t
child_make(int i, int listenfd, int addrlen)
{
	pid_t	pid;
	void	child_main(int, int, int);

	if ( (pid = Fork()) > 0)
		return(pid);		/* parent */
	
	//每次fork的子进程将继续执行child_main函数
	child_main(i, listenfd, addrlen);	/* never returns */
}
/* end child_make */

/* include child_main */
void
child_main(int i, int listenfd, int addrlen)
{
	int				connfd;
	void			web_child(int);
	socklen_t		clilen;
	struct sockaddr	*cliaddr;

	cliaddr = Malloc(addrlen);

	printf("child %ld starting\n", (long) getpid());

	//当客户发起连接时，真正的由listenfd创建每一个connfd是在这里
	//子进程将永远在这里 连接成功，web_child处理完成，关闭套接字断开连接，再次连接成功...直至被父进程杀死！
	for ( ; ; ) {
		clilen = addrlen;
		
		//多个进程可以在同一个监听套接字上调用accept
		connfd = Accept(listenfd, cliaddr, &clilen);

		web_child(connfd);		/* process the request */
		Close(connfd);
	}
}
/* end child_main */
