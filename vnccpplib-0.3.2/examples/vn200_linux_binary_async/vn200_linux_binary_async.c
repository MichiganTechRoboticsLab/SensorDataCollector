#include <stdio.h>
#include <unistd.h>
#include "vectornav.h"

#include <inttypes.h>
#include <sys/time.h>

#include <sys/signalfd.h>
#include <signal.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>

#define handle_error(msg)                               \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

/* Change the connection settings to your configuration. */
const char* const COM_PORT = "//dev//ttyS0";
const int BAUD_RATE = 115200;

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

void asyncDataListener(
	void* sender,
	VnDeviceCompositeData* data);

int main()
{
	VN_ERROR_CODE errorCode;
	Vn200 vn200;

	errorCode = vn200_connect(
        &vn200,
        COM_PORT,
        BAUD_RATE);

	/* Make sure the user has permission to use the COM port. */
	if (errorCode == VNERR_PERMISSION_DENIED) {

		printf("Current user does not have permission to open the COM port.\n");
		printf("Try running again using 'sudo'.\n");

		return 0;
	}
	else if (errorCode != VNERR_NO_ERROR)
	{
		printf("Error encountered when trying to connect to the sensor.\n");

		return 0;
	}


	/* Disable ASCII asynchronous messages since we want to demonstrate the
	   the binary asynchronous messages. */
	errorCode = vn200_setAsynchronousDataOutputType(
        &vn200,
        VNASYNC_OFF,
        true);

	/* Now configure the binary messages output. Notice how the configuration
	   flags can be joined using the binary OR. */
	errorCode = vn200_setBinaryOutput1Configuration(
		&vn200,
		BINARY_ASYNC_MODE_SERIAL_1,		/* Data will be output on serial port 1. 
		                                 This should be the one we are connected to now. */
		8,							/* Outputting binary data at (800 Hz on-board filter / 4 = 200 Hz). */
		BG1_TIME_STARTUP | BG1_TIME_GPS | BG1_QTN | BG1_POSITION | BG1_INS_STATUS,
		BG2_NONE,
		BG3_NONE,
		BG4_NONE,
		BG5_NONE,
		BG6_NONE,
		true);

	printf("Latitude, Longitude, Altitude, Yaw, Pitch, Roll\n");

	/* Now register to receive notifications when a new binary asynchronous
	   packet is received. */
	errorCode = vn200_registerAsyncDataReceivedListener(&vn200, &asyncDataListener);

	/* Sleep for 24 hours. Data will be received by the asycDataListener during this time. */
	// sleep(60*60*24);
	
	// Signals for a soft shutdown
        int sfd = setupSig();
        struct signalfd_siginfo fdsi;
        struct pollfd pfd[1];
        int ret;

        pfd[0].fd = sfd;
        pfd[0].events = POLLIN | POLLERR | POLLHUP;
	
	
	int run = 1;
	while(run)
        {
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
                        run = 0;
                        break;
                    default: break;
                }
            }
            sleep(10);
        }

	errorCode = vn200_unregisterAsyncDataReceivedListener(&vn200, &asyncDataListener);
	
	errorCode = vn200_disconnect(&vn200);
	
	if (errorCode != VNERR_NO_ERROR)
	{
		printf("Error encountered when trying to disconnect from the sensor.\n");
		
		return 0;
	}

	return 0;
}


void asyncDataListener(void* sender, VnDeviceCompositeData* data)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);


	  printf("%ld.%06ld, %" PRId64 ",  %" PRId64 ",  %5d,  %9f, %9f, %9f, %9f,  %.9f, %.9f, %.9f\n",
	  
	  (long) tv.tv_sec, 
	  (long) tv.tv_usec,
	  
		data->timeStartup,	
		data->timeGps,
		data->insStatus,	
		
		data->quaternion.x,
		data->quaternion.y,
		data->quaternion.z,
		data->quaternion.w,
		
		data->latitudeLongitudeAltitude.c0,
		data->latitudeLongitudeAltitude.c1,
		data->latitudeLongitudeAltitude.c2
		);
}
