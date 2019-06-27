/**************************************************************************************************
* File Name: q2.cpp
* Description: The code applies median filter on the greyscale image and displays it.
* Author: Amreeta Sengupta
* Date: 06/28/2019
* References: docs.opencv.org
* 			  http://www.cse.uaa.alaska.edu/~ssiewert/a485_code/simple-cv/
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
	Mat img1, img2, res_img;
	namedWindow("Output Image", CV_WINDOW_AUTOSIZE);
	
	img1 = imread("frame1.ppm", CV_LOAD_IMAGE_COLOR);
	if(!img1.data)  // Check for invalid input
    {
        printf("Could not open or find the Image\n");
        exit(-1);
    }
    extractChannel(img1,img2,1); //extract G band
    medianBlur(img2,res_img,3); //Apply Median filter using 3X3 kernel
	imshow("Output Image", res_img); //Display the Image
	
	while(1)
	{
	    char c = cvWaitKey(10);
	    if(c == 27)
	    {
			exit(1);
	    }
	}
	 
	return 1;
}
