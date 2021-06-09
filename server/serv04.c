/*此版本为了便于向内核不支持多进程阻塞于accept中的系统移植！
  1 所有进程间共同访问的全局变量(用于传递信息或者标志位)，必须定义在共享内存区，这些变量例如有互斥锁和计数器！
  2 使用上锁保护accept，实现任意时刻都只有一个进程阻塞在accept调用里！不上锁LINUX内核也是支持的*/

#include	"unp.h"



static int		nchildren;
static pid_t	*pids;

//引用外部文件定义的函数！
void my_lock_init(char *);

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
		err_quit("usage: serv04 [ <host> ] <port#> <#children>");
	nchildren = atoi(argv[argc-1]);
	pids = Calloc(nchildren, sizeof(pid_t));

	//父进程内初始化一个互斥锁
	my_lock_init(NULL);
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
	exit(0);
}
