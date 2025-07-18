#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SIZE 512
#define ZERO '\0'
#define SEP " "
#define NUM 32
#define SkipPath(p) do { p += (strlen(p) - 1); while(*p != '/') --p; }while(0)
#define SkipSpace(cmd, pos) do{\
    while(1){\
        if(isspace(cmd[pos])) ++pos;\
        else break;\
    }\
}while(0)

#define None_Redir 0
#define In_Redir   1
#define Out_Redir  2
#define App_Redir  3

int redir_type = None_Redir;
char* filename = NULL;

char cwd[SIZE*2];
char* gArgv[NUM];
int lastcode = 0;

void Die()
{
    exit(-1);
}

const char* GetHome()
{
    const char* home = getenv("HOME");
    if(home == NULL) return "/";

    return home;
}

const char* GetUserName()
{
    const char* name = getenv("USER");
    if(name == NULL) return "None";

    return name;
}

const char* GetHostName()
{
    const char* hostname = getenv("HOSTNAME");
    if(hostname == NULL) return "None";

    return hostname;
}

const char* GetCwd()
{
    const char* cwd = getenv("PWD");
    if(cwd == NULL) return "None";

    return cwd;
}

void MakeCommandLineAndPrint()
{
    char line[SIZE];

    const char* username = GetUserName();
    const char* hostname = GetHostName();
    const char* cwd = GetCwd();

    SkipPath(cwd);

    snprintf(line, sizeof(line), "[%s@%s %s]> ", username, hostname, strlen(cwd) == 1 ? "/" : cwd + 1);
    printf("%s", line);

    fflush(stdout);
}

int GetUserCommand(char command[], size_t n)
{
    char* cmd = fgets(command, n, stdin);

    if(cmd == NULL) return -1;
    command[strlen(command) - 1] = ZERO;

    return strlen(command);
}

void SplitCommand(char command[], size_t n)
{
    gArgv[0] = strtok(command, SEP);

    int index = 1;
    while((gArgv[index++] = strtok(NULL, SEP))); // 先赋值，再判断，strtok最后返回NULL，gArgv最后一个元素为NULL，并且判断结束
}

void ExecuteCommand()
{
    pid_t id = fork();

    if(id < 0) Die();
    else if(id == 0)
    {
        if(filename != NULL)
        {
            if(redir_type == In_Redir)
            {
                int fd = open(filename, O_RDONLY);
                dup2(fd, 0);
            }
            else if(redir_type == Out_Redir)
            {
                int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
                dup2(fd, 1);
            }
            else if(redir_type == App_Redir)
            {
                int fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0666);
                dup2(fd, 1);
            }
            else
            {
                // do nothing
            }
        }

        // child
        execvp(gArgv[0], gArgv);
        exit(errno);
    }
    else
    {
        // fahter
        int status = 0;
        pid_t rid = waitpid(id, &status, 0);
        if(rid > 0)
        {
            lastcode = WEXITSTATUS(status);
            if(lastcode != 0) printf("%s:%s:%d\n", gArgv[0], strerror(lastcode), lastcode);
        }
    }
}

void Cd()
{
    const char* path = gArgv[1];
    if(path == NULL) path = GetHome(); // path 不为空

    chdir(path);

    // 刷新环境变量
    char temp[SIZE*2];
    getcwd(temp, sizeof(temp));
    snprintf(cwd, sizeof(cwd), "PWD=%s", temp);
    putenv(cwd);
}

int CheckBuildin()
{
    int flag = 0;

    const char* enter_cmd = gArgv[0];
    if(strcmp(enter_cmd, "cd") == 0)
    {
        flag = 1;
        Cd();
    }
    else if(strcmp(enter_cmd, "echo") == 0 && strcmp(gArgv[1], "$?") == 0)
    {
        flag = 1;
        printf("%d\n", lastcode);

        lastcode = 0;
    }

    return flag;
}

void CheckRedir(char cmd[])
{
    int pos = 0;
    int end = strlen(cmd);

    while(pos < end)
    {
        if(cmd[pos] == '>')
        {
            if(cmd[pos + 1] == '>')
            {
                cmd[pos++] = 0;
                ++pos;
                redir_type = App_Redir;
                SkipSpace(cmd, pos);
                filename = cmd + pos;
            }
            else
            {
                cmd[pos++] = 0;
                redir_type = Out_Redir;
                SkipSpace(cmd, pos);
                filename = cmd + pos;
            }
        }
        else if(cmd[pos] == '<')
        {
            cmd[pos++] = 0;
            redir_type = In_Redir;
            SkipSpace(cmd, pos);
            filename = cmd + pos;
        }
        else
        {
            ++pos;
        }
    }
}

int main()
{
    int quit = 0;
    while(!quit)
    {
        // 0.重置
        redir_type = None_Redir;
        filename = NULL;

        // 1.输出命令行
        MakeCommandLineAndPrint();

        // 2.获取用户命令字符串
        char usercommand[SIZE];

        int n = GetUserCommand(usercommand, sizeof(usercommand));
        if(n <= 0) return -1;

        // 2.1 check redir
        CheckRedir(usercommand);
        
        // 2.2 debug
        // printf("cmd: %s\n", usercommand);
        // printf("redir: %d\n", redir_type);
        // printf("filename: %s\n", filename);

        // 3.命令行字符串分割
        SplitCommand(usercommand, sizeof(usercommand));

        // 4.检测命令是否为内建命令
        n = CheckBuildin();
        if(n) continue;

        // 5.执行命令
        ExecuteCommand();
    }

    return 0;
}
