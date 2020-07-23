#ifndef PROCESSING_H
#define PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <complex>
#include <vector>

using namespace std;
// using namespace cv;

bool light_diff(int current, int last);
int find_num_obj_using_ConnectedComponents(cv::Mat img);
int find_num_obj_using_contours(cv::Mat img);
cv::Mat BGR2GRAY(cv::Mat input);
cv::Mat thresholding(cv::Mat input, int threshold);
cv::Mat filter(cv::Mat input, cv::Mat mask);
cv::Mat gray_bilinear(cv::Mat img);
cv::Mat color_bilinear(cv::Mat img, double rx = 1, double ry = 1);
cv::Mat gray_bilinear_buildin(cv::Mat img, double r1 = 0.0, double r2 = 0.0);
cv::Mat color_bilinear_buildin(cv::Mat img, double r1 = 0.0, double r2 = 0.0);
cv::Mat single_planefit(cv::Mat img, cv::Mat mask);
cv::Mat segmentation(cv::Mat input, int x_pos, int y_pos, int w, int h);
void rect_contours(cv::Mat img, vector<vector<cv::Point>>  contours);

#endif