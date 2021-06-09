#include	"unp.h"

static int		nchildren;
static pid_t	*pids;

//共享内存区数组cptr，调用meter函数分配;cptr可由所有子进程和父进程共享！
long			*cptr, *meter(int);	/* for counting #clients/child */
//测试

int
main(int argc, char **argv)
{
	int			listenfd, i;
	socklen_t	addrlen;
	void		sig_int(int);
	pid_t		child_make(int, int, int);

	if (argc == 3)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 4)
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: serv02 [ <host> ] <port#> <#children>");
	nchildren = atoi(argv[argc-1]);
	pids = Calloc(nchildren, sizeof(pid_t));

	//给共享内存区变量cptr分配空间
	cptr = meter(nchildren);
	
	for (i = 0; i < nchildren; i++)
		pids[i] = child_make(i, listenfd, addrlen);	/* parent returns */

	Signal(SIGINT, sig_int);

	for ( ; ; )
		pause();	/* everything done by children */
}

void
sig_int(int signo)
{
	int		i;
	void	pr_cpu_time(void);

		/* terminate all children */
	for (i = 0; i < nchildren; i++)
		kill(pids[i], SIGTERM);
	while (wait(NULL) > 0)		/* wait for all children */
		;
	if (errno != ECHILD)
		err_sys("wait error");

	pr_cpu_time();

	//显示每个内核调度后分配给每个子进程的连接数！
	for (i = 0; i < nchildren; i++)
		printf("child %d, %ld connections\n", i, cptr[i]);

	exit(0);
}
