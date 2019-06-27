/**************************************************************************************************
* File Name: q4.cpp
* Description: The code performs greyscale conversion on a video.
* Author: Amreeta Sengupta
* Date: 06/28/2019
* References: docs.opencv.org
* 			  http://ecee.colorado.edu/~siewerts/extra/ecen5763/ecen5763_code/simpler-capture/
*             https://www.learnopencv.com/read-write-and-display-a-video-using-opencv-cpp-python/
****************************************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
	  int frame_num = 0;
	  char buff[100];
	  Mat present_frame, result_frame;
      VideoCapture cap("./Dark-Room-Laser-Spot.mpeg");
      if(!cap.isOpened())
      {
	    cout << "Error opening video stream or file" <<endl;
	    return -1;
	  }
	  
	  while(1)
	  {
		  
	    cap >> present_frame;
	  
	    // If the frame is empty, break immediately
	    if  (present_frame.empty())
	    break;
	 
	    extractChannel(present_frame,result_frame,1); //extract G band
		
	    // Display the resulting frame
	    imshow( "Result Frame", result_frame );
	    
	    sprintf(buff, "Frame_%d.pgm", frame_num++);
		imwrite(buff,result_frame);

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
