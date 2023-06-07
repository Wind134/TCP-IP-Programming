/* 使用标准IO，标准IO函数带缓冲区 */
#include <stdio.h>
#include <time.h>
#define BUF_SIZE 3  // 用最短数组长度构成

int main(int argc, char *argv[])
{
    clock_t start, end;
    double cpu_time_used;

    start = clock();  // 获取开始时间


    FILE * fp1; // 保存在fp1中的是FILE结构体指针
    FILE * fp2; // 保存在fp2中的是FILE结构体指针
    char buf[BUF_SIZE];

    fp1 = fopen("/home/ping/midrop_log.txt", "r");
    if (fp1 == NULL)    printf("File1 Failed Open\n");

    fp2 = fopen("/home/ping/cpy.txt", "w");
    if (fp2 == NULL)    printf("File2 Failed Open\n");

    while (fgets(buf, BUF_SIZE, fp1) != NULL)
        fputs(buf, fp2);

    fclose(fp1);    
    fclose(fp2);

    end = clock();  // 获取结束时间

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;  // 计算 CPU 时间
    printf("程序执行时间：%f 秒\n", cpu_time_used);

    return 0;
}

// 0.233647秒