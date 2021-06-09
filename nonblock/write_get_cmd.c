#include	"web.h"

void
write_get_cmd(struct file *fptr)
{
	int		n;
	char	line[MAXLINE];

	n = snprintf(line, sizeof(line), GET_CMD, fptr->f_name);

	//这里是给服务器发送http GET指令的地方
	Writen(fptr->f_fd, line, n);
	printf("wrote %d bytes for %s\n", n, fptr->f_name);

	//更新flag！
	fptr->f_flags = F_READING;			/* clears F_CONNECTING */

	//为的是使用select监控是否有服务器的数据到达该fd形成的可读状态
	FD_SET(fptr->f_fd, &rset);			/* will read server's reply */

	if (fptr->f_fd > maxfd)
		maxfd = fptr->f_fd;
}
