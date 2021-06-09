#include	"unp.h"

//传参i的用处在此，为了告诉子进程自身的ID编号以访问cptr数组
pid_t
child_make(int i, int listenfd, int addrlen)
{
	pid_t	pid;
	void	child_main(int, int, int);

	if ( (pid = Fork()) > 0)
		return(pid);		/* parent */

	child_main(i, listenfd, addrlen);	/* never returns */
}

void
child_main(int i, int listenfd, int addrlen)
{
	int				connfd;
	void			web_child(int);
	socklen_t		clilen;
	struct sockaddr	*cliaddr;

	//访问本文件外部即serv02.c的变量cptr
	extern long		*cptr;

	cliaddr = Malloc(addrlen);

	printf("child %ld starting\n", (long) getpid());
	for ( ; ; ) {
		clilen = addrlen;
		connfd = Accept(listenfd, cliaddr, &clilen);

		//该进程完成的连接数在accept返回之后就可以加1了！
		cptr[i]++;

		web_child(connfd);		/* process the request */
		Close(connfd);
	}
}
