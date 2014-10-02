//=============================================================================
// Copyright © 2008 Point Grey Research, Inc. All Rights Reserved.
//
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with PGR.
//
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================
//=============================================================================
// $Id: CustomImageEx.cpp,v 1.20 2010-02-26 23:24:47 soowei Exp $
//=============================================================================

//#include "stdafx.h"
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

#include "FlyCapture2.h"

using namespace FlyCapture2;

void PrintBuildInfo()
{
    FC2Version fc2Version;
    Utilities::GetLibraryVersion( &fc2Version );
    char version[128];
    sprintf( 
        version, 
        "FlyCapture2 library version: %d.%d.%d.%d\n", 
        fc2Version.major, fc2Version.minor, fc2Version.type, fc2Version.build );

    printf( "%s", version );

    char timeStamp[512];
    sprintf( timeStamp, "Application build date: %s %s\n\n", __DATE__, __TIME__ );

    printf( "%s", timeStamp );
}

void PrintCameraInfo( CameraInfo* pCamInfo )
{
    printf(
        "\n*** CAMERA INFORMATION ***\n"
        "Serial number - %u\n"
        "Camera model - %s\n"
        "Camera vendor - %s\n"
        "Sensor - %s\n"
        "Resolution - %s\n"
        "Firmware version - %s\n"
        "Firmware build time - %s\n\n",
        pCamInfo->serialNumber,
        pCamInfo->modelName,
        pCamInfo->vendorName,
        pCamInfo->sensorInfo,
        pCamInfo->sensorResolution,
        pCamInfo->firmwareVersion,
        pCamInfo->firmwareBuildTime );
}

void PrintFormat7Capabilities( Format7Info fmt7Info )
{
    printf(
        "Max image pixels: (%u, %u)\n"
        "Image Unit size: (%u, %u)\n"
        "Offset Unit size: (%u, %u)\n"
        "Pixel format bitfield: 0x%08x\n",
        fmt7Info.maxWidth,
        fmt7Info.maxHeight,
        fmt7Info.imageHStepSize,
        fmt7Info.imageVStepSize,
        fmt7Info.offsetHStepSize,
        fmt7Info.offsetVStepSize,
        fmt7Info.pixelFormatBitField );
}

void PrintError( Error error )
{
    error.PrintErrorTrace();
}

