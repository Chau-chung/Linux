#include <iostream>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[], char* env[])
{
    for(int i = 0; argv[i]; ++i)
    {
        printf("argv[%d] : %s\n", i, argv[i]);
    }

    printf("-------------------------------\n");

    for(int i = 0; env[i]; i++)
    {
        printf("env[%d] : %s\n", i, env[i]);
    }

    printf("-------------------------------\n");

    cout << "hello C++, I am a C++ pragma!: " << getpid() << endl;
    cout << "hello C++, I am a C++ pragma!: " << getpid() << endl;
    cout << "hello C++, I am a C++ pragma!: " << getpid() << endl;
    cout << "hello C++, I am a C++ pragma!: " << getpid() << endl;

    return 0;
}

