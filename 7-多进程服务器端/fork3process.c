/* Printf的输出并不会按照我们代码的顺序进行
具体顺序取决于操作系统层面的调度
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
    pid_t pid1, pid2, pid3;

    // 第一个子进程
    pid1 = fork();
    if (pid1 == 0) {
        printf("Child Process 1, PID: %d\n", getpid());
        // 子进程 1 的逻辑
        // ...
        return 0;
    }

    // 第二个子进程
    pid2 = fork();
    if (pid2 == 0) {
        printf("Child Process 2, PID: %d\n", getpid());
        // 子进程 2 的逻辑
        // ...
        return 0;
    }

    // 第三个子进程
    pid3 = fork();
    if (pid3 == 0) {
        printf("Child Process 3, PID: %d\n", getpid());
        // 子进程 3 的逻辑
        // ...
        return 0;
    }

    // 父进程的逻辑
    printf("Parent Process, PID: %d\n", getpid());

    return 0;
}
