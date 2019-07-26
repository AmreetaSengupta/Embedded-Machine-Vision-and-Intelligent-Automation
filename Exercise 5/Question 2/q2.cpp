/**********************************************************************************************************
* File Name: q2.cpp
* Description: The code detects and maintains a record of number of pedastrians detected in a video.
* Author: Amreeta Sengupta
* Date: 07/26/2019
* References: docs.opencv.org
*             https://answers.opencv.org/question/179816/human-detection-and-tracking-opencv-c/
***********************************************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <syslog.h>

#include <opencv2/opencv.hpp>

#define ffmpeg_command "ffmpeg -r 30 -f Image2 -i Frame_%d.jpeg "

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
	  int i,frame_num = 0;
	  int show_flag=0, log_flag=0, store_flag=0;
	  char buff[100];
	  string str1;
	  const string src = argv[1];
	  Mat present_frame, result_frame;
	  openlog("LOG_RESULT",LOG_CONS,LOG_USER);
	  
	  for (i=2; i<argc; i++)
	  {
		
		if (string(argv[i]) == "--show")
		show_flag =1;
		
		if (string(argv[i]) == "--store")
		{
			 store_flag =1;
			 str1 = ffmpeg_command + (string)(argv[i+1]);
		}
				
		if (string(argv[i]) == "--log")
		log_flag =1;
	  }
	   
      VideoCapture cap(src);
      if(!cap.isOpened())
      {
	    cout << "Error opening video stream or file" <<endl;
	    return -1;
	  }
	  
	  //Default Pedestrian Detection Setting
	  HOGDescriptor hog;
      hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    
	  while(1)
	  {
		  
	    cap >> present_frame;
	  
	    // If the frame is empty, break immediately
	    if  (present_frame.empty())
	    break;
	     
	    result_frame = present_frame.clone();
        resize(result_frame,result_frame,Size(result_frame.cols*2, result_frame.rows*2));

        vector<Rect> found;
        vector<double> weights;

        hog.detectMultiScale(result_frame, found, weights);

        // Draw bounding rectangle
        for( size_t i = 0; i < found.size(); i++ )
        {
            Rect r = found[i];
            rectangle(result_frame, found[i], cv::Scalar(0,0,255), 3);
        }

        // Display the result
        if(show_flag == 1)
        {
			imshow("Detected Pedestrian", result_frame);
		}
	    
	    // Store the result
	    if(store_flag == 1)
	    {
			sprintf(buff,"Frame_%d.jpeg", frame_num++);
			imwrite(buff,result_frame);
		}
		
		// Log the result
		 if(log_flag == 1)
	    {
			syslog(LOG_INFO, "Number of Pedestrians detected = %d ", found.size());
			for( size_t i = 0; i < found.size(); i++ )
			{
			  syslog(LOG_INFO, "Coordinates of Pedastrian %d: x=%d y=%d\n",i,found[i].x+found[i].width/2,found[i].y+found[i].height/2);
			}
		}

	    // Press  ESC on keyboard to exit
	    char c = cvWaitKey(33);
        if( c == 27 ) break;
        
	  }
	     
	  system(str1.c_str());
	  
	  // When everything done, release the video capture object
	  cap.release();
	 
	  // Closes all the frames
	  destroyAllWindows();
	     
	  return 0;
}
