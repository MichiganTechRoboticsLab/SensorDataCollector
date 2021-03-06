/*!
*/

#include "urg_sensor.h"
#include "urg_utils.h"
#include "open_urg_sensor.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <sys/time.h>
#include <sys/types.h>

#include <sys/signalfd.h>
#include <signal.h>
#include <unistd.h>
#include <poll.h>

#define handle_error(msg)                               \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

static int setupSig(void)
{
    sigset_t mask;
    int sfd;
    

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGTERM);

    /* Block signals so that they aren't handled
       according to their default dispositions */

    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        handle_error("sigprocmask");

    sfd = signalfd(-1, &mask, 0);
    if (sfd == -1)
        handle_error("signalfd");
    return sfd;
}


static void print_echo_data(long data[], unsigned short intensity[],
                            int index)
{
    int i;

    // [mm]
    for (i = 0; i < URG_MAX_ECHO; ++i) {
        printf("%ld, ", data[(URG_MAX_ECHO * index) + i]);
    }

    // [1]
    for (i = 0; i < URG_MAX_ECHO; ++i) {
        printf("%d, ", intensity[(URG_MAX_ECHO * index) + i]);
    }
}


// \~japanese 距離、強度のデータを表示する
static void print_data(urg_t *urg, long data[],
                       unsigned short intensity[], int data_n, long time_stamp)
{
#if 0
    int front_index;

    (void)data_n;

    // \~japanese 前方のデータのみを表示
    front_index = urg_step2index(urg, 0);
    print_echo_data(data, intensity, front_index);
    printf("%ld\n", time_stamp);

#else
    (void)urg;
    int i;
    
    // System Timestamp
    struct timeval tv;
    gettimeofday(&tv, NULL);
	  printf("%ld.%06ld, ", (long) tv.tv_sec, (long) tv.tv_usec);
        
    // Sensor Timestamp
    printf("%ld, ", time_stamp);
    
    // \~japanese 全てのデータを表示
    // \~English Display all of the data
    for (i = 0; i < data_n; ++i) {
        print_echo_data(data, intensity, i);
    }
    
    printf("%ld\n", time_stamp);
#endif
}


int main(int argc, char *argv[])
{
    enum {
        CAPTURE_TIMES = 1000000,
    };
    urg_t urg;
    int max_data_size;
    long *data = NULL;
    unsigned short *intensity = NULL;
    long time_stamp;
    int n;
    int i;

    while (open_urg_sensor(&urg, argc, argv) < 0);

    max_data_size = urg_max_data_size(&urg);
    data = (long *)malloc(max_data_size * 3 * sizeof(data[0]));
    intensity = (unsigned short *)malloc(max_data_size * 3 *
                                         sizeof(intensity[0]));

    if (!data) {
        perror("urg_max_index()");
        return 1;
    }
    
    // Signals for a soft shutdown
    int sfd = setupSig();
    struct signalfd_siginfo fdsi;
    struct pollfd pfd[1];
    int ret;

    pfd[0].fd = sfd;
    pfd[0].events = POLLIN | POLLERR | POLLHUP;
    
    
    // Loop Control
    bool run = true;
    
    // \~japanese データ取得
    urg_start_measurement(&urg, URG_MULTIECHO_INTENSITY, CAPTURE_TIMES, 0);
    for (i = 0; i < CAPTURE_TIMES && run; ++i) {
        ret = poll(pfd, 1, 0);
        
        if(ret > 0)
        {
            ssize_t s = read(sfd, &fdsi, sizeof(struct signalfd_siginfo));
            if (s != sizeof(struct signalfd_siginfo))
                handle_error("read");
            
            switch(fdsi.ssi_signo)
            {
                case SIGINT:
                case SIGQUIT:
                case SIGTERM:
                    run = false;
                    break;
                default: break;
            }
        }
        n = urg_get_multiecho_intensity(&urg, data, intensity, &time_stamp);
        if ((n <= 0) & 0) {
            printf("urg_get_multiecho_intensity: %s\n", urg_error(&urg));
            free(data);
            free(intensity);
            urg_close(&urg);
            return 1;
        }
        
        if (n > 0)
          print_data(&urg, data, intensity, n, time_stamp);
    }

    // \~japanese 切断
    free(data);
    free(intensity);
    urg_close(&urg);

#if defined(URG_MSC)
    getchar();
#endif
    return 0;
}
