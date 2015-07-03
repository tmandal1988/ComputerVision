#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include<iostream>
using namespace cv;
using namespace std;

int main(int argc,char** argv)

{

	if (argc!=2){

		cout<<"Usage <videoCapturetest1> <camera no>"<<endl;
		return -1;

	}

	VideoCapture cap(atoi(argv[1]));//open the default camera

    if(!cap.isOpened())//check if we succeeded
	return -1;

    namedWindow("Video Stream",1);

    for(;;){
	Mat frame;
	Mat framebgr;
	cap >> frame;
	//or cap.read(frame);

	cvtColor(frame,framebgr,COLOR_BGR2GRAY);
        imshow("Video Stream",framebgr);
	if(waitKey(30) >= 0) break;
    }

     return 0;
}


