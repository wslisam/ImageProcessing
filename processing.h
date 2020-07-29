#ifndef PROCESSING_H
#define PROCESSING_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <complex>
#include <vector>
#include <typeinfo>

using namespace std;

struct cp_struct;
struct result_struct;
struct cp_struct // control point (角既point )
{
    int x_coord = 0;
    int y_coord = 0;
    int z_value = 0;
};

struct result_struct
{
    vector<cp_struct> point;
    cv::Mat ref_plane; // plane既樣 （形容個order係左至右，上至下）
};

//auto support single plane fit (when  one region)  out ref plane , 好多個4/6/9角result
//expolation

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

vector<vector<pair<int, int>>> rect_contours(cv::Mat img, vector<vector<cv::Point>> contours);

int cal_and_cut(cv::Mat img, cv::Mat mask, int Grid_size);
cv::Mat single_planefit(cv::Mat contour_region, cv::Mat mask_region, int sample_size, vector<vector<int>> M_B, vector<vector<float>> M_A, int num_of_sample, vector<vector<pair<int, int>>> rect_coord);
cv::Mat multi_planefit(cv::Mat contour_region, cv::Mat mask_region, int sample_size, vector<vector<int>> M_B, vector<vector<float>> M_A, int num_of_sample, vector<vector<pair<int, int>>> rect_coord);
int segmentation(cv::Mat img, cv::Mat mask, int *Grid_size_x, int *Grid_size_y, int sample_size);

#endif