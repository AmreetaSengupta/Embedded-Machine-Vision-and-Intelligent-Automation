/**********************************************************************************************
* File Name: capture.cpp
* Description: The code opens a window with an image continuosly streaming in it and 
* 			   logs the frame rate analysis.
* Author: Amreeta Sengupta
* Date: 06/21/2019
* References: http://www.cse.uaa.alaska.edu/~ssiewert/a485_code/simple-cv/ and docs.opencv.org
***********************************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <syslog.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#define ns 1000000000
#define FRAME 1800
#define deadline 0.5

using namespace cv;
using namespace std;

struct timespec start,stop; 
int count_val = 0;
double pos_jitter=0;
double total_pos_jitter=0;
double timestamp, avg_pos_jitter;
double avg_frame_rate;
double fps=0; 
double avg_time;
double fps_sum=0;
double max_val=0;

int main( int argc, char** argv )
{
    cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
    CvCapture* capture = cvCreateCameraCapture(0);
    IplImage* frame;
    int count = 0;
    double timestamp, sum, max=0, average;
    openlog("Capture",LOG_CONS, LOG_USER );

    while(1)
    {
		clock_gettime( CLOCK_REALTIME, &start);
        frame=cvQueryFrame(capture);
		if(!frame) break;
		cvShowImage("Capture Example", frame);

        
        clock_gettime( CLOCK_REALTIME, &stop);
         timestamp = (double)(stop.tv_sec - start.tv_sec + (double) (stop.tv_nsec - start.tv_nsec)/(double) ns);
        if(timestamp > max_val)
		{
			max_val = timestamp;
		}
		fps = 1/timestamp;
		fps_sum += fps;
		avg_frame_rate = (double) (fps_sum/count_val);
		pos_jitter = (double) (deadline - timestamp);
		total_pos_jitter += pos_jitter; 
		avg_pos_jitter = (double) total_pos_jitter/ (double) count_val;
		syslog(LOG_INFO, "Frame %d: Execution time: %lf sec",count_val, timestamp);
		syslog(LOG_INFO, "jitter = %lf ",pos_jitter);
		count_val++; 
		if (count_val == FRAME)
		break;
			
		char c = cvWaitKey(2);
        if( c == 27) break;
    }
	syslog(LOG_INFO, "Average fps: %lf",avg_frame_rate);
	syslog(LOG_INFO, "Worst Case Frame execution time: %lf sec",max_val);
	syslog(LOG_INFO, "Average Jitter: %lf sec",avg_pos_jitter);
    cvReleaseCapture(&capture);
    cvDestroyWindow("Capture Example");
    
};
