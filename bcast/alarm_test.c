#include	"unp.h"

size_t cnt = 0;

static void recv_from(int signo)
{
    printf("定时器执行计数： %ld \n", ++cnt);
    return;
}

int
main(int argc, char **argv)
{
    Signal(SIGALRM, recv_from);

    for(int i = 0; i<5;i++)
    {
        alarm(2);
        //sleep(2);
    }
    //alarm(1);
    sleep(10);
    exit(0);



}