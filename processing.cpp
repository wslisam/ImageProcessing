#include "processing.h"

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

int ConnectedComponents(cv::Mat img)
{
	// Use connected components to divide our possibles parts of images
	cv::Mat labels;
	int num_objects = connectedComponents(img, labels); // connectivity=8 , type = CV_32S

	// Check the number of objects detected
	//background = 1
	if (num_objects < 2)
	{
		cout << "No objects detected" << endl;
		return num_objects - 1;
	}
	else
	{
		cout << "Number of objects detected: " << num_objects - 1 << endl;
	}

	return num_objects - 1;
}

cv::Mat color_bilinear(cv::Mat img, double rx, double ry)
{
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

cv::Mat gray_bilinear(cv::Mat img)
{
	int width = img.cols;
	int height = img.rows;
	int channel = img.channels();

	int x_before, y_before;
	double dx, dy;
	double val;

	cv::Mat out = cv::Mat::zeros(height, width, CV_8UC1);

	// bi-linear interpolation
	for (int y = 0; y < height; y++)
	{
		y_before = (int)floor(y); //The floor() function takes a single argument and returns a value of type double, float or long double type.
		y_before = fmin(y_before, height - 1);
		dy = y - y_before;

		for (int x = 0; x < width; x++)
		{
			x_before = (int)floor(x);
			x_before = fmin(x_before, width - 1);
			dx = x - x_before;

			val = (1.0 - dx) * (1.0 - dy) * img.at<uchar>(y_before, x_before) +
				  dx * (1.0 - dy) * img.at<uchar>(y_before, x_before + 1) +
				  (1.0 - dx) * dy * img.at<uchar>(y_before + 1, x_before) +
				  dx * dy * img.at<uchar>(y_before + 1, x_before);

			out.at<uchar>(y, x) = (uchar)val;
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
