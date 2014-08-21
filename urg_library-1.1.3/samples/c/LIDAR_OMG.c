#define BUFFSIZE 80
#define IMG_WIDTH 620
#define IMG_HEIGHT 480

#include "urg_sensor.h"
#include "urg_utils.h"
#include "open_urg_sensor.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>
#include <opencv2/highgui/highgui.hpp>

int keepgoing = 1;

void sig_handler(int signo){
    keepgoing = 0;
}

int main(int argc, char *argv[])
{

    
    signal(SIGINT , sig_handler);
    signal(SIGTERM , sig_handler);
    
    urg_t urg;
    long *data = NULL;
    long time_stamp;
    int n;
    struct timeval tv;
    IplImage* img =cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT),IPL_DEPTH_8U,1);
    int i;
    
    int display = 0;
    
    for( i = 1; i<argc; i++ )
    {
        if (!strcmp(argv[i], "-d")) {
            display = 1;
        }
    }
    
    printf("number of arguments: %d\n", argc);
    
    urg_connection_type_t connection_type = URG_ETHERNET;
    long baudrate_or_port = 10940;
    const char *ip_address = "192.168.0.10";
    
    if (urg_open(&urg, connection_type, ip_address, baudrate_or_port) < 0) {
        printf("urg_open: %s, %ld: %s\n",
            ip_address, baudrate_or_port, urg_error(&urg));
        return -1;
    }
    
    data = (long *)malloc(urg_max_data_size(&urg) * sizeof(data[0]));
    if (!data) {
        perror("urg_max_index()");
        return 1;
    }

#if 0
    urg_set_scanning_parameter(&urg,
                               urg_deg2step(&urg, -90),
                               urg_deg2step(&urg, +90), 0);
#endif

    urg_start_measurement(&urg, URG_DISTANCE, 0, 0);
    while(keepgoing) {
        n = urg_get_distance(&urg, data, &time_stamp);
        if (n <= 0) {
            printf("urg_get_distance: %s\n", urg_error(&urg));
            free(data);
            urg_close(&urg);
            return 1;
        }

        double radian;
        long x;
        long y;
        
        gettimeofday(&tv, NULL);
        printf("-1,%d,%d\n", tv.tv_sec, tv.tv_usec);
        
        if( display )
            cvSet(img, cvScalar(255,255,255,0),NULL);
         
        for( i=0; i<n; i++ )
        {
            radian = urg_index2rad(&urg, i);
            
            
            x = (long)(data[i]/50 * cos(radian));
            y = (long)(data[i]/50 * sin(radian));
            
            x += IMG_WIDTH/2;
            y = -y + IMG_HEIGHT/2;
            
            if( display )
                cvLine(img,cvPoint(x,y),cvPoint(x,y),cvScalar(0,255,0,0),1,8,0);

            printf("%lu,%.4f\n", data[i],radian);
            
        }
        
        if( display )
        {
            cvShowImage("opencvtest",img);
            cvWaitKey(1);
        }
        //printf("0,0,0,0,0,0.0\n");       
    }

    free(data);
    urg_close(&urg);

/*
#if defined(URG_MSC)
    getchar();
#endif
*/
    return 0;
}
