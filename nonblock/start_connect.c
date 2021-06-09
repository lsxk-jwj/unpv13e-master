#include	"web.h"

void
start_connect(struct file *fptr)
{
	int				fd, flags, n;
	struct addrinfo	*ai;

	ai = Host_serv(fptr->f_host, SERV, 0, SOCK_STREAM);

	fd = Socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	fptr->f_fd = fd;
	printf("start_connect for %s, fd %d\n", fptr->f_name, fd);

		/* 4Set socket nonblocking */
	flags = Fcntl(fd, F_GETFL, 0);
	Fcntl(fd, F_SETFL, flags | O_NONBLOCK);

		/* 4Initiate nonblocking connect to the server. */
	if ( (n = connect(fd, ai->ai_addr, ai->ai_addrlen)) < 0) {
		if (errno != EINPROGRESS)
			err_sys("nonblocking connect error");

		//更新该fptr的f_flags为F_CONNECTING
		fptr->f_flags = F_CONNECTING;

	//这个分支里fd被同时添加到了rset和wset，为的是使用select监控该非阻塞连接是否正在进展！
		FD_SET(fd, &rset);			/* select for reading and writing */
		FD_SET(fd, &wset);
		if (fd > maxfd)
			maxfd = fd;

	//这个分支里fd被添加到了rset，为的是使用select监控是否有服务器的数据到达该fd形成的可读状态！
	} else if (n >= 0)				/* connect is already done */
		write_get_cmd(fptr);	/* write() the GET command */
}
