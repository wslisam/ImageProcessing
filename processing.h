#ifndef PROCESSING_H
#define PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <complex>

using namespace std;
// using namespace cv;

bool light_diff(int current, int last);
int ConnectedComponents(cv::Mat img);
cv::Mat color_bilinear(cv::Mat img, double rx = 1, double ry = 1);
cv::Mat gray_bilinear(cv::Mat img);
cv::Mat gray_bilinear_buildin(cv::Mat img);
cv::Mat color_bilinear_buildin(cv::Mat img);
cv::Mat planefit(cv::Mat input, cv::Mat output, int num_row, int num_col, int threshold);
cv::Mat filter(cv::Mat input, cv::Mat mask);
cv::Mat BGR2GRAY(cv::Mat input);
cv::Mat thresholding(cv::Mat input, int threshold);

#endif