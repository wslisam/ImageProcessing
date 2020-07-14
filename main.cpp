
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <complex>

using namespace std;
//using namespace cv;

bool light_diff(int current, int last)
{
	// calculate the difference of that block / pixel
}

cv::Mat planefit(cv::Mat input, cv::Mat output, int num_row, int num_col, int threshold)
{
}

cv::Mat filter(cv::Mat input, cv::Mat mask)
{
	// get height and width
	// int width = input.cols;
	// int height = input.rows;
	cv::Mat out;
	// = cv::Mat::zeros(height, width, CV_8UC1);

	// cv::bitwise_and(input, input, mask);
	input.copyTo(out, mask);

	return out;
}

cv::Mat BGR2GRAY(cv::Mat input)
{

	// get height and width
	int width = input.cols;
	int height = input.rows;

	// prepare output
	cv::Mat out = cv::Mat::zeros(height, width, CV_8UC1); //0-255 one channel
														  // inverse the black and write in this case

	// each y, x
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			// BGR -> Gray
			//Formula to determine brightness of RGB color
			out.at<uchar>(y, x) = 0.2126 * (float)input.at<cv::Vec3b>(y, x)[2] + 0.7152 * (float)input.at<cv::Vec3b>(y, x)[1] + 0.0722 * (float)input.at<cv::Vec3b>(y, x)[0];
		}
	}

	return out;
}

cv::Mat thresholding(cv::Mat input, int threshold)
{
	int width = input.cols;
	int height = input.rows;

	// prepare output
	cv::Mat out = cv::Mat::zeros(height, width, CV_8UC1);

	// each y, x
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			// Binarize
			if (input.at<uchar>(y, x) > threshold)
			{
				out.at<uchar>(y, x) = 0;
			}
			else
			{
				out.at<uchar>(y, x) = 255;
			}
		}
	}

	return out;
}

int main(int argc, char **argv)
{
	cv::Mat img = cv::imread("./1/input3.bmp");
	cv::Mat mask = cv::imread("./1/Mask.bmp");
	cv::Mat fit_img;

	//change the img to grayscale first
	img = BGR2GRAY(img);

	//inverse the mask
	cv::Mat out_mask = ~mask;
	out_mask = BGR2GRAY(out_mask);

	// cv::Mat after_filter = filter(img, out_mask);
	cv::Mat after_filter;
	img.cv::Mat::copyTo(after_filter, out_mask);
	// cv::bitwise_and(img, after_filter, mask);

	// fit_img = filter(img, mask, 125);
	cv::imshow("sample", img);
	cv::imshow("mask after inverse", out_mask);
	cv::imshow("after filter", after_filter);
	cv::waitKey(0);
	// cv::destroyAllWindows();

	return 0;
}
