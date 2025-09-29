#pragma once

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>

#define TASK_NUM 3

typedef void (*task_t)(); // task_t 函数指针类型

task_t tasks[TASK_NUM];

void Print()
{
    std::cout << "I am print task..." << std::endl;
}

void Download()
{
    std::cout << "I am download task..." << std::endl;
}

void Upload()
{
    std::cout << "I am upload task..." << std::endl;
}
void LoadTasks()
{
    srand(time(nullptr) ^ getpid()); // 每个进程的任务不一样
    tasks[0] = Print;
    tasks[1] = Download;
    tasks[2] = Upload;
}

void ExcuteTask(int index)
{
    if (index < 0 || index >= TASK_NUM)
    {
        std::cerr << "index error..." << std::endl;
        return;
    }

    tasks[index]();
}

int SelectTask()
{
    return rand() % TASK_NUM;
}

void work()
{
    while (true)
    {
        int task_code = -1;
        int n = read(0, &task_code, sizeof(task_code));

        if (n == sizeof(int))
        {
            std::cout << "pid is : " << getpid() << ", handle task" << std::endl;
            ExcuteTask(task_code);
        }
        else if (n == 0)
        {
            std::cout << "master close the wfd, subprocess: " << getpid() << " exit" << std::endl;
            break;
        }
    }
}