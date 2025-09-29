#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "Task.hpp"

// master
class Channel
{
public:
    Channel(int wfd, pid_t id, const std::string& name)
        : _wfd(wfd)
        , _subprocessid(id)
        , _name(name)
    {}

    int GetWfd() const { return _wfd; }

    pid_t GetSubProcessId() const { return _subprocessid; }

    std::string GetName() const { return _name; }

    void CloseChannel()
    {
        close(_wfd);
    }

    void Wait()
    {
        pid_t rid = waitpid(_subprocessid, nullptr, 0);
        if (rid > 0)
        {
            std::cout << "wait subprocess done, rid: " << rid << std::endl;
        }
    }

    ~Channel() {}

private:
    int _wfd;
    pid_t _subprocessid;
    std::string _name;
};

// void work(int rfd)
// {
//     while (true)
//     {
//         int task_code = -1;
//         int n = read(rfd, &task_code, sizeof(task_code));

//         if (n == sizeof(int))
//         {
//             std::cout << "pid is : " << getpid() << ", handle task" << std::endl;
//             ExcuteTask(task_code);
//         }
//         else if (n == 0)
//         {
//             std::cout << "master close the wfd, subprocess: " << getpid() << " exit" << std::endl;
//             break;
//         }
//     }
// }

// 形参类型和命名规范
// const &: 输入
// &: 输入输出
// *: 输出
// task_t task: 回调函数
void CreateChannelAndSub(int num, std::vector<Channel>* channels, task_t task)
{
    // bug
    for(int i = 0; i < num; ++i)
    {
        // 1.创建管道
        int pipefd[2] = {0};
        int n = pipe(pipefd);
        if (n < 0) exit(1);

        // 2.创建子进程
        pid_t id = fork();
        if (id == 0)
        {
            // child -- read
            close(pipefd[1]); // 关闭写端
            dup2(pipefd[0], 0); // rfd -> 0 把管道的读端，重定向到标准输入
            task();

            close(pipefd[0]);
            exit(0);
        }

        // 3.构建Channel
        std::string channel_name = "Channel_" + std::to_string(i);
        
        // parent
        close(pipefd[0]); // 关闭读端
        channels->push_back(Channel(pipefd[1], id, channel_name));
    }
}

// 轮询调度 -- 负载均衡
int NextChannel(int channel_num)
{
    static int next = 0;
    int channel_index = next;
    next = (next + 1) % channel_num;
    return channel_index;
}

void SendTaskCode(const Channel& channel , int task_code)
{
    write(channel.GetWfd(), &task_code, sizeof(task_code));
}

void CtrlProcess(const std::vector<Channel>& channels)
{
    sleep(1);

    int task_code = SelectTask();

    int channel_index = NextChannel(channels.size());

    SendTaskCode(channels[channel_index], task_code);

    std::cout << std::endl;

    std::cout << "task_code: " << task_code
              << ", channel: " << channels[channel_index].GetName()
              << " subprocess id: " << channels[channel_index].GetSubProcessId() << std::endl;

}

void CtrlProcessLoop(const std::vector<Channel>& channels, int looptimes = -1)
{
    if (looptimes > 0)
    {
        while (looptimes--)
        {
            CtrlProcess(channels);
        }
    }
    else
    {
        while (true)
        {
            CtrlProcess(channels);
        }
    }
}

void CleanUpChannel(std::vector<Channel>& channels)
{
    for (auto& channel : channels)
    {
        channel.CloseChannel();
        channel.Wait();
    }

    // for (auto& channel : channels)
    // {
    //     channel.CloseChannel();
    // }

    // for (auto& channel : channels)
    // {
    //     channel.Wait();
    // }
}

// ./processpool 5
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "usage: ./processpool subprocessnum" << std::endl;
        return 1;
    }
    int num = std::stoi(argv[1]);

    LoadTasks();

    std::vector<Channel> channels;
    // 1.创建信道和子进程
    CreateChannelAndSub(num, &channels, work);
    
    // 2.通过channel控制子进程
    CtrlProcessLoop(channels, 5);
    
    // 3.回收管道和子进程 a.关闭所有的写端 b.回收子进程
    CleanUpChannel(channels);

    return 0;
}