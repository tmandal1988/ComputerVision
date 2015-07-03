#include <cv.h>
#include <highgui.h>

#include <pthread.h>

using namespace cv;
using namespace std;

int tanmay = 0;
int successes = 0;

void* UserInputFunction(void*) {

	while (successes < 10) {

		if (tanmay != 0) {
			printf("Enter Y to save Picture: ");
			scanf("%c", &tanmay);
		}

	}

	return 0;

}

int main(int argc, char** argv) {

	if (argc != 2) {

		cout << "Usage <CameraCalibration> <camera no>" << endl;
		return -1;

	}

	FileStorage fs("LeftEye", FileStorage::WRITE);

	int numBoards = 20;
	int numCornersHor = 8;
	int numCornersVer = 6;

	pthread_t UserInputThread;

	/*	int iret1 = pthread_create(&UserInputThread, NULL, UserInputFunction,
	 NULL);
	 if (iret1) {
	 fprintf(stderr, "Error - pthread_create() return code: %d\n", iret1);
	 exit(EXIT_FAILURE);
	 }*/

	/*	printf("Enter number of corners along width: ");
	 scanf("%d", &numCornersHor);

	 printf("Enter number of corners along height: ");
	 scanf("%d", &numCornersVer);

	 printf("Enter number of board picture to capture: ");
	 scanf("%d", &numBoards);*/

	int numSquares = numCornersHor * numCornersVer;

	Size board_sz = Size(numCornersHor, numCornersVer);

	VideoCapture capture(atoi(argv[1])); //open user selected camera

	vector<vector<Point3f> > object_points;
	vector<vector<Point2f> > image_points;

	vector<Point2f> corners;

	Mat image;
	Mat gray_image;
	capture >> image;

	vector<Point3f> obj;

	for (int j = 0; j < numSquares; j++)
		obj.push_back(Point3f(j / numCornersHor, j % numCornersHor, 0.0f));

	while (successes < numBoards) {
		cvtColor(image, gray_image, CV_BGR2GRAY);

		bool found = findChessboardCorners(image, board_sz, corners,
		CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

		if (found) {
			cornerSubPix(gray_image, corners, Size(11, 11), Size(-1, -1),
					TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			drawChessboardCorners(gray_image, board_sz, corners, found);
		}

		//imshow("win1", image);
		imshow("win2", gray_image);

		capture >> image;

		/*		printf("Enter Y to save Picture: ");
		 scanf("%c", &tanmay);*/

		int key = 0xff & waitKey(capture.isOpened() ? 50 : 500);

		cout << key << endl;

		if ((key & 255) == 27 && found != 0) {
			image_points.push_back(corners);
			object_points.push_back(obj);

			printf("Snap stored!");

			successes++;

			if (successes >= numBoards)
				break;
		}
	}

	Mat intrinsic = Mat(3, 3, CV_32FC1);
	Mat distCoeffs;
	vector<Mat> rvecs;
	vector<Mat> tvecs;

	intrinsic.ptr<float>(0)[0] = 1;
	intrinsic.ptr<float>(1)[1] = 1;

	calibrateCamera(object_points, image_points, image.size(), intrinsic,
			distCoeffs, rvecs, tvecs);

	Mat imageUndistorted;
	while (1) {
		capture >> image;
		undistort(image, imageUndistorted, intrinsic, distCoeffs);

		imshow("win1", image);
		imshow("win2", imageUndistorted);
		int key = 0xff & waitKey(capture.isOpened() ? 50 : 500);
		if ((key & 255) == 27)
			break;
	}

	fs << "Intrinsic" << intrinsic;
	fs << "distCoeffs" << distCoeffs;
	fs << "rvecs" << rvecs;
	fs << "tvecs" << tvecs;

	fs.release();

	cout << "write done" << endl;

	capture.release();

	/*pthread_join( UserInputThread, NULL);*/

	return 0;

}
