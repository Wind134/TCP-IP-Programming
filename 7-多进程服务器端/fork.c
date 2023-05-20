/* 展示了fork的具体使用 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>  // 包含类型的头文件

int gval = 10;  // 全局变量
int main(int argc, char *argv[])
{
    pid_t pid;
    int lval = 20;
    gval++, lval += 5;

    pid = fork();   // 执行fork函数

    if(pid == 0)    gval += 2, lval += 2;   // 如果是子进程
    else gval -= 2, lval -= 2;              // 如果是父进程

    // 观察了输出结果，在这台计算机上是父进程先，子进程后啊
    if(pid == 0)    printf("Child Proc: [%d, %d] \n", gval, lval);   // 如果是子进程
    else printf("Parent Proc: [%d, %d] \n", gval, lval);              // 如果是父进程

    return 0;
}