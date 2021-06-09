#include	"unp.h"


pid_t
child_make(int i, int listenfd, int addrlen)
{
	pid_t	pid;
	void	child_main(int, int, int);

	if ( (pid = Fork()) > 0)
		return(pid);		/* parent */

	child_main(i, listenfd, addrlen);	/* never returns */
}

//各个子进程执行的函数，因为每个子进程都得调用accept函数
//故每个子进程都得给互斥锁上锁和解锁，则这个互斥锁需要定义在共享内存区
void
child_main(int i, int listenfd, int addrlen)
{
	int				connfd;
	void			web_child(int);
	socklen_t		clilen;
	struct sockaddr	*cliaddr;

	//添加引用外部文件的函数的声明!
	void my_lock_wait();
	void my_lock_release();
	extern int *totalConnection;

	cliaddr = Malloc(addrlen);

	printf("child %ld starting\n", (long) getpid());
	for ( ; ; ) {
		clilen = addrlen;

		//使用上锁保护accept，实现任意时刻都只有一个进程阻塞在accept调用里！
		my_lock_wait();
		connfd = Accept(listenfd, cliaddr, &clilen);
		printf("服务器建立连接的总数：%d\n", ++(*totalConnection));
		my_lock_release();

		web_child(connfd);		/* process the request */
		Close(connfd);
	}
}
