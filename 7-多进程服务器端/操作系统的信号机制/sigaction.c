/* vscode提示不允许使用不完整的类型，但是编译可以通过，暂且忽略这个问题 */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

// struct sigaction
// {
//     void (*sa_handler)(int);	// sa_handler成员保存信号处理函数的指针值
//     sigset_t sa_mask;			// 初始化为0，这两个成员用于指定信号相关的选项和特性，后面介绍这些参数
//     int sa_flags;
// };

void timeout(int sig)
{
    if(sig == SIGALRM)  puts("Time out!");

    alarm(2);      // 约定信号发生时间剩2s
}


int main()
{
    struct sigaction act;

    act.sa_handler = timeout;

    sigemptyset(&act.sa_mask);  // 将sa_mask成员的所有位初始化为0

    act.sa_flags = 0;
    
    sigaction(SIGALRM, &act, 0);
    alarm(2);   // 约定信号发生时间剩2s，仅针对SIGALRM

    // 接下来约定2s调用timout
    // 进入for循环之后，由于沉睡100s，在沉睡的过程中会出现timeout函数的调用
    // 而每次调用timeout函数都又会约定2s产生SIGALRM信号，从而再调用timeout

    for (int i = 0; i < 3; i++)
    {
        puts("wait...");
        sleep(100);
    }
    return 0;
}
/* wait...  进入循环，输出wait...然后进入沉睡状态
* Time out! 首次约定的2s到了，调用timeout函数，打印出Time out!信息，为了调用信号处理器，OS将唤醒处于阻塞态的进程
* wait...   再次执行循环，又进入沉睡状态
* Time out! 又到了约定的2s，再次打印信息，又唤醒沉睡状态的进程
* wait...   同上，刚好这个过程重复3次，因为循环有3次
* Time out! 
*/