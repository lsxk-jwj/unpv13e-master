#include	"unp.h"
#include	<time.h>
//#include	<stdio.h>
/*
1 这里很多函数全是C语言的库函数，可以go to definition查看
2 VSCode和shell配合使用很方便

*/




int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	char				buff[MAXLINE];
	time_t				ticks;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(13);	/* daytime server */
	// \n 在printf中具有重要作用：刷新缓冲区，即即使缓冲区未满也立刻将内容输出！
	
	
	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);
	

	for ( ; ; ) {
		printf("before Accept: The serve is running, waiting for connection!\n");
		connfd = Accept(listenfd, (SA *) NULL, NULL);//accept
		printf("The serve is running, waiting for connection!\n");
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buff, strlen(buff));

		Close(connfd);
	}
}
