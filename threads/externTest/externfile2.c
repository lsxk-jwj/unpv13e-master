//#include "externfile2.h"


/*
在此源文件里定义的非static变量和函数，可供externfile.c里引用，只要externfile.c里使用前加上extern声明即可
static 表示静态的变量，限制此变量作用域在一个源文件内，其他文件不能用extern来访问此变量，不仅适用于变量，函数也可以。
*/
int a = 10;
int add2num(int num1, int num2)
{
    int arg2 = 30;
    return num1+num2;
}

int arg1 = 20;
