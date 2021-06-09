/*
1 str_cli函数的 多线程版本pthread_create() VS 多进程f版本fork()
  都是为了防止事件A阻塞事件B而使用的技巧。
  多线程与多进程都可以创建多个客户服务器套接字对！

*/
//#include	"unp.h"
#include	"unpthread.h"

int
main(int argc, char **argv)
{
	int		sockfd;

	if (argc != 3)
		err_quit("usage: tcpcli <hostname> <service>");

	//使用的是协议无关的tcp字节流套接字版本！
	sockfd = Tcp_connect(argv[1], argv[2]);

	str_cli(stdin, sockfd);		/* do it all */

	exit(0);
}

