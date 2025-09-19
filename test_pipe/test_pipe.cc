#include <iostream>
#include <string>
#include <cerrno>  // errno.h
#include <cstring> // string.h
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// fork之后子进程是能拿到父进程的数据的 -- 可以通信吗？写时拷贝，对方看不到

const int size = 1024;

std::string getOtherMessage()
{
    static int cnt = 0;
    std::string msgid = std::to_string(cnt++);
    
    pid_t self_id = getpid();
    std::string message = "hello, i am process " + std::to_string(self_id) + ", msgid: " + msgid;

    return message;
}

// 子进程写
void SubProcessWrite(int wfd)
{
    int pipesize = 0;
    std::string message = "father, this is subprocess!";
    char c = 'a';
    while (true)
    {
        std::cout << "write message: " << std::endl;
        std::string info = message + getOtherMessage(); // 子进程发给父进程的message
        write(wfd, info.c_str(), info.size()); // 写入管道的时候，没有写入\0 -- 没必要

        sleep(5); // 子进程写慢点
        // write(wfd, &c, 1);
        // std::cout << "pipesize: " << ++pipesize << "write charactor is: " << c++ << std::endl;
        // if (c == 'G') break;

        // sleep(1);
    }
    
    std::cout << "subprocess write done.." << std::endl;
}

void FatherProcessRead(int rfd)
{
    char inbuffer[size]; // c99, gnu g99
    while (true)
    {
        // sleep(1);
        std::cout << "father read data from pipe" << std::endl;
        // sleep(500);
        ssize_t n = read(rfd, inbuffer, sizeof(inbuffer) - 1);
        if (n > 0)
        {
            inbuffer[n] = 0; // == '\0'
            std::cout << "father read done, message is: " << inbuffer << std::endl;
        }
        else if (n == 0)
        {
            // 如果read的返回值为0，表示写端关闭了，读到了文件的结尾
            std::cout << "pipe has been closed, no data to read, father get val: " << n << std::endl;
            break;
        }
        else
        {
            std::cerr << "read error: " << strerror(errno) << std::endl;
            break;
        }
        
        // sleep(1);
        // break;
    }
}

int main()
{
    // 1. 创建管道
    int pipefd[2];
    int n = pipe(pipefd); // 输出型参数，wfd，rfd
    if (n != 0)
    {
        std::cerr << "errno: " << errno << " "
                  << "pipe error: " << strerror(errno) << std::endl;
        return 1;
    }

    // pipefd[0] 读端 pipefd[1] 写端
    std::cout << "pipefd[0]: " << pipefd[0] << ", pipefd[1]: " << pipefd[1] << std::endl;
    sleep(1);

    // 2. 创建子进程
    pid_t id = fork();
    if (id == 0)
    {
        std::cout << "子进程关闭不需要的fd了，begin send messages" << std::endl;
        sleep(1);

        // 子进程 -- write
        // 3. 关闭读端
        close(pipefd[0]);

        // if (fork() > 0) exit(0); // 具有血缘关系，也能通信

        SubProcessWrite(pipefd[1]);

        close(pipefd[1]);
        exit(0);
    }

    std::cout << "父进程关闭不需要的fd了，begin receive messages" << std::endl;
    sleep(1);

    // 父进程 -- read
    // 3. 关闭写端
    close(pipefd[1]);

    FatherProcessRead(pipefd[0]);

    std::cout << "5s later, father close rfd" << std::endl;
    sleep(5);
    close(pipefd[0]);

    int status = 0;
    pid_t rid = waitpid(id, &status, 0);
    if (rid > 0)
    {
        std::cout << "wait child process done, exit sig: " << (status & 0x7f) << std::endl;
        std::cout << "wait child process done, exit code(ign) " << ((status >> 8) & 0xff) << std::endl;
    }
    
    return 0;
}