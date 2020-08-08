#ifndef PROCESSING_H
#define PROCESSING_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <complex>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <typeinfo>
#include <vector>

using namespace std;

struct cp_struct;
struct grid_struct;
struct cp_struct // control point (角既point )
{
    int x_coord = 0;
    int y_coord = 0;
    float z_value = 0; // 小數做intepolation , 最尾先比人選round up 定 round down (普通round)
    // cv::Point3_< _Tp > Class Template Reference
};
// cv ::3dpoint

struct grid_struct {
    vector<cp_struct> point; // 	// block struct {width , heigh   }
    cv::Mat ref_plane; // plane既樣 （形容個order係左至右，上至下）
};

// value 入 matrix ,  之後補0   ,solve , 嘔 mat
// *  *  *   選一邊 (都是同一組數字)
// |  |  |
// *  *  *
// block 直橫線先處理  之後先掉中間d數入去處理

// auto support single plane fit (when  one region)  out ref plane ,
// 好多個4/6/9角result expolation

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

cv::Mat cal_and_cut(cv::Mat img, cv::Mat mask, int sample_size);

cv::Mat single_planefit(cv::Mat contour_region, cv::Mat mask_region,
    int sample_size, vector<vector<int>> M_B,
    vector<vector<float>> M_A, int num_of_sample,
    vector<vector<pair<int, int>>> rect_coord);
cv::Mat planefit(cv::Mat img, cv::Mat mask);
cv::Mat gen2_planefit(cv::Mat img, cv::Mat mask);

cv::Mat LR_planefit(cv::Mat contour_region, cv::Mat mask_region,
    int sample_size, int num_of_sample); // 1x2
cv::Mat TDLR_planefit(cv::Mat contour_region, cv::Mat mask_region,
    int sample_size, int num_of_sample); // 2x2
cv::Mat TMDLMR_planefit(cv::Mat contour_region, cv::Mat mask_region,
    int sample_size, int num_of_sample); // 3x3
cv::Mat general_planefit(cv::Mat contour_region, cv::Mat mask_region,
    int sample_size, int num_of_sample, int num_row, int num_col); // 3x3  //愈細mean愈細

int get_num_sample(cv::Mat img, cv::Mat mask, int sample_size);

cv::Mat gen2_segmentation(cv::Mat img, cv::Mat mask, int* Grid_size_x, int* Grid_size_y, int dimension, int* region_num);
cv::Mat segmentation(cv::Mat img, cv::Mat mask, int* Grid_size_x, int* Grid_size_y, int sample_size);

#endif