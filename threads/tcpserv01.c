#include	"unpthread.h"

static void	*doit(void *);		/* each thread executes this function */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int
main(int argc, char **argv)
{
	int				listenfd, connfd;
	pthread_t		tid;
	socklen_t		addrlen, len;
	struct sockaddr	*cliaddr;

	if (argc == 2)
		listenfd = Tcp_listen(NULL, argv[1], &addrlen);
	else if (argc == 3)
		listenfd = Tcp_listen(argv[1], argv[2], &addrlen);
	else
		err_quit("usage: tcpserv01 [ <host> ] <service or port>");

	cliaddr = Malloc(addrlen);

	for ( ; ; ) {
		len = addrlen;
		//每一个新建立的连接都会使Accept返回一个新的connfd，同时建立一个新的TCP套接字对
		//这里有没有必要加锁来解决同步的问题呢???

		pthread_mutex_lock(&mutex);
		connfd = Accept(listenfd, cliaddr, &len);
		printf("the connfd of the new connection is %d\n", connfd);
		Pthread_create(&tid, NULL, &doit, (void *) connfd);
		pthread_mutex_unlock(&mutex);
	}
}

//每一个新建立的连接都是由doit线程处理！
static void *
doit(void *arg)
{
	Pthread_detach(pthread_self());
	str_echo((int) arg);	/* same function as before */

	//都是由doit线程关闭连接，而不是主线程
	Close((int) arg);		/* done with connected socket */
	return(NULL);
}
