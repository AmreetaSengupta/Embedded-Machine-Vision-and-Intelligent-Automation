/**********************************************************************************************
* File Name: simplecv.cpp
* Description: The code opens a window with an image in it which has a 4 pixel border
*              and a crosshair drawn at the center of the image.
* Author: Amreeta Sengupta
* Date: 06/14/2019
* References: http://www.cse.uaa.alaska.edu/~ssiewert/a485_code/simple-cv/ and docs.opencv.org
***********************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>


#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/contrib/contrib.hpp"

using namespace cv;
using namespace std;

// Default resolution is 360p
#define VRES_ROWS (360)
#define HRES_COLS (640)

#define ESC_KEY (27)

// Buffer for highest resolution visualization possible
unsigned char imagebuffer[1440*2560*3]; // 1440 rows, 2560 cols/row, 3 channel

int main(int argc, char **argv)
{
    int hres = HRES_COLS;
    int vres = VRES_ROWS;
    Mat basicimage(vres, hres, CV_8UC3, imagebuffer);
    int frameCnt=0;
    int border_size = 4;
    int border_type = BORDER_CONSTANT;
    Scalar val(100,255,255); 
    int marker_type = MARKER_CROSS;
    int marker_size = 25;
    int marker_thickness = 1;
    int marker_line_type = 8;
    int interpolation = INTER_LINEAR;

    printf("hres=%d, vres=%d\n", hres, vres);

    // interactive computer vision loop 
    namedWindow("Output Image", CV_WINDOW_AUTOSIZE);

    // read in default image
    if(vres == 360)
        basicimage = imread("Cactus360p.jpg", CV_LOAD_IMAGE_COLOR);
    else if(vres == 720)
        basicimage = imread("Cactus720p.jpg", CV_LOAD_IMAGE_COLOR);
    else if(vres == 1080)
        basicimage = imread("Cactus1080p.jpg", CV_LOAD_IMAGE_COLOR);
    else if(vres == 1440)
        basicimage = imread("Cactus1440p.jpg", CV_LOAD_IMAGE_COLOR);
        
    resize(basicimage,basicimage,Size(),0.5,0.5,interpolation); // resize image to 180 X 320
    
    cv::drawMarker(basicimage,Point(hres/4,vres/4),Scalar(0,255,255),marker_type,marker_size,marker_thickness,marker_line_type); // draw crosshair at center of image
    
    copyMakeBorder(basicimage,basicimage,border_size,border_size ,border_size ,border_size,border_type,val); // Make the border of the image

    if(!basicimage.data)  // Check for invalid input
    {
        printf("Could not open or find the refernece starting image\n");
        exit(-1);
    }

    // Create an IplImage mapping onto the basicimage Mat object
    //
    IplImage basicimageImg = basicimage;
	

    // Interactive LOOP
    //
    while(1)
    {
        frameCnt++;

        // Write a zero value into the image buffer
        //
        basicimageImg.imageData[frameCnt] = (unsigned char)0;

       imshow("Output Image", basicimage); 

        // set pace on shared memory sampling in msecs
        char c = cvWaitKey(10);

        if(c == ESC_KEY)
        {
            exit(1);
        }

    }
 
    return 1;

}
