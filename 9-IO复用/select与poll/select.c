/* 
 * 这个源码比较简单，仅仅是关注了一个文件描述符的一个监视项
 * 怎么同时关注到多个监视项呢，就是将这多个剪视频加入到位图fd_set
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <string.h>
#define BUF_SIZE 30

int main(int argc, char * argv[])
{
    fd_set reads, temps;
    int result, str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;

    FD_ZERO(&reads);    // 初始化reads变量
    FD_SET(0, &reads);  // 0是标准输入文件描述符，将该位设置为1


    while (1)
    {
        temps = reads;  // 为了记住初始值，必须经过这种复制过程
        timeout.tv_sec = 5;     // 设置超时事件，秒数为5s
        timeout.tv_usec = 0;    // 毫秒数设置为0，每次循环都会初始化

        // 仅仅关注"是否存在待读取数据"的文件描述
        // 而没有关注其他事件监视项
        // 因此这个例子好理解
        // 同时select第一个参数的值等于最大的文件描述符值+1
        result = select(1, &temps, 0, 0, &timeout); // 该函数返回一个整数，有控制台输入数据则返回大于0的整数

        printf("The result value is: %d", result);

        if (result == -1)
        {
            puts("select() error!");
            break;
        }
        else if(result == 0)    puts("\nTime-out!");
        else
        {
            if (FD_ISSET(0, &temps))    // 验证发生变化的文件描述符是否为标准输入，若是，则读取数据并输出
            {
                str_len = read(0, buf, BUF_SIZE);   // 0是标准输入
                buf[str_len] = 0;
                printf("\nMessage from console: %s", buf);

                if (strcmp(buf, "quit\n") == 0)   break;    // 输入quit就退出
            }   
        }
    }
    return 0;
}