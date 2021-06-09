#include	"unp.h"

//仍然使用默认的阻塞形式的connect连接。
//实现当服务器程序未运行时，一直尝试connect，不终止客户进程！
//当连接建立完成后，服务器程序终止时，客户程序也不终止，而是再次一直尝试connect!

int
main(int argc, char **argv)
{
	int					i, sockfd;
	struct sockaddr_in	servaddr;
	void str_cli(FILE *fp, int sockfd);

	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");

	for (; ; ) {
		sockfd = Socket(AF_INET, SOCK_STREAM, 0);

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(SERV_PORT);
		Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);
		
		if(connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) == 0)//连接成功
		{
			str_cli(stdin, sockfd);		/* do it all */
			close(sockfd);
		}
			
		else
		{
			close(sockfd);
			sleep(1);//间隔1秒再发起
		}
		
	}

	

	//exit(0);
}

void
str_cli(FILE *fp, int sockfd)
{
	char	sendline[MAXLINE], recvline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Writen(sockfd, sendline, strlen(sendline));

		if (Readline(sockfd, recvline, MAXLINE) == 0)
			//err_quit("str_cli: server terminated prematurely");
			{
				printf("服务器程序终止，尝试再次建立连接...\n");
				break;
			}
			

		Fputs(recvline, stdout);
	}
}