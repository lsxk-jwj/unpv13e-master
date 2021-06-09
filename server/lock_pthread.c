/* include my_lock_init 
  1 所有进程间共同访问的全局变量(用于传递信息或者标志位)，必须定义在共享内存区，这些变量例如有互斥锁和计数器！

*/
#include	"unpthread.h"
#include	<sys/mman.h>

static pthread_mutex_t	*mptr;	/* actual mutex will be in shared memory */
int *totalConnection;

/* 使用普通全局变量的锁在子进程之间共享也可以，但这是因为accept不上锁也是支持的！所以还是必须定义在共享内存区*/
//pthread_mutex_t	mptr = PTHREAD_MUTEX_INITIALIZER;

void
my_lock_init(char *pathname)
{
	int		fd;
	pthread_mutexattr_t	mattr;

	fd = Open("/dev/zero", O_RDWR, 0);

	/*将锁mptr置于共享内存区，用于测试的计数器也应该在共享内存区！
	  mmap函数：将文件或者设备空间映射到共享内存区。*/
	mptr = Mmap(0, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE,
				MAP_SHARED, fd, 0);
	totalConnection	= Mmap(0, sizeof(int), PROT_READ | PROT_WRITE,
				MAP_SHARED, fd, 0);
	Close(fd);

	//设置该锁的属性，使之用于进程间的上锁，默认是同一进程中线程之间的上锁！
	Pthread_mutexattr_init(&mattr);
	Pthread_mutexattr_setpshared(&mattr, PTHREAD_PROCESS_SHARED);
	Pthread_mutex_init(mptr, &mattr);
}
/* end my_lock_init */

/* include my_lock_wait */
void
my_lock_wait()
{
	Pthread_mutex_lock(mptr);
}

void
my_lock_release()
{
	Pthread_mutex_unlock(mptr);
}
/* end my_lock_wait */
