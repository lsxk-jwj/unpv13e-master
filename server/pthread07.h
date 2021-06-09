typedef struct {
  pthread_t		thread_tid;		/* thread ID */
  long			thread_count;	/* # connections handled */
} Thread;
Thread	*tptr;		/* array of Thread structures; calloc'ed */

//还声明了一些全局变量！
int				listenfd, nthreads;
socklen_t		addrlen;
pthread_mutex_t	mlock;
