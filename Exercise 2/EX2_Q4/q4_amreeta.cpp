/**********************************************************************************************
* File Name: q4_amreeta.cpp
* Description: The code opens a window with an image in it which either applies canny transform
* 			   or Sobel tranform based on used input and logs the frame rate analysis.
* Author: Amreeta Sengupta
* Date: 06/21/2019
* References: http://www.cse.uaa.alaska.edu/~ssiewert/a485_code/simple-cv/ and docs.opencv.org
***********************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <syslog.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#define HRES 640
#define VRES 480
#define FRAME 1800
#define ns 1000000000
#define deadline 0.5


IplImage* frame;
Mat src, src_gray;
Mat dst, detected_edges;
Mat grad; 
int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
const char* window_name = "Canny and Sobel Transform";
int scale = 1;
int delta = 0;
int ddepth = CV_16S;
int c;
struct timespec start,stop; 
int count_val = 0;
double pos_jitter=0;
double total_pos_jitter=0;
double timestamp, avg_pos_jitter;
double avg_frame_rate;
double fps=0; 
double fps_sum=0;
double max_val=0;
char c1;

void CannyThreshold(int, void*)
{
  /// Reduce noise with a kernel 3x3
  blur( src_gray, detected_edges, Size(3,3) );

  /// Canny detector
  Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

  /// Using Canny's output as a mask, we display our result
  dst = Scalar::all(0);

  src.copyTo( dst, detected_edges);
  imshow( window_name, dst );
}

int main( int argc , char** argv)
{
	CvCapture* capture;
	capture = (CvCapture *)cvCreateCameraCapture(0);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, HRES);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, VRES);
    openlog("Amreeta",LOG_CONS, LOG_USER );

    while(1)
    {
		clock_gettime( CLOCK_REALTIME, &start);
        frame=cvQueryFrame(capture);
        if(!frame) break;

		src = cvarrToMat(frame);
		
		if(*argv[1] == 'c' || *argv[1] == 'C')
		{
	  		/// Create a matrix of the same type and size as src (for dst)
	  	    dst.create( src.size(), src.type() );
	
	  		/// Convert the image to grayscale
	  		cvtColor( src, src_gray, CV_BGR2GRAY );
	
	  		/// Create a window
	  		namedWindow( window_name, CV_WINDOW_AUTOSIZE );
	
	  		/// Create a Trackbar for user to enter threshold
	  		createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );
	
	  		/// Show the image
	  		CannyThreshold(0, 0);
	  		
		}
		
		else if (*argv[1] == 's' || *argv[1] == 'S')
		{
			  GaussianBlur( src, src, Size(3,3), 0, 0, BORDER_DEFAULT );
			 /// Convert it to gray
			  cvtColor( src, src_gray, CV_RGB2GRAY );
			
			  /// Create window
			  namedWindow( window_name, CV_WINDOW_AUTOSIZE );
			
			  /// Generate grad_x and grad_y
			  Mat grad_x, grad_y;
			  Mat abs_grad_x, abs_grad_y;
			 
			  /// Gradient X
			  //Scharr( src_gray, grad_x, ddepth, 1, 0, scale, delta, BORDER_DEFAULT );
			  Sobel( src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );   
			  convertScaleAbs( grad_x, abs_grad_x );
			
			  /// Gradient Y 
			  //Scharr( src_gray, grad_y, ddepth, 0, 1, scale, delta, BORDER_DEFAULT );
			  Sobel( src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );   
			  convertScaleAbs( grad_y, abs_grad_y );
			
			  /// Total Gradient (approximate)
			  addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );
			
			  imshow( window_name, grad );
			}
			
		else
		{
			printf("Error!\n");
			exit(0);
		}
		
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
		
		c1 = cvWaitKey(1);
        if( c1 == 'q')
        {
            printf("quit\n"); 
            break;
        }
	}
	syslog(LOG_INFO, "Average fps: %lf",avg_frame_rate );
	syslog(LOG_INFO, "Worst Case Frame execution time: %lf sec",max_val);
	syslog(LOG_INFO, "Average Jitter: %lf sec",avg_pos_jitter);
    cvReleaseCapture(&capture);
    return 0;
    
};

