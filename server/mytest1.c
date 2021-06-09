#include	"unp.h"

int main(int argc, char **agrv)
{
    char request[4];
    int n = 5;
    // snprintf(request, sizeof(request), "%d", n);

    // printf("now request is %s, the size of request is %d, the length is %d\n", request, sizeof(request), strlen(request));
    
    // //int res = ("100" == request);
    // printf("res is %ld\n", atol(request));

    // char ttt[100];
    // printf("ttt is %s\n", ttt);

    int *ptr;
    //calloc类似于 new 一个动态数组
    ptr = Calloc(n, sizeof(int));
    for(int i = 0; i<n; i++)
    {
        printf("ptr[%d] is %d\n", i, ptr[i]);
    }

    return 0;
}