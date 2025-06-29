#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    printf("procexec begin..!\n");

    pid_t id = fork();
    if(id == 0)
    {
        // child
        printf("child process pid: %d\n", getpid());
        sleep(2);
        
        extern char**environ;

        execl("/usr/bin/ls", "ls", "-l", "-a", "--color", NULL);
        
        // exit(1);
    }

    // father
    int statu = 0;
    pid_t rid = waitpid(id, &statu, 0);
    if(rid > 0)
    {
        printf("father wait success, child exit code: %d\n", WEXITSTATUS(statu));
    }
    
    sleep(2);
    printf("procexec end..!\n");

    return 0;
}
