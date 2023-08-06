/**
 * 这里展示了poll函数的使用；
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>

#define MAX_EVENTS 2

int main() {
    struct pollfd fds[MAX_EVENTS];
    int timeout = 5000;  // 5 秒超时

    // 监视标准输入
    fds[0].fd = 0;
    fds[0].events = POLLIN;

    // 监视文件描述符 1
    fds[1].fd = 1;
    fds[1].events = POLLIN;

    while (1) {
        int result = poll(fds, MAX_EVENTS, timeout);

        if (result > 0) {
            // 检查标准输入是否有数据可读
            if (fds[0].revents & POLLIN) {
                char buffer[1024];
                int bytesRead = read(0, buffer, sizeof(buffer));
                if (bytesRead > 0) {
                    printf("Read from stdin: %.*s\n", bytesRead, buffer);
                }

                if (strncmp(buffer, "q\n", 2) == 0)  break;
            }

            // 检查文件描述符 1 是否有数据可读
            if (fds[1].revents & POLLIN) {
                char message[] = "Hello, poll!\n";
                write(1, message, sizeof(message));
            }
        } else if (result == 0) {
            printf("Poll timeout.\n");
        } else {
            perror("Poll error");
            break;
        }
    }

    return 0;
}
