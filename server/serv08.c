/* include serv08 
  1 由于主线程和所有子线程共享同一进程的变量，包括描述符，则使用一个全局变量的数组存储描述符即可在主子线程间传递
  2 而进程间传递描述符的方式则不一样，要通过UNIX域套接字，如父子进程之间通过socketpair创建的字节流管道
  */
#include	"unpthread.h"
#include	"pthread08.h"

static int			nthreads;
pthread_mutex_t		clifd_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t		clifd_cond = PTHREAD_COND_INITIALIZER;

int
main(int argc, char **argv)
{
	int			i, listenfd, connfd;
	void		sig_int(int), thread_make(int);
	socklen_t	addrlen, clilen;
	struct sockaddr	*cliaddr;

	if (argc == 3)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: serv08 [ <host> ] <port#> <#threads>");
	cliaddr = Malloc(addrlen);

	nthreads = atoi(argv[argc-1]);
	tptr = Calloc(nthreads, sizeof(Thread));
	iget = iput = 0;

	/* 4create all the threads 
	在这里一次性创建线程池*/
	for (i = 0; i < nthreads; i++)
		thread_make(i);		/* only main thread returns */

	Signal(SIGINT, sig_int);

	for ( ; ; ) {
		clilen = addrlen;

		//这里返回的每一个connfd都是所有子线程和主线程间共享的，不需要它定义在pthread_create函数之前，这一点和fork不同！
		connfd = Accept(listenfd, cliaddr, &clilen);
		printf("main 线程的connfd：%d\n", connfd);

		//这里上锁用于保护clifd数组，依靠全局变量clifd数组在主线程和各个子线程之间传递每一个新建立的connfd
		Pthread_mutex_lock(&clifd_mutex);
		clifd[iput] = connfd;

		if (++iput == MAXNCLI)
			iput = 0;
		if (iput == iget)
			err_quit("iput = iget = %d", iput);
		Pthread_cond_signal(&clifd_cond);
		Pthread_mutex_unlock(&clifd_mutex);
	}
}
/* end serv08 */

void
sig_int(int signo)
{
	int		i;
	void	pr_cpu_time(void);

	pr_cpu_time();

	for (i = 0; i < nthreads; i++)
		printf("thread %d, %ld connections\n", i, tptr[i].thread_count);

	exit(0);
}
