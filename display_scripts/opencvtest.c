#include<opencv2/highgui/highgui.hpp>

int main()
{
    IplImage* img = cvLoadImage("/home/USER/Pictures/python.jpg",CV_LOAD_IMAGE_COLOR);
    cvShowImage("opencvtest",img);
    cvWaitKey(0);
    return 0;
}
