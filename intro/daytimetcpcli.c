#include	"unp.h"

int
main(int argc, char **argv)
{
	int					sockfd, n;
	char				recvline[MAXLINE + 1];
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: a.out <IPaddress>");

	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		err_sys("socket error");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(13);	/* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_quit("inet_pton error for %s", argv[1]);

	//可以改用非阻塞connect_nonb进行TCP连接！
	if (connect_nonb(sockfd, (SA *) &servaddr, sizeof(servaddr), 10) < 0)
		err_sys("connect error");


	struct sockaddr_in selfaddr;
	socklen_t len = sizeof(selfaddr);
	char name[MAXLINE];
	while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
		//让客户端显示内核为自己分配的IP和端口号
		//由于本客户程序并无bind自身的IP和端口号，于是想要查询一下！
		if(getsockname(sockfd, (SA *) &selfaddr, &len) == 0)
		{	
			inet_ntop(AF_INET, &selfaddr.sin_addr, name, sizeof(name));//转换一下IP地址的格式,输出保存在name！
			printf("the client IP is %s;port is %d \n", name, ntohs(selfaddr.sin_port));//转换网络-主机字节序
		}
		recvline[n] = 0;	/* null terminate */
		if (fputs(recvline, stdout) == EOF)
			err_sys("fputs error");
	}
	if (n < 0)
		err_sys("read error");

	exit(0);
}
