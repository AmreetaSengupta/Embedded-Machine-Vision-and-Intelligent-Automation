/**********************************************************************************************
* File Name: q4.cpp
* Description: The code performs skeletal transform on the continously streaming image
* Author: Amreeta Sengupta
* Date: 07/19/2019
* References: http://www.cse.uaa.alaska.edu/~ssiewert/a485_code, 
*             E.R. Davies Chapter 9,
*             docs.opencv.org
***********************************************************************************************/
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

#define ns 1000000000

using namespace cv;
using namespace std;

struct timespec start,stop;

int main( int argc, char** argv )
{
	int i,j;
	char buff[100];
	int frame_num = 0, iterations = 0;
	bool done;
	double timestamp;
    double max_val=0;
	uchar a0, a1, a2, a3, a4, a5, a6, a7, a8;
	int sigma, chi;
	Mat past_frame,pres_frame,src,gray,mfblur,thresh;
	IplImage* frame, prev_frame;
	
    cvNamedWindow("Capture Example", CV_WINDOW_AUTOSIZE);
    CvCapture* capture = cvCreateCameraCapture(0);
    
	
	frame=cvQueryFrame(capture);
	past_frame = cvarrToMat(frame);
	cvtColor(past_frame,past_frame, CV_BGR2GRAY);
	
    while(1)
    {
		clock_gettime( CLOCK_REALTIME, &start);
        frame=cvQueryFrame(capture);
        if(!frame) break;
        
        pres_frame = cvarrToMat(frame);
        cvtColor(pres_frame,gray, CV_BGR2GRAY); //greyscaling the image
        
        //absdiff(past_frame,pres_frame,src); //Background elimination
        
		threshold(gray, thresh, 115, 255, CV_THRESH_BINARY_INV); //convert the image to a binary bitmap
		medianBlur(thresh,mfblur, 3);
		
		do
		{
			done = true;
			for(i = 1; i < mfblur.rows-1; i++)
			{
				for(j = 1; j < mfblur.cols-1; j++)
				{
					if (mfblur.at<uchar>(i,j) != 0)
					{
						//accessing the individual pixel values
						a0=mfblur.at<uchar>(i,j);
						a1=mfblur.at<uchar>(i,j+1);
						a2=mfblur.at<uchar>(i-1,j+1);
						a3=mfblur.at<uchar>(i-1,j);
						a4=mfblur.at<uchar>(i-1,j-1);
						a5=mfblur.at<uchar>(i,j-1);
						a6=mfblur.at<uchar>(i+1,j-1);
					    a7=mfblur.at<uchar>(i+1,j);
					    a8=mfblur.at<uchar>(i+1,j+1);
				
						
						sigma = a0+a1+a2+a3+a4+a5+a6+a7+a8; //sum of all pixels
						
						chi = (int)(a1 != a3) + (int)(a3 != a5) + (int)(a5 != a7) + (int)(a7 != a1)
						       + 2 *((int)((a2>a1)&&(a2>a3)) + (int)((a4>a3)&&(a4>a5)) +
						       (int)((a6>a5)&&(a6>a7)) + (int)((a8>a7)&&(a8>a1)));
						 
						 if((chi == 2) && (sigma != 255))
						 {
							 mfblur.at<uchar>(i,j)=0;
							 done = false;
						 }
					}
				}
			}
			iterations++;
		} while (!done && (iterations < 100));
		
		 clock_gettime( CLOCK_REALTIME, &stop);
         timestamp = (double)(stop.tv_sec - start.tv_sec + (double) (stop.tv_nsec - start.tv_nsec)/(double) ns);
		 if(timestamp > max_val)
		 {
			max_val = timestamp; //Calculating the maximum execution time of frames for analysis
			printf("Max Time = %lf seconds\n",max_val);
		 }
		 
	    //Saving the images
		sprintf(buff, "Frame_%d.jpeg", frame_num++);
		imwrite(buff,mfblur);
		if(frame_num == 301)
		break;
		
        past_frame = pres_frame.clone();

        char c = cvWaitKey(33);
        if( c == 27 ) break;
    }

    cvReleaseCapture(&capture);
    cvDestroyWindow("Capture Example");
    return 0;
    
}
