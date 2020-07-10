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

	//合并的
	addWeighted(grad_x, 0.5, grad_y, 0.5, 0, dst);
	namedWindow("x+y");
	imshow("x+y", dst);

	waitKey(0);
	return 0;
}
