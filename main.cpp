#include <iostream>
//#include <opencv2/core.hpp>
//#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#define kernel_rows 3

using namespace std;

using namespace cv;

int main(int, char**) {
	cout << "test cv";

	Mat frame;
	Mat gray, edge;
	VideoCapture cap;

	cap.open(0);

	if (!cap.isOpened()) {
		cout << "error opening camera" << endl;
		return -1;
	}

	for (;;) {
		cap.read(frame);
		if (frame.empty()) {
			cout << "blank image" << endl;
			break;
		}

		cvtColor(frame, gray, COLOR_BGR2GRAY);
		blur(gray, edge, Size(30, 30));
		Canny(edge, edge, 3, 9);
		imshow("Webcam testing windows", edge);
		if (waitKey(5) > 0) {
			cout << "program terminated" << endl;
			break;
		}
	}

	return 0;
}
