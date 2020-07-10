# Notes for OPENCV

> Basic IO

```cpp
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

	const cv::String filename = "./baby.jpg";
	cv::Mat img = cv::imread(filename, 1);	// cv::Mat

	if (img.empty()) {
		printf("could not load the image..");
		return -1;
	}

	cv::imshow("Image", img);
	cv::waitKey();

	return 0;
}

```

> Print Pixel number

```cpp
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

	const cv::String filename = "./baby.jpg";
	cv::Mat img = cv::imread(filename, 1);	// cv::Mat

	if (img.empty()) {
		printf("could not load the image..");
		return -1;
	}
	for (int r = 0; r < img.rows; ++r) {
		const uchar* ptr = img.ptr<uchar>(r);
		for (int c = 0; c < img.cols; ++c) {
			cout << setprecision(2) << (uint)ptr[c] << " ";
		}
		cout << "\n";
	}

	return 0;
}

```

> Edge

```cpp
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

	const cv::String filename = "./baby.jpg";
	cv::Mat img = cv::imread(filename, 1);	// cv::Mat

	if (img.empty()) {
		printf("could not load the image..");
		return -1;
	}

	int cols = (img.cols - 1) * img.channels();
	int rows = img.rows;
	int gap_col = img.channels();
	Mat dst = Mat::zeros(img.size(), img.type());
	for (int row = ((kernel_rows - 1) / 2);
		 row <= rows - ((kernel_rows - 1) / 2); ++row) {
		//  Mat.ptr<uchar>(int i = 0)
		//  获取像素矩阵的指针，索引i表示第几行，从0开始计行数。
		// 	获得当前行指针const uchar*  current = img.ptr<uchar>(row);

		//  获取当前像素点P(row, col)的像素值 p(row, col) = current[col]

		// unsigned 8bit  0~255
		const uchar* current_row = img.ptr<uchar>(row);
		const uchar* prior_row = img.ptr<uchar>(row);
		const uchar* next_row = img.ptr<uchar>(row);
		uchar* dst_row = dst.ptr<uchar>(row);
		for (int col = 0 + img.channels(); col <= cols; ++col) {
			dst_row[col] = saturate_cast<uchar>(
				4 * current_row[col] - current_row[col - gap_col] -
				current_row[col + gap_col] - prior_row[col] - next_row[col]);
		}
	}

	namedWindow("old");

	imshow("old", img);
	waitKey(0);

	namedWindow("new");

	imshow("new", dst);
	waitKey(0);

	return 0;
}

```

> Camera

```cpp
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

```

opencv4.0 <br>
vs studio 2017 <br>
Thresholding <br>
Plane fitting <br>
Edge extraction <br>
overkill <br>
underkill <br>
