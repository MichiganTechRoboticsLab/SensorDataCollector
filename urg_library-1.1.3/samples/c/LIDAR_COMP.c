/*!
  \author Joshua Manela
  \author Tim Bradt
*/

#define BUFFSIZE 80

//w,620 h,480
#define IMG_WIDTH 1000
#define IMG_HEIGHT 1000
#define IMG_SCALE 1

#define R2_CUTOFF 0.8

#define N_REG_PTS 200

#include "urg_sensor.h"
#include "urg_utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <signal.h>

#include <opencv2/highgui/highgui.hpp>

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf_bessel.h>
#include <gsl/gsl_statistics.h>
#include <gsl/gsl_fit.h>

int keepgoing = 1;

void sig_handler(int signo){
    keepgoing = 0;
}

int main(int argc, char *argv[])
{

    int run = 0;
    double x = 5.0;
    double y = gsl_sf_bessel_J0(x);
    printf("J0(%g) = %.18e\n", x, y);

    signal(SIGINT , sig_handler);
    signal(SIGTERM , sig_handler);
    
    urg_t urg;
    long *data = NULL;
    long time_stamp;
    int n;
    struct timeval tv;
    IplImage* img =cvCreateImage(cvSize(IMG_WIDTH,IMG_HEIGHT),IPL_DEPTH_8U,1);
    int i;
    
    int display = 1;
    
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
        double x;
        double y;
        
        gettimeofday(&tv, NULL);
        //printf("-1,%d,%d\n", tv.tv_sec, tv.tv_usec);
        
        if( display )
            cvSet(img, cvScalar(255,255,255,0),NULL);
        
        
        double samp_arr[N_REG_PTS*2];
        
        for( i=0; i<n; i++ )
        {
            radian = urg_index2rad(&urg, i);
            
            x = (double)(data[i] * cos(radian));
            y = (double)(data[i] * sin(radian));
            
            //printf("%d %d\n", (i%N_REG_PTS)*2, (i%N_REG_PTS)*2+1);
            
            samp_arr[(i%N_REG_PTS)*2] = x;
            samp_arr[(i%N_REG_PTS)*2+1] = y;
            
            double c0, c1, cov00, cov01, cov11, sumsq;
            if( i > N_REG_PTS && i<(n-N_REG_PTS) ){
                //y = c0 + c1x
                gsl_fit_linear(samp_arr, 2, samp_arr+1, 2, N_REG_PTS, &c0, &c1,
                    &cov00,&cov01, &cov11, &sumsq);
                    
                
                double r = gsl_stats_correlation(samp_arr,2,samp_arr+1,2,N_REG_PTS);
                double r_2 = r*r;
                  
                //printf("r = %f r^2 = %f\ny = %.2f + %.2fx\n", r, r_2, c0, c1);
                
                double x_sd  = gsl_stats_sd(samp_arr  , 2, N_REG_PTS);
                double y_sd  = gsl_stats_sd(samp_arr+1, 2, N_REG_PTS);
                
                ///*
                if( display && r_2 > R2_CUTOFF && x_sd < 1000 && y_sd < 1000){
                
                    double x_mean = gsl_stats_mean(samp_arr  , 2, N_REG_PTS);
                    double y_mean = gsl_stats_mean(samp_arr+1, 2, N_REG_PTS);
                    

                    
                    double x_max = x_mean + 1.5*x_sd;
                    double y_max = y_mean + 1.5*y_sd;
                    
                    double x_min = x_mean - 1.5*x_sd;
                    double y_min = y_mean - 1.5*y_sd;
                    
                    ///*
                    x_min = ((int)x_min) / IMG_SCALE + IMG_WIDTH/2;
                    x_max = ((int)x_max) / IMG_SCALE + IMG_WIDTH/2;
                    
                    y_min = -((int)y_min) / IMG_SCALE + IMG_HEIGHT/2;
                    y_max = -((int)y_max) / IMG_SCALE + IMG_HEIGHT/2;
                    //*/



                    struct CvPoint lstrt = cvPoint( x_min, y_min );
                    struct CvPoint lend   = cvPoint( x_max, y_max );
                    
                    //cvLine(img, lstrt, lend, cvScalar(0,58,23,0),3,8,0);
                    
                    //*
                    //Plots median point
                    struct CvPoint start = cvPoint( ((int) x_mean)/IMG_SCALE+IMG_WIDTH/2,
                        -((int)y_mean)/IMG_SCALE+IMG_HEIGHT/2 );
                        
                    struct CvPoint end = cvPoint( ((int) x_mean)/IMG_SCALE+IMG_WIDTH/2,
                        -((int)y_mean)/IMG_SCALE+IMG_HEIGHT/2 );
                   
                    
                    cvLine(img, end, start, cvScalar(0,255,255,0),3,8,0);
                    //*/

                 }
                
            }
            ///*
           
            x/=IMG_SCALE;
            y/=IMG_SCALE;
            
            x += IMG_WIDTH/2;
            y = -y + IMG_HEIGHT/2;
            

            
            if( display )
                cvLine(img,cvPoint(x,y),cvPoint(x,y),cvScalar(0,255,0,0),1,8,0);
            //*/
            //printf("%lu,%.4f\n", data[i],radian);
            
        }
        
        ///*
        run++;
        if( display && run%10==0 )
        {
            cvShowImage("opencvtest",img);
            cvWaitKey(1);
        }
        //*/  
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
