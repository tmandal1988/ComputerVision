#include "opencv2/highgui/highgui.hpp"
#include "opencv2/opencv.hpp"
#include<iostream>
using namespace cv;
using namespace std;

int main(int argc,char** argv){

	const char* leftWindow="leftWindow";
	const char* rightWindow="rightWindow";

	VideoCapture capLeft(1);
	VideoCapture capRight(2);

	if(!capLeft.isOpened()){
		cout<<"Error Opening Left Camera"<<endl;
		return -1;
	}

	if(!capRight.isOpened()){
		cout<<"Error Opening Right Camera"<<endl;
		return -1;
	}

	cout<<"Successfully Opened Left and Right Camera"<<endl;

	namedWindow(leftWindow,1);
	namedWindow(rightWindow,1);

	for(;;){

		Mat leftFrame;
		Mat rightFrame;
		capLeft>>leftFrame;
		capRight>>rightFrame;

		imshow(leftWindow,leftFrame);
		imshow(rightWindow,rightFrame);

		if(waitKey(30) >= 0) break;
	}

	return 0;


}