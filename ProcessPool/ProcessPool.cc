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

private:
    int _wfd;
    pid_t _subprocessid;
    std::string _name;
}

int main()
{
    return 0;
}