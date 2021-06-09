/* include serv06 
  现场创建新线程处理新连接的版本
  问题1：该版本存在问题，当客户测试程序每个子进程发起的连接数过多时，连接就会出错！
  解决办法1：要使用线程安全版本的函数，即web_child_r.c和readine_r.c定义的readline_r等！
*/
#include	"unpthread.h"

int
main(int argc, char **argv)
{
	int				listenfd, connfd;
	void			sig_int(int);
	void			*doit(void *);
	pthread_t		tid;
	socklen_t		clilen, addrlen;
	struct sockaddr	*cliaddr;

	if (argc == 2)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 3)
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: serv06 [ <host> ] <port#>");
	cliaddr = Malloc(addrlen);

	Signal(SIGINT, sig_int);

	for ( ; ; ) {
		clilen = addrlen;
		connfd = Accept(listenfd, cliaddr, &clilen);

		Pthread_create(&tid, NULL, &doit, (void *) connfd);
	}
}

void *
doit(void *arg)
{
	void	web_child(int);

	Pthread_detach(pthread_self());
	web_child((int) arg);
	Close((int) arg);
	return(NULL);
}
/* end serv06 */

void
sig_int(int signo)
{
	void	pr_cpu_time(void);

	pr_cpu_time();
	exit(0);
}
