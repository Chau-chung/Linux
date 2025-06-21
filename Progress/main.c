#include "progressbar.h"

double bandwidth = 1024 * 1024 * 1.0;

void download(double filesize, callback_t cb)
{
   double cur = 0.0;

   printf("download begin\n");

   while(cur <= filesize)
   {
       cb(filesize, cur);

       usleep(20000);
       cur += bandwidth;
   }

   printf("\ndownload done\n");
}

int main()
{
    download(100 * 1024 * 1024 * 1.0, ProgBar);
    download(5 * 1024 * 1024 * 1.0, ProgBar);
    download(15 * 1024 * 1024 * 1.0, ProgBar);
    download(100 * 1024 * 1024 * 1.0, ProgBar);
    download(200 * 1024 * 1024 * 1.0, ProgBar);
    download(300 * 1024 * 1024 * 1.0, ProgBar); 
    
    return 0;
}