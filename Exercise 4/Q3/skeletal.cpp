/**********************************************************************************************
* File Name: skeletal.cpp
* Description: The code performs skeletal transform on the continously streaming image
* Author: Amreeta Sengupta
* Date: 07/19/2019
* References: http://www.cse.uaa.alaska.edu/~ssiewert/a485_code and docs.opencv.org
***********************************************************************************************/

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <time.h>

#define ns 1000000000

using namespace cv;
using namespace std;

struct timespec start,stop; 

int main(int argc, char** argv)
{

 Mat src, gray, binary, mfblur;
 cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
 CvCapture* capture = cvCreateCameraCapture(0);
 IplImage* frame;
 char buff[100];
 int frame_num = 0;
 double timestamp;
 double max_val=0;
 while(1)
 {
   clock_gettime( CLOCK_REALTIME, &start);
   frame=cvQueryFrame(capture);
   if(!frame) break;
   
   src = cvarrToMat(frame);
   
   cvtColor(src, gray, CV_BGR2GRAY); //greyscaling the image
 
   threshold(gray, binary, 115, 255, CV_THRESH_BINARY); //convert the image to a binary bitmap
   binary = 255 - binary;

   // To remove median filter, just replace blurr value with 1
   medianBlur(binary, mfblur, 1);
   
   Mat skel(mfblur.size(), CV_8UC1, Scalar(0));
   Mat temp;
   Mat eroded;
   Mat element = getStructuringElement(MORPH_CROSS, Size(3, 3));
   bool done;
   int iterations=0;

 do
 {
   erode(mfblur, eroded, element);
   dilate(eroded, temp, element);
   subtract(mfblur, temp, temp);
   bitwise_or(skel, temp, skel);
   eroded.copyTo(mfblur);

   done = (countNonZero(mfblur) == 0);
   iterations++;
    
 } while (!done && (iterations < 100));
 
 clock_gettime( CLOCK_REALTIME, &stop);
 timestamp = (double)(stop.tv_sec - start.tv_sec + (double) (stop.tv_nsec - start.tv_nsec)/(double) ns);
 if(timestamp > max_val)
 {
	max_val = timestamp; //Calculating the maximum execution time of frames for analysis
	printf("Max Time = %lf seconds\n",max_val);
 }

 // Save the resulting frames
 sprintf(buff, "Frame_%d.jpeg", frame_num++);
 imwrite(buff,skel);
 if(frame_num == 301)
 break;
}
 //waitKey();
 cvReleaseCapture(&capture);
 cvDestroyWindow("Capture Example");
 
 return 0;
}
