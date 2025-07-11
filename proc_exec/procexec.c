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
        putenv("ENV0=00000");

        char* const argv[] =
        {
            // (char*)"ls",
            // (char*)"-l",
            // (char*)"-a",
            // (char*)"--color",

            (char*)"testexec",
            (char*)"-a",
            (char*)"-c",

            NULL
        };

        char* const envp[] =
        {
            (char*)"ENV1=11111",
            (char*)"ENV2=22222",

            NULL
        };

        extern char** environ;

        printf("child pid: %d\n", getpid());
        sleep(2);

        // execl("/usr/bin/top", "top", NULL);
        // execlp("top", "top", NULL);
        // execle("/usr/bin/ls", "ls", NULL, envp);
        // execv("/usr/bin/ls", argv);
        // execvp("ls", argv);
        execvpe("./testexec", argv, environ);

        // execve("/usr/bin/ls", argv, envp); // system call

        exit(-1);
    }

    // father
    int statu = 0;
    pid_t rid = waitpid(id, &statu, 0);
    if(rid > 0)
    {
        printf("father wait success, child exit code: %d\n", WEXITSTATUS(statu));
    }

    printf("procexec end..!\n");

    return 0;
}
