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
struct final_struct;
struct cp_struct { // control point (角既point)
    int x_coord = 0;
    int y_coord = 0;
    float z_value = 0;
};

struct grid_struct {
    vector<cp_struct> point; // block struct {width , heigh}
    cv::Mat ref_plane; // planeInter既樣 （形容個order係左至右，上至下）
};

struct final_struct {
    vector<grid_struct> grid_vector;
    cv::Mat whole_plane;
    cv::Mat dark;
    cv::Mat bright;
    double area;
};

int find_num_obj_using_ConnectedComponents(cv::Mat img);
int find_num_obj_using_contours(cv::Mat img);
int get_num_sample(cv::Mat img, cv::Mat mask, int sample_size); // calculate number of sample

cv::Mat BGR2GRAY(cv::Mat input);
cv::Mat filter(cv::Mat input, cv::Mat mask); // masking
vector<vector<pair<int, int>>> rect_contours(cv::Mat img, vector<vector<cv::Point>> contours);

final_struct planefit(cv::Mat img, cv::Mat mask, int num_row, int num_col); //function to call different kind of planefit
grid_struct general_planefit(cv::Mat contour_region, cv::Mat mask_region, int sample_size, int num_of_sample, int num_row, int num_col, vector<pair<int, int>> rect); // mean decrease if num_row , num col increase

cv::Mat Diff(cv::Mat filter, cv::Mat refplane); // Compute absolute difference between just after filter and the ref plane
cv::Mat bright_and_dark(cv::Mat old, cv::Mat thenew, int threshold); // show the dark and bright defect in the same function
cv::Mat dark(cv::Mat old, cv::Mat thenew, int threshold); // show the dark defect
cv::Mat bright(cv::Mat old, cv::Mat thenew, int threshold); // show the bright defect
cv::Mat Binarize(cv::Mat gray, int threshold); // apply thresholding

int find_defects_using_contours(cv::Mat img, double total_area);
int find_defects_using_ConnectedComponents(cv::Mat img, double total_area);
static cv::Scalar randomColor( cv::RNG& rng );

#endif