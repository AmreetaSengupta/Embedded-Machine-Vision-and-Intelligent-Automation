/**********************************************************************************************************
* File Name: q2.cpp
* Description: The code detects and maintains a record of number of pedastrians detected in a video.
* Author: Amreeta Sengupta
* Date: 07/26/2019
* References: docs.opencv.org
*             https://answers.opencv.org/question/179816/human-detection-and-tracking-opencv-c/
* 			  Embedded Machine Vision and Intelligent Automation Exercise 2 by Amreeta Sengupta
***********************************************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <syslog.h>

#include <opencv2/opencv.hpp>

#define ffmpeg_command "ffmpeg -r 30 -f Image2 -i Frame_%d.jpeg "
#define ns 1000000000

using namespace cv;
using namespace std;

struct timespec start,stop;

int main( int argc, char** argv )
{
	  int i,frame_num = 0, frame_count = 0;
	  int show_flag=0, log_flag=0, store_flag=0;
	  int count_val = 0;
	  char buff[100];
	  string str1;
	  const string src = argv[1];
	  double timestamp, avg_time;
	  double max_val=0;
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
		
		clock_gettime( CLOCK_REALTIME, &start); 
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
        
        clock_gettime( CLOCK_REALTIME, &stop);
        
        // Timestamp Calculation
        timestamp = (double)(stop.tv_sec - start.tv_sec + (double) (stop.tv_nsec - start.tv_nsec)/(double) ns); 
        avg_time += timestamp;
		if(timestamp > max_val)
		{
			max_val = timestamp; // Calculating the maximum execution time of frames for analysis
		}
		count_val++; 

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
			syslog(LOG_INFO, "Number of Pedestrians detected in Frame %d = %d ", frame_count++,(int)found.size());
			for( size_t i = 0; i < found.size(); i++ )
			{
				syslog(LOG_INFO, "Coordinates of Pedastrian %d: x=%d y=%d\n",(int)i,(int)(found[i].x+found[i].width/2),(int)(found[i].y+found[i].height/2));
			}
		}
		

	    // Press  ESC on keyboard to exit
	    char c = cvWaitKey(33);
        if( c == 27 ) break;
        
	  }
	  
	  // Execution Time analysis
	  printf("Average Time = %lf seconds\n",avg_time/count_val);
	  printf("Max Time = %lf seconds\n",max_val);
	  
	  // Re-encoding the video
	  system(str1.c_str()); 
	  
	  // When everything done, release the video capture object
	  cap.release();
	 
	  // Closes all the frames
	  destroyAllWindows();
	     
	  return 0;
}
