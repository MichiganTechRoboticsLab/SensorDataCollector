#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>
#include <stdint.h>

#include "vectornav.h"
#include "IRLServer.h"
#include "vn_errorCodes.h"

#define NPOINTS 3
/* Change the connection settings to your configuration. */
const char* const COM_PORT = "//dev//ttyS0";
//original 115200
const int BAUD_RATE = 115200;

void vnerr_msg(VN_ERROR_CODE vn_error, char* msg)
{
  switch(vn_error)
  {
    case VNERR_NO_ERROR:
      strcpy(msg, "No Error");
      break;
    case VNERR_UNKNOWN_ERROR:
      strcpy(msg, "Unknown Error");
      break;
    case VNERR_NOT_IMPLEMENTED:
      strcpy(msg, "Not implemented");
      break;
    case VNERR_TIMEOUT:
      strcpy(msg, "Timemout");
      break;
    case VNERR_INVALID_VALUE:
      strcpy(msg, "Invalid value");
      break;
    case VNERR_FILE_NOT_FOUND:
      strcpy(msg, "File not found");
      break;
    case VNERR_NOT_CONNECTED:
      strcpy(msg, "Not connected");
      break;
    default:
      strcpy(msg, "Undefined Error");
  }
}

int main()
{

/*
    double gpsTime;
    unsigned short gpsWeek, status;
    VnVector3 ypr, latitudeLognitudeAltitude, nedVelocity;
    float attitudeUncertainty, positionUncertainty, velocityUncertainty;

    unsigned char gpsFix, numberOfSatellites;
    VnVector3 positionAccuracy;
    float speedAccuracy, timeAccuracy;

    VnVector3 magnetic, acceleration, angularRate;
    float temperature, pressure;
*/
    Vn200 vn200;
    int i;
    VN_ERROR_CODE vn_retval;
    char vn_error_msg[100];

    //printf("Trying to connect\n");
    vn_retval = vn200_connect(&vn200, COM_PORT, BAUD_RATE);
    if (vn_retval != VNERR_NO_ERROR)
    {
        vnerr_msg(vn_retval, vn_error_msg);
        printf(vn_error_msg);
        exit(1);
    }


    //FILE* fp = fopen("vnstuff.csv", "w+");

    struct timeval tv;

	IRLServer serv(1141);
	uint32_t senddata[NPOINTS];
	vn200_reset(&vn200);
	sleep(1);
	
    //printf("connected!\n");
    for (i = 0; 1; i++) {
        Vn200CompositeData data;
        vn200_getCurrentAsyncData(&vn200, &data);

/*
        printf("INS Solution:\n"
            "  YPR.Yaw:                %+#7.2f\n"
            "  YPR.Pitch:              %+#7.2f\n"
            "  YPR.Roll:               %+#7.2f\n"
            "  LLA.Lattitude:          %+#7.2f\n"
            "  LLA.Longitude:          %+#7.2f\n"
            "  LLA.Altitude:           %+#7.2f\n"
            "  Velocity.North:         %+#7.2f\n"
            "  Velocity.East:          %+#7.2f\n"
            "  Velocity.Down:          %+#7.2f\n",
            data.ypr.yaw,
            data.ypr.pitch,
            data.ypr.roll,
            data.latitudeLongitudeAltitude.c0,
            data.latitudeLongitudeAltitude.c1,
            data.latitudeLongitudeAltitude.c2,
            data.velocity.c0,
            data.velocity.c1,
            data.velocity.c2);
        printf("\n\n");
*/

		gettimeofday(&tv, NULL);
		//printf("%d,%d,%.2f,%.2f,%.2f,%.6f,%.6f,%.6f\n",tv.tv_sec, tv.tv_usec, data.ypr.yaw, data.ypr.pitch, data.ypr.roll,
			//data.latitudeLongitudeAltitude.c0, data.latitudeLongitudeAltitude.c1, data.latitudeLongitudeAltitude.c2);
        usleep(1000000/100);

		struct packet {
			double roll;
			double pitch;
			double yaw;
			long a;
			long b;
		} pack;
		
		pack.a = 3293;
		pack.b = 0xCAFEBABE;
		pack.roll = data.ypr.roll;
		pack.pitch = data.ypr.pitch;
		pack.yaw = data.ypr.yaw;
		
		char tosend[32];
		memcpy(tosend,&pack,sizeof(packet));
		
		//std::cout<<serv.send(senddata, sizeof(long)*NPOINTS, 3540, "192.168.0.102")<<std::endl;
		serv.send(&tosend, 32, 3540, "192.168.0.102");
    }


    //fclose(fp);
    vn200_disconnect(&vn200);

    return 0;
}
