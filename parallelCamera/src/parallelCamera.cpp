/*
 * parallelCamera.cpp
 *
 *  Created on: Jun 30, 2015
 *      Author: dabang
 */

#include <cv.h>
#include <highgui.h>

#include <pthread.h>

using namespace cv;
using namespace std;

Mat capture2;

void* Camera2Capture(void*) {

	VideoCapture camera2(1);

	while(1){
		camera2>>capture2;
	}

	camera2.release();
	return 0;
}

int main(int argc, char** argv) {

	VideoCapture camera1(0);

	pthread_t camera2Thread;

	int iret1 = pthread_create(&camera2Thread, NULL, Camera2Capture,
	NULL);
	if (iret1) {
		fprintf(stderr, "Error - pthread_create() return code: %d\n", iret1);
		exit(EXIT_FAILURE);
	}

	while(1){
		Mat capture1;
		camera1>>capture1;
		imshow("Camera1",capture1);

		if(!capture2.empty())
			imshow("Camera2",capture2);

		int key=waitKey(2);
		if(key==27)
			break;

	}

	camera1.release();
	return 0;

}
