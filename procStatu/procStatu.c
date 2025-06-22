#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    pid_t id = fork();

    if(id == 0)
    {
        // child
        int cnt = 5;
        while(cnt)
        {
            printf("I am child process, cnt: %d, pid: %d\n", cnt, getpid());
            sleep(1);
        }
    }
    else
    {
        // parent
        int cnt = 5;
        while(cnt--)
        {
            printf("I am parent process, running, pid: %d\n", getpid());
            sleep(1);
        }
    }

    return 0;
}
