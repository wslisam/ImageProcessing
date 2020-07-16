#include "processing.h"

int main(int argc, char **argv)
{

	cv::Mat img = cv::imread("./Surface_Fitting_image/3/input3.bmp");
	cv::Mat mask = cv::imread("./Surface_Fitting_image/3/Mask.bmp");

	cv::Mat fit_img;

	if (img.empty())
	{
		cout << "!!! Failed imread(): image not found" << endl;
		return -1;
	}
	// cv::Mat test2 = color_bilinear(img);
	//change the img to grayscale first
	img = BGR2GRAY(img);

	//inverse the mask
	cv::Mat out_mask = ~mask;
	out_mask = BGR2GRAY(out_mask);

	cv::Mat after_filter = filter(img, out_mask);

	// cv::Mat test = gray_bilinear(after_filter);

	int num_objects = ConnectedComponents(after_filter);

	cv::imshow("sample ", img);
	cv::imshow("mask after inverse", out_mask);
	cv::imshow("after filter", after_filter);
	// cv::imshow("Bilinear Interpolation", test);
	// cv::imshow("Bilinear Interpolation2", test2);

	cv::waitKey(0);
	cv::destroyAllWindows();

	return 0;
}
