#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/select.h>
#include<unistd.h>
#include<signal.h>
#include<string.h>
#include "MioSoC.h"
#include<sys/ioctl.h>


unsigned int flag = 0;

void sig_handler(int sig)
{
  //printf("%s\n",__FUNCTION__);
  flag++;
  printf("flag = %d\n", flag);
}


int main(void)
{
    int iIntCycleTimeUser=100;
    int fd;
    int f_flags;


    fd=open("/dev/MioSoC0",O_RDWR);
    if(fd<0)
    {
        perror("open");
        return-1;
    } else {
        printf("open ok!\n");
    }


    signal(SIGIO, sig_handler);
    fcntl(fd, F_SETOWN, getpid());
    f_flags = fcntl(fd, F_GETFL);
    fcntl(fd, F_SETFL, FASYNC | f_flags);



    if (ioctl(fd, MioSoCinit,&iIntCycleTimeUser) < 0) {  // chip init.
        perror("MioSoCinit failed");
        goto err_close;
    } else {
        printf("MioSoCinit ok.\n");
    }




    //MOT_InitData();
/*
    //[MotionLibraryV2.1.2_20030724
    if(GetCardAxisNum(giFpgaBaseAddress) == 0x0003)
    {
        glBufferNumber[3] = 1500;
        glBufferNumber[4] = 1500;
        glBufferNumber[5] = 1500;
        iMaxAxisNum = GetCardAxisNum(giFpgaBaseAddress);
    }
    else
    {
        iMaxAxisNum = MAX_AXIS;
    }
    //
*/




    while(1)
      {
	sleep(10);
      }
/*
        for(i=0;i<100;i++)
        {
            printf("loop value is i:%d ",i);
        }
*/
        /*
    {
        printf("waiting \n");
        sleep(2);
        if(flag > 3)
            break;
    }*/



err_close:
    close(fd);

    return 0;


}





