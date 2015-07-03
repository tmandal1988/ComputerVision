/*
 * CameraCalibration3D.cpp
 *
 *  Created on: Jun 30, 2015
 *      Author: dabang
 */

#include <cv.h>
#include <highgui.h>

using namespace cv;
using namespace std;

int main(int argc, char** argv) {

	if (argc != 3) {

		cout << "Usage <CameraCalibration> <camera no1> <camera no2>" << endl;
		return -1;

	}

	cout << "Reading Camera Calibration Files" << endl;

	FileStorage fsLeft, fsRight;

	fsLeft.open("LeftEye", FileStorage::READ);
	fsRight.open("LeftEye", FileStorage::READ);

	if (!fsLeft.isOpened() && fsRight.isOpened()) {
		cout << "Files failed to opened" << endl;
		return -1;
	}

	Mat IntrinsicLeft, IntrinsicRight;
	Mat distCoeffsLeft, distCoeffsRight;

	fsLeft["Intrinsic"] >> IntrinsicLeft;
	fsRight["Intrinsic"] >> IntrinsicRight;

	fsLeft["distCoeffs"] >> distCoeffsLeft;
	fsRight["distCoeffs"] >> distCoeffsRight;

	int numBoards = 20;
	int numCornersHor = 8;
	int numCornersVer = 6;

	int numSquares = numCornersHor * numCornersVer;
	Size board_sz = Size(numCornersHor, numCornersVer);

	VideoCapture captureLeft(atoi(argv[1])); //open user selected camera1
	VideoCapture captureRight(atoi(argv[2])); //open user selected camera2

	if (!captureLeft.isOpened() || !captureRight.isOpened()) {
		cout << "Camera Crapped out, Bye Bye" << endl;
		return -1;
	}

	vector<vector<Point3f> > object_points;
	vector<vector<Point2f> > image_pointsLeft, image_pointsRight;

	vector<Point2f> cornersLeft, cornersRight;

	vector<Point3f> obj;

	for (int j = 0; j < numSquares; j++)
		obj.push_back(Point3f(j / numCornersHor, j % numCornersHor, 0.0f));

	Mat imageLeft, imageRight;
	Mat gray_imageLeft, gray_imageRight;

	int successes = 0;
	while (successes < numBoards) {
		captureLeft >> imageLeft;
		captureRight >> imageRight;

		if (!imageLeft.empty() && !imageRight.empty()) {

			cvtColor(imageLeft, gray_imageLeft, CV_BGR2GRAY);
			cvtColor(imageRight, gray_imageRight, CV_BGR2GRAY);

			bool foundLeft = findChessboardCorners(imageLeft, board_sz,
					cornersLeft,
					CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

			bool foundRight = findChessboardCorners(imageRight, board_sz,
					cornersRight,
					CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

			if (foundLeft) {
				cornerSubPix(gray_imageLeft, cornersLeft, Size(11, 11),
						Size(-1, -1),
						TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30,
								0.1));
				drawChessboardCorners(gray_imageLeft, board_sz, cornersLeft,
						foundLeft);
			}

			if (foundRight) {
				cornerSubPix(gray_imageRight, cornersRight, Size(11, 11),
						Size(-1, -1),
						TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30,
								0.1));
				drawChessboardCorners(gray_imageRight, board_sz, cornersRight,
						foundRight);
			}

			imshow("imageLeft", gray_imageLeft);
			imshow("imageRight", gray_imageRight);

			int key = 0xff & waitKey(10);

			if ((key & 255) == 27 && foundLeft != 0 && foundRight != 0) {
				image_pointsLeft.push_back(cornersLeft);
				image_pointsRight.push_back(cornersRight);
				object_points.push_back(obj);

				printf("Snap stored!");

				successes++;

				if (successes >= numBoards)
					break;
			}
		}

	}

	Mat R, T, E, F;

	stereoCalibrate(object_points, image_pointsLeft, image_pointsRight,
			IntrinsicLeft, distCoeffsLeft, IntrinsicRight, distCoeffsRight,
			imageLeft.size(), R, T, E, F,
			cvTermCriteria(CV_TERMCRIT_ITER + CV_TERMCRIT_EPS, 100, 1e-5),
			CV_CALIB_SAME_FOCAL_LENGTH | CV_CALIB_ZERO_TANGENT_DIST
					| CV_CALIB_USE_INTRINSIC_GUESS);

	cout << "Saving Stereo Calibration File" << endl;

	FileStorage fsout("StereoCalib", FileStorage::WRITE);

	fsout << "IntrinsicLeft" << IntrinsicLeft;
	fsout << "IntrinsicRight" << IntrinsicRight;

	fsout << "distCoeffsLeft" << distCoeffsLeft;
	fsout << "distCoeffsRight" << distCoeffsRight;

	fsout << "R" << R;
	fsout << "T" << T;
	fsout << "E" << E;
	fsout << "F" << F;

	fsLeft.release();
	fsRight.release();

	printf("Done Calibration\n");

	printf("Starting Rectification\n");

	Mat R1, R2, P1, P2, Q;
	stereoRectify(IntrinsicLeft, distCoeffsLeft, IntrinsicRight,
			distCoeffsRight, imageLeft.size(), R, T, R1, R2, P1, P2, Q);
	fsout << "R1" << R1;
	fsout << "R2" << R2;
	fsout << "P1" << P1;
	fsout << "P2" << P2;
	fsout << "Q" << Q;

	fsout.release();

	printf("Done Rectification\n");

	printf("Applying Undistort\n");

	Mat mapLeftx, mapLefty, mapRightx, mapRighty;
	Mat imgULeft, imgURight;

	initUndistortRectifyMap(IntrinsicLeft, distCoeffsLeft, R1, P1,
			imageLeft.size(), CV_32FC1, mapLeftx, mapRighty);
	initUndistortRectifyMap(IntrinsicRight, distCoeffsRight, R2, P2,
			imageRight.size(), CV_32FC1, mapLeftx, mapRighty);

	while (1) {
		captureLeft >> imageLeft;
		captureLeft >> imageRight;

		if (!imageLeft.empty() && !imageRight.empty()) {

			remap(imageLeft, imgULeft, mapLeftx, mapRighty, INTER_LINEAR,
					BORDER_CONSTANT, Scalar());
			remap(imageRight, imgURight, mapLeftx, mapRighty, INTER_LINEAR,
					BORDER_CONSTANT, Scalar());

			imshow("imageLeft", imgULeft);
			imshow("imageRight", imgURight);

			int key = 0xff & waitKey(captureLeft.isOpened() ? 50 : 500);
			if ((key & 255) == 27)
				break;
		}

	}

	captureLeft.release();
	captureRight.release();

	return (0);
}

