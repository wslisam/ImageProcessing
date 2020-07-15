
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
	int diff = abs(current - last);

	if (diff > 10)
	{
		return true;
	}

	return false;
}

cv::Mat bilinear(cv::Mat img, double rx, double ry)
{

	//normal rx and ry = 1
	// get height and width
	int width = img.cols;
	int height = img.rows;
	int channel = img.channels();

	// get resized shape
	int resized_width = (int)(width * rx);
	int resized_height = (int)(height * ry);
	int x_before, y_before;
	double dx, dy;
	double val;

	// output image
	cv::Mat out = cv::Mat::zeros(resized_height, resized_width, CV_8UC3);

	// bi-linear interpolation
	for (int y = 0; y < resized_height; y++)
	{
		y_before = (int)floor(y / ry);
		y_before = fmin(y_before, height - 1);
		dy = y / ry - y_before;

		for (int x = 0; x < resized_width; x++)
		{
			x_before = (int)floor(x / rx);
			x_before = fmin(x_before, width - 1);
			dx = x / rx - x_before;

			// compute bi-linear
			for (int c = 0; c < channel; c++)
			{
				val = (1. - dx) * (1. - dy) * img.at<cv::Vec3b>(y_before, x_before)[c] +
					  dx * (1. - dy) * img.at<cv::Vec3b>(y_before, x_before + 1)[c] +
					  (1. - dx) * dy * img.at<cv::Vec3b>(y_before + 1, x_before)[c] +
					  dx * dy * img.at<cv::Vec3b>(y_before + 1, x_before)[c];

				// assign pixel to new position
				out.at<cv::Vec3b>(y, x)[c] = (uchar)val;
			}
		}
	}

	return out;
}

cv::Mat planefit(cv::Mat input, cv::Mat output, int num_row, int num_col, int threshold)
{
	int width = input.cols;
	int height = input.rows;
	cv::Mat out = cv::Mat::zeros(height, width, CV_8UC3);

	return out;
}

cv::Mat filter(cv::Mat input, cv::Mat mask)
{
	cv::Mat out;
	input.cv::Mat::copyTo(out, mask);

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

	cv::Mat after_filter = filter(img, out_mask);

	cv::Mat test = bilinear(after_filter, 1, 1);
	cv::imshow("sample ", img);
	cv::imshow("mask after inverse", out_mask);
	cv::imshow("after filter", after_filter);
	cv::imshow("Bilinear Interpolation", test);
	cv::waitKey(0);
	cv::destroyAllWindows();

	return 0;
}
