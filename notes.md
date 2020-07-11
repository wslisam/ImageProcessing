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

> Thresholding

> applies fixed threshold to the image<br>
>
> ```cpp
> CV_EXPORTS_W double threshold( InputArray src, OutputArray dst , double thresh, double maxval, int type );
> ```

````

```cpp
#include <opencv2/opencv.hpp>

// #include <opencv2/core/core.hpp>
// #include <opencv2/highgui/highgui.hpp>
// #include <opencv2/imgproc/imgproc.hpp>

#include <string>

using namespace cv;

void main() {
	cv::Mat image = imread("threshold.jpg");
	cv::namedWindow("original");
	cv::imshow("original", image);

	char windowstring[] = "result 0";
	char imagestring[] = "result 0.jpg";
	cv::Mat result;
	enum thresholdtype {
		THRESH_BINARY,     // usually
		THRESH_BINARY_INV, // usually
		THRESH_TRUNC,
		THRESH_TOZERO,
		THRESH_TOZERO_INV
	};

	for (int thresh = 0; thresh < 5; thresh++) {
		/*0：二進制閾值 1：反二進制閾值 2：截斷閾值 3：0閾值 4：反0閾值
		 */
		threshold(image, result, 150, 255,
				  thresholdtype(thresh));  //改變參數實現不同的閾值
		cv::namedWindow(windowstring);
		cv::imshow(windowstring, result);  // show output
		cv::imwrite(imagestring, result);
		windowstring[7]++;
		imagestring[7]++;
	}

	waitKey(0);

}


````

```cpp
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
// #include <opencv2/opencv.hpp>

using namespace cv;

// cv::Sobel(InputArray Src,	// input image
//   OutputArray dst,	// output image ( same size )
//   int depth, int dx, int dy,
//   int ksize,  // kernel大小 1,3,5,7
//   double scale = 1, double delta = 0, int borderType = BORDER_DEFAULT);

int main() {
	Mat grad_x, grad_y, dst;
	Mat src = imread("lenna.jpg");

	namedWindow("OLD");
	imshow("OLD", src);

	// x-dir
	Sobel(src, grad_x, CV_8U, 1, 0, 3, 1, 0, BORDER_DEFAULT);

	namedWindow("x-dir");
	imshow("x-dir", grad_x);

	// y-dir
	Sobel(src, grad_y, CV_8U, 0, 1, 3, 1, 0, BORDER_DEFAULT);
	namedWindow("y-dir");
	imshow("y-dir", grad_y);

	//merge
	addWeighted(grad_x, 0.5, grad_y, 0.5, 0, dst);
	namedWindow("x+y");
	imshow("x+y", dst);

	waitKey(0);
	return 0;
}

```

> Simulated background

```cpp
#include <opencv2/highgui/highgui_c.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char **argv) {
	Mat z = imread("color2.webp", IMREAD_COLOR);

	Mat M = Mat_<double>(z.rows * z.cols, 6);
	Mat I = Mat_<double>(z.rows * z.cols, 1);
	for (int i = 0; i < z.rows; i++)
		for (int j = 0; j < z.cols; j++) {
			double x = (j - z.cols / 2) / double(z.cols),
				   y = (i - z.rows / 2) / double(z.rows);
			M.at<double>(i * z.cols + j, 0) = x * x;
			M.at<double>(i * z.cols + j, 1) = y * y;
			M.at<double>(i * z.cols + j, 2) = x * y;
			M.at<double>(i * z.cols + j, 3) = x;
			M.at<double>(i * z.cols + j, 4) = y;
			M.at<double>(i * z.cols + j, 5) = 1;
			I.at<double>(i * z.cols + j, 0) = z.at<uchar>(i, j);
		}
	SVD s(M);
	Mat q;
	s.backSubst(I, q);
	std::cout << q;
	imshow("Orignal", z);
	std::cout << q.at<double>(2, 0);
	Mat background(z.rows, z.cols, CV_8UC1);
	for (int i = 0; i < z.rows; i++)
		for (int j = 0; j < z.cols; j++) {
			double x = (j - z.cols / 2) / double(z.cols),
				   y = (i - z.rows / 2) / double(z.rows);
			double quad = q.at<double>(0, 0) * x * x +
						  q.at<double>(1, 0) * y * y +
						  q.at<double>(2, 0) * x * y;
			quad += q.at<double>(3, 0) * x + q.at<double>(4, 0) * y +
					q.at<double>(5, 0);
			background.at<uchar>(i, j) = saturate_cast<uchar>(quad);
		}
	imshow("Simulated background", background);
	waitKey();
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
