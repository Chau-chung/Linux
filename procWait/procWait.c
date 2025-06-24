#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

int main()
{
    pid_t id = fork();
    if(id == 0)
    {
        // child
        int cnt = 5;
        while(cnt)
        {
            printf("I am child process, pid: %d, ppid: %d, cnt: %d\n", getpid(), getppid(), cnt);
            sleep(1);
                
            --cnt;
        }

        printf("child process exit..\n");
        exit(111);
    }

    // father
    while(1)
    {
        int status = 0;
        pid_t rid = waitpid(id, &status, WNOHANG); // non block
        if(rid == 0)
        {
            sleep(1);
            printf("child proc is running, father proc check next time!\n");
        }
        else if(rid > 0)
        {
            if(WIFEXITED(status))
            {
                printf("child exit success, child exit code: %d\n", WEXITSTATUS(status));
            }
            else
            {
                printf("child exit unnormal!\n");
            }
            break;
        }
        else
        {
            printf("waitpid failed!\n");
            break;
        }
    }
}

//int main()
//{
//    pid_t id = fork();
//    if(id == 0)
//    {
//        // child
//        int cnt = 5;
//        while(cnt)
//        {
//            printf("I am child process, pid: %d, ppid: %d, cnt: %d\n", getpid(), getppid(), cnt);
//            sleep(1);
//
//            --cnt;
//        }
//
//        printf("child process exit..\n");
//        exit(111);
//    }
//    sleep(5);
//    
//    // father
//    // pid_t rid = wait(NULL);
//    int status = 0;
//    pid_t rid = waitpid(id, &status, 0);
//    if(rid > 0) 
//    {
//        printf("wait success, rid: %d\n", rid);
//    }
//    else 
//    {
//        printf("wait failed!\n");
//    }
//    sleep(2);
//
//    printf("father process exit, status: %d, child exit code: %d, coredump: %d, child exit signal: %d\n", status, (status>>8) & 0xFF, status & 0x80, status & 0x7F);
//
//    return 0;
//}