int main(int argc, char** argv)
{
    struct timeval result;

    PrintBuildInfo();

    //fastest BW is MODE_4 w/ PIXEL_FORMAT_MONO8 @ 32.00 fps, calculated to 30.30 fps
    //fastest C  is MODE_1 w/ PIXEL_FORMAT_RGB8  @ 22.00 fps, calculated to 21.74 fps
    //Full-Rez Color is MODE_0 w/ PIXEL_FORMAT_RGB8  @ 8.00 fps
    const Mode k_fmt7Mode = MODE_0;
    const PixelFormat k_fmt7PixFmt = PIXEL_FORMAT_RAW8;
    const int k_numImages = 1000000;

    Error error;

    // Since this application saves images in the current folder
    // we must ensure that we have permission to write to this folder.
    // If we do not have permission, fail right away.
    
    //got rid of writing to the sd card because it was taking forever to the sd card
	//FILE* tempFile = fopen("/media/ubuntu/Kingston/blackfly_pics/test.txt", "w+");
	
	char __buff[100];
	//sprintf(__buff,"%stest.txt", PICLOC);
	sprintf(__buff,"%stest.txt", argv[1]);
	
	printf("testing file at %s\n", __buff);
	
	FILE* tempFile = fopen(__buff, "w+");
	
	if (tempFile == NULL)
	{
		printf("Failed to create file in current folder.  Please check permissions.\n");
		return -1;
	}
	fclose(tempFile);
	remove("test.txt");

    BusManager busMgr;
    unsigned int numCameras;
    error = busMgr.GetNumOfCameras(&numCameras);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    printf( "Number of cameras detected: %u\n", numCameras );

    if ( numCameras < 1 )
    {
        printf( "Insufficient number of cameras... exiting\n" );
        return -1;
    }

    PGRGuid guid;
    error = busMgr.GetCameraFromIndex(0, &guid);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    Camera cam;

    // Connect to a camera
    error = cam.Connect(&guid);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    // Get the camera information
    CameraInfo camInfo;
    error = cam.GetCameraInfo(&camInfo);
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    PrintCameraInfo(&camInfo);        

    // Query for available Format 7 modes
    Format7Info fmt7Info;
    bool supported;
    fmt7Info.mode = k_fmt7Mode;
    error = cam.GetFormat7Info( &fmt7Info, &supported );
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    PrintFormat7Capabilities( fmt7Info );

    if ( (k_fmt7PixFmt & fmt7Info.pixelFormatBitField) == 0 )
    {
        // Pixel format not supported!
		printf("Pixel format is not supported\n");
        return -1;
    }
    
    Format7ImageSettings fmt7ImageSettings;
    fmt7ImageSettings.mode = k_fmt7Mode;
    fmt7ImageSettings.offsetX = 0;
    fmt7ImageSettings.offsetY = 0;
    fmt7ImageSettings.width = fmt7Info.maxWidth;
    fmt7ImageSettings.height = fmt7Info.maxHeight;
    fmt7ImageSettings.pixelFormat = k_fmt7PixFmt;

    bool valid;
    Format7PacketInfo fmt7PacketInfo;

    // Validate the settings to make sure that they are valid
    error = cam.ValidateFormat7Settings(
        &fmt7ImageSettings,
        &valid,
        &fmt7PacketInfo );
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    if ( !valid )
    {
        // Settings are not valid
		printf("Format7 settings are not valid\n");
        return -1;
    }

    // Set the settings to the camera
    error = cam.SetFormat7Configuration(
        &fmt7ImageSettings,
        fmt7PacketInfo.recommendedBytesPerPacket );
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    // Start capturing images
    error = cam.StartCapture();
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    // Retrieve frame rate property
    Property frmRate;
    frmRate.type = FRAME_RATE;
    error = cam.GetProperty( &frmRate );
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    printf( "Frame rate is %3.2f fps\n", frmRate.absValue );

    printf( "Grabbing %d images\n", k_numImages );

    //this is how many error frames were received
    int badboy = 0;

    time_t timer1, timer2;
    
    time(&timer1);
    Image rawImage;    
    for ( int imageCount=0; imageCount < k_numImages; imageCount++ )
    {
        // Retrieve an image
        error = cam.RetrieveBuffer( &rawImage );
        if (error != PGRERROR_OK)
        {
            //PrintError( error );
            //printf("I messed up on picture number: %d\n", imageCount);
            badboy++;
            continue;
        }

        //printf( "." );

        // Get the raw image dimensions
        PixelFormat pixFormat;
        unsigned int rows, cols, stride;
        rawImage.GetDimensions( &rows, &cols, &stride, &pixFormat );

        // Create a converted image
        Image convertedImage;

        // Convert the raw image
	    // I commented this out because it was taking way to long to get data
	    ///*
        error = rawImage.Convert( PIXEL_FORMAT_BGRU, &convertedImage );
        if (error != PGRERROR_OK)
        {
            PrintError( error );
            return -1;
        }
        //*/

        // Create a unique filename
        char filename[512];
        //sprintf( filename, "/media/ubuntu/Kingston/blackfly_pics/%u-%d.bmp", camInfo.serialNumber, imageCount );
        //sprintf( filename, "%s%u-%d.bmp", PICLOC, camInfo.serialNumber, imageCount );
        //sprintf( filename, "%s%u-%d.bmp", argv[1], camInfo.serialNumber, imageCount );
        
        gettimeofday(&result, NULL);
        
        sprintf( filename, "%s%06d,%ld,%ld.bmp", argv[1], imageCount, result.tv_sec, result.tv_usec );

        // Save the image. If a file format is not passed in, then the file
        // extension is parsed to attempt to determine the file format.
        error = convertedImage.Save( filename );
        //error = rawImage.Save( filename );

	    //printf("GOT HERE!");

        if (error != PGRERROR_OK)
        {
            PrintError( error );
            return -1;
        }

    }
    time(&timer2);
    double seconds = difftime(timer2, timer1);

    printf( "\nFinished grabbing images\nCongratulations! You only messed up %d out of %d times\nIt took %.0f seconds\nThe calculated frame rate is %.2f fps\n", badboy, k_numImages, seconds, ((double) k_numImages)/seconds);

    // Stop capturing images
    error = cam.StopCapture();
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }      

    // Disconnect the camera
    error = cam.Disconnect();
    if (error != PGRERROR_OK)
    {
        PrintError( error );
        return -1;
    }

    printf( "Done! Press Enter to exit...\n" );
    getchar();

	return 0;
}
