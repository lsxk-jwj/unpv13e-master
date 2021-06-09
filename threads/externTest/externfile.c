#include "externfile2.h"


int main()
{
    int c =9, d = 10;
    // extern int arg1;
    // extern int a;
    // extern int add2num(int, int);
    extern int arg2;//arg2是定义在另一文件中的局部变量，不可用extern访问！

    printf("a = %d\n", a);
    printf("the func result is %d\n", add2num(c, d));
    printf("arg1 = %d\n", arg1);
}