#include	"unp.h"
//只是放在这个c文件里，不能直接include它
void
sig_chld(int signo)
{
	pid_t	pid;
	int		stat;

	//pid和stat全是wait函数的输出量！PID是已终止子进程的号；stat是一个进程终止的状态
	pid = wait(&stat);
	printf("child %d terminated\n", pid);

	//看似循环调用wait也是可以的！
	//while ( (pid = wait(&stat)) > 0)
		//printf("child %d terminated\n", pid);

	
	return;
}
