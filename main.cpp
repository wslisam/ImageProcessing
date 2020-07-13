
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
// using namespace cv;

bool light_diff ( int current , int last){
	// calculate the difference of that block / pixel

}

cv::Mat planefit(cv::Mat input, cv::Mat output, int num_row, int num_col, int threshold)
{

}

cv::Mat filter(cv::Mat input, cv::Mat mask, uchar threshold)
{
	cv::Mat output = cv::Mat::zeros(cv::Size(100, 100), CV_8UC1); //0-255 one channel
	

}

cv::Mat inverse(cv::Mat input)
{
	cv::Mat output = cv::Mat::zeros(cv::Size(100, 100), CV_8UC1); //0-255 one channel
		// inverse the black and write in this case
	return output;
}

int main(int argc, char **argv)
{
	cv::Mat img = cv::imread("input3.bmp");
	cv::Mat mask = cv::imread("Mask.bmp");
	cv::Mat fit_img ;
	mask = inverse(mask);

	//change the img to grayscale first 

	fit_img = filter(img, mask, 125);

	return 0;
}
