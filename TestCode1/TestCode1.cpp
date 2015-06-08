#include "opencv2/highgui/highgui.hpp"
#include<iostream>

using namespace cv;
using namespace std;

int main(int argc,char** argv)
{
	if(argc!=2){
		cout<<"Usage <TestCode1> <image>"<<endl;
		return -1;		
	}

	Mat img=imread(argv[1],CV_LOAD_IMAGE_COLOR);
	imshow("Test Image",img);
	waitKey(0);

	return 0;
}
