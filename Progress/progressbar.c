#include "progressbar.h"
#include <string.h>

#define Length 101
#define Style '>'

const char* lable = "|/-\\";

// version 1
//void ProgBar()
//{
//    char bar[Length];
//    memset(bar, '\0', sizeof(bar));
//
//    int len = strlen(lable);
//
//    int cnt = 0;
//    while(cnt <= 100)
//    {
//        printf("[%-100s][%3d%%][%c]\r", bar, cnt, lable[cnt%len]);
//        fflush(stdout);
//
//        bar[cnt++] = Style;
//        usleep(20000);
//    }
//
//    printf("\n");
//}

// version 2
void ProgBar(double total, double cur)
{
    char bar[Length];
    memset(bar, '\0', sizeof(bar));

    int len = strlen(lable);

    int cnt = 0;
    double rate = (cur * 100.0) / total;
    int loop_cnt = (int)rate;
    
    while(cnt <= loop_cnt)
    {
        bar[cnt++] = Style;
        //usleep(20000);
    }

    printf("[%-100s][%.1lf%%][%c]\r", bar, rate, lable[cnt%len]);
    fflush(stdout);
}