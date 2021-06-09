#include	"unpthread.h"
#include	"pthread08.h"

//源文件同样放的只是函数的定义

void
thread_make(int i)
{
	//这样的函数声明作用是：表明此函数的定义在本函数或者本文件外部，函数不需要extern，若是变量则需要加上extern！
	void	*thread_main(void *);

	Pthread_create(&tptr[i].thread_tid, NULL, &thread_main, (void *) i);
	return;		/* main thread returns */
}

void *
thread_main(void *arg)
{
	int		connfd;
	void	web_child(int);

	printf("thread %d starting\n", (int) arg);
	for ( ; ; ) {

		//这里上锁用于保护clifd数组，一旦主线程释放了clifd_mutex,某个子线程就会在此获取该锁，
    	Pthread_mutex_lock(&clifd_mutex);
		while (iget == iput)
			Pthread_cond_wait(&clifd_cond, &clifd_mutex);

		connfd = clifd[iget];	/* connected socket to service */
		printf("子线程%d 收到的connfd：%d\n", (int)arg, connfd);

		if (++iget == MAXNCLI)
			iget = 0;
		Pthread_mutex_unlock(&clifd_mutex);

		tptr[(int) arg].thread_count++;
		web_child(connfd);		/* process request */
		Close(connfd);
	}
}
