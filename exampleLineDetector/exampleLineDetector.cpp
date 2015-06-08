#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

//Global Variables

Mat src, edges;
Mat srcGray;
Mat standardHough, probabilisticHough;
int min_threshold=50;
int maxTrackbar=150;

const char* standard_name = "Standard Hough Lines Demo";
const char* probabilistic_name = "Probabilistic Hough Lines Demo";

int Strackbar=maxTrackbar;
int Ptrackbar=maxTrackbar;

//function headers

void StandardHough(int,void*);
void ProbabilisticHough(int,void*);

//Main
int main(int argc, char** argv){

	if (argc!=2){

		std::cout<<"Usage <exampleLineDetector> <camera no>"<<std::endl;
		return -1;
	}

	VideoCapture cap(atoi(argv[1]));//open the default camera    

    if(!cap.isOpened())//check if we succeeded
    	return -1;

    namedWindow("FrameCaptured",CV_WINDOW_AUTOSIZE);

    cap >> src;

    //pass the image to gray
    cvtColor(src,srcGray,CV_BGR2GRAY);

    //Apply Canny edge detector
    Canny(srcGray,edges,50,200,3);


   /// Create Trackbars for Thresholds
    char thresh_label[50];
    sprintf( thresh_label, "Thres: %d + input", min_threshold );
    namedWindow(standard_name,CV_WINDOW_AUTOSIZE);
    createTrackbar(thresh_label,standard_name,&Strackbar,maxTrackbar,StandardHough);

    namedWindow(probabilistic_name,CV_WINDOW_AUTOSIZE);
    createTrackbar(thresh_label,probabilistic_name,&Ptrackbar,maxTrackbar,ProbabilisticHough);

    //Initialize
    imshow("FrameCaptured",src);
    StandardHough(0,0);
    ProbabilisticHough(0,0);
    waitKey(0);
    return 0;
}

void StandardHough(int, void*){
	std::vector<Vec2f> Slines;
	cvtColor(edges,standardHough,CV_GRAY2BGR);

	//use standard Hough Transform
	HoughLines(edges,Slines,1,CV_PI/180,min_threshold+Strackbar,0,0);

	//Show the rsult
	for( size_t i=0; i<Slines.size();i++){

		float r=Slines[i][0],t=Slines[i][1];
		double cos_t=cos(t), sin_t=sin(t);
		double x0=r*cos_t,y0=r*sin_t;
		double alpha=1000.00;

		Point pt1(cvRound(x0+alpha*(-sin_t)),cvRound(y0+alpha*(cos_t)));
		Point pt2(cvRound(x0-alpha*(-sin_t)),cvRound(y0-alpha*(cos_t)));
		line( standardHough, pt1, pt2, Scalar(255,0,0), 3, CV_AA);

		imshow(standard_name,standardHough);

	}
}

void ProbabilisticHough(int, void*){

	vector<Vec4i> Plines;
	cvtColor( edges, probabilisticHough, COLOR_GRAY2BGR );

	  /// 2. Use Probabilistic Hough Transform
	HoughLinesP( edges, Plines, 1, CV_PI/180, min_threshold + Ptrackbar, 30, 10 );
	for(size_t i=0; i<Plines.size();i++){
		Vec4i l=Plines[i];
		line(probabilisticHough,Point(l[0],l[1]),Point(l[2],l[3]),Scalar(255,0,0), 3, CV_AA);
	}

	imshow( probabilistic_name, probabilisticHough);
}
