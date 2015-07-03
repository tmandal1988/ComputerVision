#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;

//Global variables
Mat dst,frame,frameGray;
Mat detected_edges;

  int edgeThresh = 1;
  int lowThreshold;
  int const max_lowThreshold = 100;
  int ratio = 3;
  int kernel_size = 3;

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{
  /// Reduce noise with a kernel 3x3
  blur( frameGray, detected_edges, Size(3,3) );

  /// Canny detector
  Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

  /// Using Canny's output as a mask, we display our result
  dst = Scalar::all(0);
frame.copyTo( dst, detected_edges);
  imshow( "EdgeMap", dst );
 }

int main(int argc,char** argv){

	if (argc!=2){
		
		std::cout<<"Usage <exampleEdgeDetector> <camera no>"<<std::endl;
		return -1;
	
	}

	VideoCapture cap(atoi(argv[1]));//open the default camera    

    if(!cap.isOpened())//check if we succeeded
		return -1;

	namedWindow("FrameCaptured",CV_WINDOW_AUTOSIZE);

	 /// Create a window
  	namedWindow( "EdgeMap", CV_WINDOW_AUTOSIZE );

  	createTrackbar("Min Threshold: ","EdgeMap",&lowThreshold,max_lowThreshold,CannyThreshold);

	
	for(;;){
		cap >> frame;
		
		imshow("FrameCaptured",frame);
		//create a matrix of same type and size as frame
		dst.create(frame.size(),frame.type());
	 	/// Convert the image to grayscale

	 	CannyThreshold(0,0);
  		cvtColor( frame, frameGray, CV_BGR2GRAY ); 	
  		if(waitKey(30) >= 0) break;

  	}


}