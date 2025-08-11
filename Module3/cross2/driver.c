#include "driver.h"
#include <bits/time.h>
#include <bits/types/struct_itimerspec.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <sys/types.h>
#include <unistd.h>

void start_driver(int read_fd, int write_fd)
{
    fcntl(read_fd, F_SETFL, O_NONBLOCK);
    fcntl(write_fd, F_SETFL, O_NONBLOCK);

    int timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (timer_fd == -1) 
    {
        perror("timerfd_create error");
        return;
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) 
    {
        perror("epoll_create error");
        close(timer_fd);
        return;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = read_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, read_fd, &ev);

    ev.data.fd = timer_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, timer_fd, &ev);

    DriverStatus status = AVAILABLE;

    char buf[BUF_SIZE];
    char response[BUF_SIZE];
    while (1) 
    {
        struct epoll_event events[2];
        int nfds = epoll_wait(epoll_fd, events, 2, -1);
        if (nfds == -1)
        {
            perror("epoll_wait error");
            break;
        }

        for (int i = 0; i < nfds; i++)
        {
            int fd = events[i].data.fd;

            if (fd == read_fd)
            {
                ssize_t count = read(read_fd, buf, sizeof(buf) - 1);
                if (count == 0)
                {
                    close(timer_fd);
                    close(read_fd);
                    return;
                }
                else if (count < 0)
                {
                    perror("read pipe error");
                    continue;
                }
                buf[count] = '\0';

                int task_seconds = 0;
                if (sscanf(buf, "TASK %d", &task_seconds) == 1)
                {
                    if (status == AVAILABLE)
                    {
                        struct itimerspec specs = {0};
                        specs.it_value.tv_sec = task_seconds;
                        timerfd_settime(timer_fd, 0, &specs, NULL);
                        status = BUSY;

                        snprintf(response, sizeof(response), "BUSY\n");
                        write(write_fd, response, strlen(response));
                    }
                    else
                    {
                        struct itimerspec curr_value;
                        timerfd_gettime(timer_fd, &curr_value);
                        snprintf(response, sizeof(response), "Busy %d\n", (int)curr_value.it_value.tv_sec);
                        write(write_fd, response, strlen(response));
                    }
                }
                else if (strncmp(buf, "STATUS", 6) == 0)
                {
                    if (status == AVAILABLE) 
                    {
                        snprintf(response, sizeof(response), "AVAILABLE\n");
                    }
                    else 
                    {
                        struct itimerspec curr_value;
                        timerfd_gettime(timer_fd, &curr_value);
                        snprintf(response, sizeof(response), "BUSY %d\n", (int)curr_value.it_value.tv_sec);
                    }
                    write(write_fd, response, strlen(response));
                }
                else if (strncmp(buf, "REMOVE", 6) == 0)
                {
                    close(timer_fd);
                    close(epoll_fd);
                    return;
                }
                else 
                {
                    const char* msg = "Неизвестная команда\n";
                    write(write_fd, msg, strlen(msg));
                }
            }
            else if (fd == timer_fd) 
            {
                uint64_t termination;
                ssize_t count = read(timer_fd, &termination, sizeof(termination));
                if (count != sizeof(termination)) 
                {
                    perror("timerfd read");
                    continue;
                }

                status = AVAILABLE;
                const char* msg = "AVAILABLE\n";
                write(write_fd, msg, strlen(msg));
            }
        }
    }

    close(timer_fd);
    close(epoll_fd);
}