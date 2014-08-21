/*!
  \author Satofumi KAMIMURA

  $Id: get_distance.c,v 586c4fa697ef 2011/01/24 08:50:01 Satofumi $
*/

#define BUFFSIZE 80

#include "/home/ubuntu/IRLLibs/data_storage/datalocs.h"

#include "urg_sensor.h"
#include "urg_utils.h"
#include "open_urg_sensor.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

static int keepRunning = 1;

void killData(int dummy)
{
    keepRunning = 0;
}

static void print_data(urg_t *urg, long data[], int data_n, long time_stamp, FILE* fp, int data_i)
{
#if 1
    int front_index;

    (void)data_n;

    front_index = urg_step2index(urg, 0);

    //printf("%ld [mm], (%ld [msec])\n", data[front_index], time_stamp);

    char buffer[BUFFSIZE];
    
    memset(buffer, 0, BUFFSIZE);
    //int n = sprintf(fd, "%ld,%ld\n", data[front_index], time_stamp);
    fprintf(fp, "%ld,%ld\n", data[front_index], time_stamp);
    //printf("current angle: %.2f\n", urg_index2rad(urg, data_i-1));
    //int error = write(fd, buffer, n);

#endif
}


int main(int argc, char *argv[])
{
    //couldn't write when sshing in
    //FILE* fp = fopen("/media/ubuntu/Kingston/lidarData.csv", "w");
    FILE* fp = fopen(LIDARLOC, "w");

    //FILE* fp = fopen("lidarData.csv","w");
    if( !fp ){
        printf("could not create file, exiting...\n");
        exit(1);
    }

    signal(SIGINT, killData);
    enum {
        CAPTURE_TIMES = 10,
    };
    urg_t urg;
    long *data = NULL;
    long time_stamp;
    int n;
    int i;

    if (open_urg_sensor(&urg, argc, argv) < 0) {
        return 1;
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
    //for (i = 0; i < CAPTURE_TIMES; ++i) {
    for( i=0; keepRunning; ++i ) {
        n = urg_get_distance(&urg, data, &time_stamp);
        if (n <= 0) {
            printf("urg_get_distance: %s\n", urg_error(&urg));
            printf("thank you for retreiving data!\n");
            free(data);
            urg_close(&urg);
            return 1;
        }
        print_data(&urg, data, n, time_stamp, fp, i);
    }
    
    

    free(data);
    urg_close(&urg);

#if defined(URG_MSC)
    getchar();
#endif
    return 0;
}
