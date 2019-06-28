/*************************************************************************************************************
* File Name: q6.cpp
* Description: The code is used to track the laser beam and mark the center with a crosshair.
* Author: Amreeta Sengupta
* Date: 06/28/2019
* References: 
* docs.opencv.org
* -> http://ecee.colorado.edu/~siewerts/extra/ecen5763/ecen5763_code/simpler-capture/
* -> https://www.learnopencv.com/read-write-and-display-a-video-using-opencv-cpp-python/
* -> https://docs.opencv.org/3.3.0/da/d0c/tutorial_bounding_rects_circles.html
* -> https://docs.opencv.org/3.0-beta/doc/tutorials/imgproc/shapedescriptors/moments/moments.html
**************************************************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

int thresh;
int max_thresh = 255;
Mat present_frame, past_frame,result_present, new_result, back_frame,result_past,drawing;
int marker_type = MARKER_CROSS;
int marker_size = 75;
int marker_thickness = 1;
int marker_line_type = 8;

void thresh_func()
{
  vector<vector<Point> > contours;
  vector<Vec4i> hierarchy;
  
  findContours( new_result, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );
  
  vector<vector<Point> > contours_poly( contours.size() );
  vector<Rect> boundRect( contours.size() );
  vector<Moments> m( contours.size() );
  vector<Point2f> p( contours.size() );
  int max_index = 0, area_max = 0;
   
  for( size_t i = 0; i < contours.size(); i++ )
  {
    approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
    boundRect[i] = boundingRect( Mat(contours_poly[i]) );
    m[i] = moments(contours[i],false);
	p[i]=Point2f(m[i].m10/m[i].m00, m[i].m01/m[i].m00); // center of the contour
	double area = contourArea(contours[i]);
	if(area>area_max)
	{
		area_max = area;
		max_index = i;
	}
  }
  drawing = Mat::zeros( new_result.size(), CV_8UC3 );
  for( size_t i = 0; i< contours.size(); i++ )
  {
    drawContours( drawing, contours_poly, (int)max_index, Scalar(0,255,0), 1, 8, vector<Vec4i>(), 0, Point() ); 
    rectangle( drawing, boundRect[max_index].tl(), boundRect[max_index].br(), Scalar(255,255,255), 2, 8, 0 ); // draw the rectagle around the contour
    cv::drawMarker(drawing,p[max_index],Scalar(255,255,255),marker_type,marker_size,marker_thickness,marker_line_type); // draw crosshair at the center
  }
}

int main( int argc, char** argv )
{
	  int frame_num = 0,i,j;
	  char buff[100];
      VideoCapture cap("./Light-Room-Laser-Spot-with-Clutter.mpeg");
      if(!cap.isOpened())
      {
	    cout << "Error opening video stream or file" <<endl;
	    return -1;
	  }
	  
	   cap >> past_frame;
	   extractChannel(past_frame,result_past,1);
	   	
	  while(1)
	  {
		  
	    cap >> present_frame;
	    
		extractChannel(present_frame,result_present,1); //extract G band
		
		// If the frame is empty, break immediately
	    if (result_past.empty() || result_present.empty())
	    break;
	    
		absdiff(result_past,result_present,back_frame); 
		medianBlur(back_frame,back_frame,3);
		
		thresh = 200;
		threshold(back_frame, new_result, thresh, max_thresh, THRESH_BINARY);

		if(countNonZero(new_result) == 0)
		{
			do
			{
				thresh = thresh - 30;
				threshold(back_frame, new_result, thresh, max_thresh, THRESH_BINARY);
			}
			while(countNonZero(new_result) < 10 && thresh != 30);
		}
		
	    thresh_func();
	    
	    // Display the resulting frame
	    imshow( "Result Frame", drawing);
	    
	    sprintf(buff, "Frame_%d.pgm", frame_num++);
		imwrite(buff,drawing);
		
		result_past = result_present.clone();
		
		// Press  ESC on keyboard to exit
		 char c = cvWaitKey(33);
	     if( c == 27 ) break;
        
	  }
	     
	  // When everything done, release the video capture object
	  cap.release();
	 
	  // Closes all the frames
	  destroyAllWindows();
	     
	  return 0;
}
