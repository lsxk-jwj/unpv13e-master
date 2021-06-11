#include	"unp.h"

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;
	int flag = 1, len = sizeof(int);
	

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	//在服务器重启时想要再次bind上次用过的端口如9877，那么必须在bind之前设定SO_REUSEADDR的选项，否则就会产生bind error: Address already in use的错误！
	if( setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &flag, len) < 0){
       err_sys("setsockopt error");
       exit(1);
    }


	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	// the serve process is always included in the for loop to handle the client request all the time 
	// 这是一种有新客户连接时就现场fork一个子进程的方法！
	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);//accept可以直接获取client的地址

		if ( (childpid = Fork()) == 0) {	/* child process */
			Close(listenfd);	/* close listening socket */
			str_echo(connfd);	/* process the request */
			exit(0);
		}
		Close(connfd);			/* parent closes connected socket */
	}
}
