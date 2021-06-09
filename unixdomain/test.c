#include	"unp.h"

int main ()
{
    char *buff;
    //声明一个指针之后必须分配空间才能使用strcpy函数
    buff = (char *)malloc(104);
    strcpy(buff, UNIXSTR_PATH);

    //buff = UNIXSTR_PATH;
    printf("buff is %s；the size of buff is %ld \n", buff, strlen(buff));
    exit(0);
}