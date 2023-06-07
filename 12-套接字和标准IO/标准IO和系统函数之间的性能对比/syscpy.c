/* 使用系统函数进行内容复制，不带缓冲区 */
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#define BUF_SIZE 3  // 用最短数组长度构成

int main(int argc, char *argv[])
{
    clock_t start, end;
    double cpu_time_used;

    start = clock();  // 获取开始时间

    char buf[BUF_SIZE];
    int fd1 = open("/home/ping/midrop_log.txt", O_RDONLY);
    int fd2 = open("/home/ping/cpy.txt", O_WRONLY|O_CREAT|O_TRUNC);
    int len;
    

    while ((len = read(fd1, buf, sizeof(buf))) > 0)
        write(fd2, buf, len);

    close(fd1);    
    close(fd2);

    end = clock();  // 获取结束时间

    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;  // 计算 CPU 时间
    printf("程序执行时间：%f 秒\n", cpu_time_used);;

    return 0;
}

// 程序执行时间：5.769706 秒