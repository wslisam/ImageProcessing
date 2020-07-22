#include "processing.h"

int main(int argc, char **argv)
{

	// cv::Mat img = cv::imread("./Surface_Fitting_image/colorplane/test.bmp");
	// cv::Mat mask = cv::imread("./Surface_Fitting_image/colorplane/mask.bmp");
		cv::Mat img = cv::imread("./Surface_Fitting_image/1/input3.bmp");
	cv::Mat mask = cv::imread("./Surface_Fitting_image/1/mask.bmp");

	cv::Mat fit_img;

	if (img.empty())
	{
		cout << "!!! Failed imread(): image not found" << endl;
		return -1;
	}
	// cv::Mat test2 = color_bilinear(img);
	// cv::Mat test3 = color_bilinear_buildin(img);

	//change the img to grayscale first
	img = BGR2GRAY(img);

	cv::Mat test2 = gray_bilinear(img);
	cv::Mat test3 = gray_bilinear_buildin(img);

	// inverse the mask
	cv::Mat out_mask = ~mask;
	out_mask = BGR2GRAY(out_mask);

	cv::Mat after_filter = filter(img, out_mask);

	// cv::Mat test = gray_bilinear(after_filter);


	// int num_objects = ConnectedComponents(after_filter);

	// cv::Mat after_planefit = single_planefit(after_filter);
	// int num_objects = ConnectedComponents(after_filter);
	int num_objects = Contours(after_filter);


	cv::Mat after_planefit = single_planefit(img);

	

	// cv::imshow("sample ", img);
	// cv::imshow("mask after inverse", out_mask);
	// cv::imshow("after filter", after_filter);
	// cv::imshow("Bilinear Interpolation", test);
	// cv::imshow("Bilinear Interpolation2", test2);
	// cv::imshow("Bilinear Interpolation3", test3);

	cv::imshow("planefit", after_planefit);


	cv::waitKey(0);
	cv::destroyAllWindows();

	return 0;
}
