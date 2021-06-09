#include <stdio.h>

/*
若在头文件里对变量和函数的声明前加上extern时，使用这些外部定义的变量的源文件如externfile就不必加上extern声明了
头文件一般只做声明，不做定义！
*/

extern int a;
//extern int add2num(int num1, int num2);
extern int arg1;

