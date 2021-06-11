#include	"unp.h"

void
str_cli(FILE *fp, int sockfd)
{
	char	sendline[MAXLINE], recvline[MAXLINE];

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Writen(sockfd, sendline, strlen(sendline));

		//若是服务器事先终止，那么上一行的writen就是在向一个已接收fin的sockfd写入，导致该sockfd收到一个rst，此时的readline去读该sockfd将会返回0，若是再write这个sockfd
		//将会返回一个错误epipe。同时内核会给该进程发送sigpipe信号！
		if (Readline(sockfd, recvline, MAXLINE) == 0)
			err_quit("str_cli: server terminated prematurely");

		Fputs(recvline, stdout);
	}
}
