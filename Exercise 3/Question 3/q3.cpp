/**************************************************************************************************
* File Name: q3.cpp
* Description: The code performs background elimination through frame differenciation on a video.
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

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
	  int frame_num = 0;
	  char buff[100];
	  Mat present_frame, past_frame, result_frame;
      VideoCapture cap("./Dark-Room-Laser-Spot-with-Clutter.mpeg");
      if(!cap.isOpened())
      {
	    cout << "Error!" <<endl;
	    return -1;
	  }
	  
	  cap >> past_frame;
	  while(1)
	  {
		cap >> present_frame;
	  
	    // If the frame is empty, break immediately
	    if (past_frame.empty() || present_frame.empty())
	    break;
	 
	    // Calculate the difference between the frames
		absdiff(past_frame,present_frame,result_frame); 
		
	    // Display the resulting frame
	    imshow( "Result Frame", result_frame );
		
		// Save the resulting frames
		sprintf(buff, "Frame_%d.pgm", frame_num++);
		imwrite(buff,result_frame);
		
		past_frame = present_frame.clone();
		
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
