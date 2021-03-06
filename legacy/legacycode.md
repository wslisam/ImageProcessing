processing.h

```cpp
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
```


```cpp
#include "processing.h"

bool light_diff(int current, int last)
{
    // calculate the difference of that block / pixel
    int diff = abs(current - last);

    if (diff > 10) // big difference !?
    {
        return true;
    }

    return false;
}

int find_num_obj_using_ConnectedComponents(cv::Mat img)
{
    // Use connected components to divide our possibles parts of images
    cv::Mat labels;
    int num_objects = connectedComponents(img, labels); // connectivity=8 , type = CV_32S

    // Check the number of objects detected
    // background = 1
    if (num_objects < 2) {
        cout << "No objects detected" << endl;
        return num_objects - 1;
    } else {
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
    for (int y = 0; y < resized_height; y++) {
        y_before = (int)floor(y / ry);
        y_before = fmin(y_before, height - 1);
        dy = y / ry - y_before;

        for (int x = 0; x < resized_width; x++) {
            x_before = (int)floor(x / rx);
            x_before = fmin(x_before, width - 1);
            dx = x / rx - x_before;

            // compute bi-linear
            for (int c = 0; c < channel; c++) {
                val = (1.0 - dx) * (1.0 - dy) * img.at<cv::Vec3b>(y_before, x_before)[c] + dx * (1.0 - dy) * img.at<cv::Vec3b>(y_before, x_before + 1)[c] + (1.0 - dx) * dy * img.at<cv::Vec3b>(y_before + 1, x_before)[c] + dx * dy * img.at<cv::Vec3b>(y_before + 1, x_before)[c];

                // assign pixel to new position
                out.at<cv::Vec3b>(y, x)[c] = (uchar)val;
            }
        }
    }
    return out;
}

cv::Mat color_bilinear_buildin(cv::Mat img, double r1, double r2)
{
    int width = img.cols;
    int height = img.rows;

    cv::Mat out = cv::Mat::zeros(height, width, CV_8UC3);
    cv::resize(img, out, img.size(), r1, r2, cv::INTER_LINEAR);
    return out;
}

cv::Mat gray_bilinear_buildin(cv::Mat img, double r1, double r2)
{
    int width = img.cols;
    int height = img.rows;

    cv::Mat out = cv::Mat::zeros(height, width, CV_8UC1);
    cv::resize(img, out, img.size(), r1, r2, cv::INTER_LINEAR);

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
    for (int y = 0; y < height; y++) {
        y_before = (int)floor(
            y); // The floor() function takes a single argument and returns a value
        // of type double, float or long double type.
        y_before = fmin(y_before, height - 1);
        dy = y - y_before;

        for (int x = 0; x < width; x++) {
            x_before = (int)floor(x);
            x_before = fmin(x_before, width - 1);
            dx = x - x_before;

            val = (1.0 - dx) * (1.0 - dy) * img.at<uchar>(y_before, x_before) + dx * (1.0 - dy) * img.at<uchar>(y_before, x_before + 1) + (1.0 - dx) * dy * img.at<uchar>(y_before + 1, x_before) + dx * dy * img.at<uchar>(y_before + 1, x_before);

            out.at<uchar>(y, x) = (uchar)val;
        }
    }

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
    cv::Mat out = cv::Mat::zeros(
        height, width, CV_8UC1); // 0-255 one channel
    // inverse the black and write in this case

    // each y, x
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // BGR -> Gray
            // Formula to determine brightness of RGB color
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
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Binarize
            if (input.at<uchar>(y, x) > threshold) {
                out.at<uchar>(y, x) = 0;
            } else {
                out.at<uchar>(y, x) = 255;
            }
        }
    }

    return out;
}

vector<vector<pair<int, int>>> rect_contours(cv::Mat img, vector<vector<cv::Point>> contours)
{
    vector<cv::Rect> boundRect(contours.size());
    //  0    1   00  10
    //  2    3   01  11
    vector<vector<pair<int, int>>> coord(contours.size());

    for (int i = 0; i < contours.size(); i++) {
        boundRect[i] = boundingRect(contours[i]); //  次次都右至左  下至上
        // cv::Rect2i contourRect = cv::boundingRect(contours[i]);

        // cout<<"boundrect dim"<< contourRect.width <<endl;
        // rectangle(img, boundRect[i], cv::Scalar(255, 0, 255),0);

        // imshow("Rect", img);

        cout << "Rect " << i << endl;

        coord[i].push_back(make_pair(boundRect[i].x, boundRect[i].y));
        coord[i].push_back(
            make_pair(boundRect[i].x + boundRect[i].width, boundRect[i].y));
        coord[i].push_back(
            make_pair(boundRect[i].x, boundRect[i].y + boundRect[i].height));
        coord[i].push_back(make_pair(boundRect[i].x + boundRect[i].width,
            boundRect[i].y + boundRect[i].height));
        for (int j = 0; j <= 3; j++) { //    X                                 Y
            cout << "Point " << j << " :" << coord[i][j].first << " "
                 << coord[i][j].second << endl;
        }
        cout << endl;
    }
    return coord;
}

int find_num_obj_using_contours(cv::Mat img)
{
    vector<vector<cv::Point>> contours;

    findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    cv::Mat output = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);

    if (contours.size() == 0) {
        cout << "No objects detected" << endl;
        return 0;
    } else {
        cout << "Number of objects detected: " << contours.size() << endl;
    }

    rect_contours(img, contours);

    // cv::RNG rng(0xFFFFFFFF);
    // label the contours
    for (int i = 0; i < contours.size(); i++) {
        cv::drawContours(output, contours, i, 0x0000BBBB);
    }

    // cv::imshow("Contours Result", output);

    return contours.size();
}

cv::Mat cal_and_cut(cv::Mat img, cv::Mat mask, int sample_size)
{
    int height = img.rows;
    int width = img.cols;

    cv::Mat final = img.clone();
    cv::Mat result_rect;
    cv::Mat rect_roi;

    vector<vector<cv::Point>> contours;
    findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    vector<vector<pair<int, int>>> rect_coord = rect_contours(img, contours);

    int num_of_sample[contours.size()];
    int num_of_region = contours.size();

    vector<vector<vector<int>>> M_B;
    vector<vector<vector<float>>> M_A;

    float dx, dy;
    int y_0 = 0, y_1 = 0, x_0 = 0, x_1 = 0;
    int pixel_val;

    // cout<<"num of reg"<<num_of_region<<endl;
    for (int seg = 0; seg < num_of_region; seg++) // seg = region index
    {
        y_0 = rect_coord[seg][0].second;
        y_1 = rect_coord[seg][2].second;
        num_of_sample[seg] = 0;

        for (int y = y_0; y < y_1; y += sample_size) // size can the same
        {
            dy = ((y - y_0) / (y_1 - y_0 * 1.0));

            x_0 = rect_coord[seg][0].first;
            x_1 = rect_coord[seg][1].first;

            for (int x = x_0; x < x_1; x += sample_size) { // rect seg  point 0
                dx = ((x - x_0) / (x_1 - x_0 * 1.0));

                if (mask.at<uchar>(y, x) >= 0) {
                    pixel_val = img.at<uchar>(y, x);
                    // cout<<pixel_val<<endl;

                    M_B.push_back(vector<vector<int>>());
                    M_B.at(seg).push_back(vector<int>());
                    M_B.at(seg)[num_of_sample[seg]].push_back(pixel_val);
                    // cout<<M_B.at(seg)[num_of_sample[seg]][0]<<endl;

                    // M_B.push_back(M_B[seg]);

                    vector<float> temp_row;
                    M_A.push_back(vector<vector<float>>());
                    // M_A.at(seg).push_back(vector<float>());

                    temp_row.push_back((1.0 - dx) * dy); // 00
                    temp_row.push_back(dx * dy); // 10
                    temp_row.push_back((1.0 - dx) * (1.0 - dy)); // 01
                    temp_row.push_back(dx * (1.0 - dy)); // 11
                    M_A[seg].push_back(temp_row);

                    (num_of_sample[seg])++;
                }
            }
        }

        cv::Mat m_roi = img(cv::Rect(rect_coord[seg][0].first, rect_coord[seg][0].second,
            rect_coord[seg][1].first - rect_coord[seg][0].first,
            rect_coord[seg][2].second - rect_coord[seg][0].second));

        cv::Mat mask_roi = mask(cv::Rect(rect_coord[seg][0].first, rect_coord[seg][0].second,
            rect_coord[seg][1].first - rect_coord[seg][0].first,
            rect_coord[seg][2].second - rect_coord[seg][0].second));

        // cout << "num" << num_of_sample[seg] << endl;

        result_rect = single_planefit(m_roi, mask_roi, sample_size, M_B[seg],
            M_A[seg], num_of_sample[seg], rect_coord);
        rect_roi = final(cv::Rect(rect_coord[seg][0].first, rect_coord[seg][0].second,
            result_rect.cols, result_rect.rows));
        result_rect.copyTo(
            rect_roi, m_roi); // mask size need to same as rect roi so only m_roi
    }

    //   cv::imshow(cv::format("Result using sample size : %d ", sample_size),
    //   final); cv::waitKey(0);

    //   for (vector<vector<vector<float>>>::const_iterator i = M_A.begin();
    //        i != M_A.end(); ++i) {
    //     for (vector<vector<float>>::const_iterator j = i->begin(); j !=
    //     i->end();
    //          ++j) {
    //       for (vector<float>::const_iterator k = j->begin(); k != j->end();
    //       ++k) {
    //         cout << *k << ' ' << endl;
    //       }
    //     }
    //   }

    return final;
}

cv::Mat single_planefit(cv::Mat contour_region, cv::Mat mask_region, int sample_size, vector<vector<int>> M_B, vector<vector<float>> M_A, int num_of_sample, vector<vector<pair<int, int>>> rect_coord)
{
    cv::imshow("ROI", contour_region);
    // cv::imshow("MASKROI", mask_region);
    cv::Mat result = cv::Mat::zeros(4, 1, CV_32FC1);
    cv::Mat matrix_a = cv::Mat::zeros(num_of_sample, 4, CV_32FC1);
    cv::Mat matrix_b = cv::Mat::zeros(num_of_sample, 1, CV_32FC1);

    //圖入黎,seg唔seg ,／ 直斬，每一份掉入去一個reg ,掉function cut格

    for (int sample = 0; sample < num_of_sample; sample++) {
        matrix_b.at<float>(sample, 0) = M_B[sample][0];
        // cout<<M_B[sample][0]<<endl;
    }

    for (int current_row = 0; current_row < num_of_sample; current_row++) {
        for (int i = 0; i < 4; i++) {
            matrix_a.at<float>(current_row, i) = M_A[current_row][i];
        }
    }

    cv::solve(matrix_a, matrix_b, result, cv::DECOMP_SVD);

    cp_struct cp[4];
    cp[0].x_coord = 0;
    cp[0].y_coord = 0;
    cp[1].x_coord = 1;
    cp[1].y_coord = 0;
    cp[2].x_coord = 0;
    cp[2].y_coord = 1;
    cp[3].x_coord = 1;
    cp[3].y_coord = 1;

    cout << "result" << endl;
    for (int n = 0; n <= 3; n++) {
        // cout << result.at<float>(n, 0) << endl;
        cp[n].z_value = result.at<float>(n, 0); // float to int
        cout << "x : " << cp[n].x_coord << " y : " << cp[n].y_coord << endl;
        cout << cp[n].z_value << endl;
    }
    cout << endl;

    cv::Mat after_fit = cv::Mat::zeros(contour_region.size(), CV_8UC1);
    int pixel_val = 0;

    int height = contour_region.rows;
    int width = contour_region.cols;
    float dx, dy;
    int y_0 = 0, y_1 = 0, x_0 = 0, x_1 = 0;

    for (int y = 0; y < height; y++) {
        dy = ((y - y_0) / (height - y_0 * 1.0));
        for (int x = 0; x < width; x++) {
            dx = ((x - x_0) / (width - x_0 * 1.0));

            if (mask_region.at<uchar>(y, x) != 0) {
                pixel_val = ((1.0 - dx) * dy) * cp[0].z_value + (dx * dy) * cp[1].z_value + ((1.0 - dx) * (1.0 - dy)) * cp[2].z_value + (dx * (1.0 - dy)) * cp[3].z_value;

                after_fit.at<uchar>(y, x) = pixel_val;
            }
        }
    }
    cv::imshow("after fit", after_fit);
    // cv::waitKey(0);

    // cv::imshow("result", result);

    return after_fit;
}

cv::Mat segmentation(cv::Mat img, cv::Mat mask, int* Grid_size_x, int* Grid_size_y, int sample_size)
{
    vector<vector<cv::Point>> contours;
    findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    vector<vector<pair<int, int>>> rect_coord = rect_contours(img, contours);

    int num_of_region = contours.size();
    cout << "number of region" << num_of_region << endl;

    int region_number_y[num_of_region];
    int region_number_x[num_of_region];

    cv::Rect2i contourRect;

    for (int seg = 0; seg < num_of_region; seg++) // how many region
    {
        contourRect = cv::boundingRect(contours[seg]);

        // cout << "boundrect dim: W: " << contourRect.width << "  H: " <<
        // contourRect.height << endl;
        region_number_y[seg] = (contourRect.height / sample_size);
        region_number_x[seg] = (contourRect.width / sample_size);
        // cout << "contour  num x " << region_number_x[seg] << " num y " <<
        // region_number_y[seg] << endl;
    }

    // int num_of_sample[num_of_region][region_number_x][region_number_y];
    // int region[num_of_region][region_number_x][region_number_y];

    int y_0 = 0, y_1 = 0, x_0 = 0, x_1 = 0;
    int width = 0, height = 0;

    vector<cv::Rect> img_Cells;
    vector<cv::Rect> mask_Cells;

    cv::Mat rect_roi;
    cv::Mat tempimg;
    int x = 0;
    int last_x = 0;

    for (int seg = 0; seg < num_of_region; seg++) // seg = region index
    {
        y_0 = rect_coord[seg][0].second;
        y_1 = rect_coord[seg][2].second;
        // num_of_sample[seg] = 0;
        *Grid_size_y = (rect_coord[seg][2].second - rect_coord[seg][0].second) / (region_number_y[seg]);
        for (int y = y_0; y <= y_1 - *Grid_size_y;
             y += *Grid_size_y) // size can the same
        {
            x_0 = rect_coord[seg][0].first;
            x_1 = rect_coord[seg][1].first;

            width = 0;
            // cout << "reg num: " << region_number_x[seg] << endl;
            *Grid_size_x = (rect_coord[seg][1].first - rect_coord[seg][0].first) / (region_number_x[seg]);

            for (x = x_0; x < x_1 - *Grid_size_x; x += *Grid_size_x) {
                // cout << "hellllllll    " << x << endl;
                cv::Rect grid_rect(x, y, *Grid_size_x, *Grid_size_y);
                // cout << "heeeeeelo" << x << x+*Grid_size_x<<endl;
                img_Cells.push_back(grid_rect);

                // cv::rectangle(img, grid_rect, cv::Scalar(0, 255, 0), 0);
                // cv::imshow("img", img);
                // cv::imshow(cv::format("IMG_Grid: seg:%d  %d%d", seg, width, height),
                // img(grid_rect));

                // cv::waitKey(0);
                mask_Cells.push_back(grid_rect);
                // cv::rectangle(mask, grid_rect, cv::Scalar(0, 255, 0), 0);
                // cv::imshow("mask", mask);
                // cv::imshow(cv::format("MASK_Grid: seg:%d  %d%d", seg, width, height),
                // mask(grid_rect)); cv::waitKey(0);
                tempimg = cal_and_cut(img(grid_rect), mask(grid_rect), 50);
                rect_roi = img(cv::Rect(x, y, tempimg.cols, tempimg.rows));
                tempimg.copyTo(rect_roi, mask(grid_rect));
                // cv::imshow("tempimg",tempimg);
                // cv::waitKey(0);

                width++;
                last_x = x;
            }

            {
                cv::Rect grid_rect(last_x + (*Grid_size_x), y,
                    x_1 - (last_x + (*Grid_size_x)), (*Grid_size_y));
                img_Cells.push_back(grid_rect);
                // cv::rectangle(img, grid_rect, cv::Scalar(0, 255, 0), 0);
                // cv::imshow("img", img);
                // cv::imshow(cv::format("IMG_Grid: seg:%d  %d%d", seg, width, height),
                // img(grid_rect)); cv::waitKey(0);
                mask_Cells.push_back(grid_rect);
                // cv::rectangle(mask, grid_rect, cv::Scalar(0, 255, 0), 0);
                // cv::imshow("mask", mask);
                // cv::imshow(cv::format("MASK_Grid: seg:%d  %d%d", seg, width, height),
                // mask(grid_rect)); cv::waitKey(0);
                {
                    tempimg = cal_and_cut(img(grid_rect), mask(grid_rect), 10);
                    rect_roi = img(cv::Rect(x, y, tempimg.cols, tempimg.rows));
                    tempimg.copyTo(rect_roi, mask(grid_rect));
                }
                // cv::waitKey(0);

                width++;
            }

            height++;
        }
        height = 0;
    }

    //   cv::Mat input_roi = input(cv::Rect(x_pos, y_pos, w, h));  // e.g function
    //   contour to cv rect
    // depend on w , h  , not     area

    //       cv::Rect(x_pos, y_pos, w, h)

    //   cv::imwrite("ROI.bmp", input_roi);

    return img;
}

cv::Mat gen2_segmentation(cv::Mat img, cv::Mat mask, int* Grid_size_x, int* Grid_size_y, int dimension, int* region_num)
{
    vector<vector<cv::Point>> contours;
    findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    vector<vector<pair<int, int>>> rect_coord = rect_contours(img, contours);

    *region_num = contours.size();
    int num_of_region = contours.size();
    cout << "number of region:    " << num_of_region << endl;
    int region_number_y[num_of_region];
    int region_number_x[num_of_region]; // region number

    cv::Rect2i contourRect;

    for (int seg = 0; seg < num_of_region; seg++) // how many region
    {
        contourRect = cv::boundingRect(contours[seg]);

        // cout << "boundrect dim: W: " << contourRect.width << "  H: " <<
        // contourRect.height << endl;
        region_number_y[seg] = 1;
        region_number_x[seg] = dimension;
    }
    cout << "----------------------------------------------------------" << endl;

    int y_0 = 0, y_1 = 0, x_0 = 0, x_1 = 0;
    int width = 0, height = 0;

    vector<cv::Rect> img_Cells;
    vector<cv::Rect> mask_Cells;

    cv::Mat rect_roi;
    cv::Mat tempimg;
    int x = 0;
    int last_x = 0;

    for (int seg = 0; seg < num_of_region; seg++) // seg = region index
    {
        y_0 = rect_coord[seg][0].second;
        y_1 = rect_coord[seg][2].second;

        *Grid_size_y = (rect_coord[seg][2].second - rect_coord[seg][0].second) / (region_number_y[seg]);

        for (int y = y_0; y <= y_1 - *Grid_size_y;
             y += *Grid_size_y) // 無左等號會少左下面個part
        {
            x_0 = rect_coord[seg][0].first;
            x_1 = rect_coord[seg][1].first;

            width = 0;
            // cout << "reg num: " << region_number_x[seg] << endl;
            *Grid_size_x = (rect_coord[seg][1].first - rect_coord[seg][0].first) / (region_number_x[seg]);

            for (x = x_0; x <= x_1 - *Grid_size_x;
                 x += *Grid_size_x) // 無左等號會少左右面
            {
                cv::Rect grid_rect(x, y, *Grid_size_x, *Grid_size_y);

                img_Cells.push_back(grid_rect);

                // cv::rectangle(img, grid_rect, cv::Scalar(0, 255, 0), 0);
                // cv::imshow("img", img);
                // cv::imshow(cv::format("IMG_Grid: seg:%d  %d%d", seg, width, height),
                // img(grid_rect)); cv::waitKey(0);

                mask_Cells.push_back(grid_rect);

                // cv::rectangle(mask, grid_rect, cv::Scalar(0, 255, 0), 0);
                // cv::imshow("mask", mask);
                // cv::imshow(cv::format("MASK_Grid: seg:%d  %d%d", seg, width, height),
                // mask(grid_rect)); cv::waitKey(0); get_num_sample(img(grid_rect),
                // mask(grid_rect), 5);

                tempimg = cal_and_cut(img(grid_rect), mask(grid_rect), 5);
                rect_roi = img(cv::Rect(x, y, tempimg.cols, tempimg.rows));
                tempimg.copyTo(rect_roi, mask(grid_rect));
                // cv::imshow("tempimg", tempimg);
                // cv::waitKey(0);

                width++;
                last_x = x;
            }

            {
                cv::Rect grid_rect(last_x + (*Grid_size_x), y,
                    x_1 - (last_x + (*Grid_size_x)), (*Grid_size_y));

                img_Cells.push_back(grid_rect);
                // cv::rectangle(img, grid_rect, cv::Scalar(0, 255, 0), 0);
                // cv::imshow("img", img);
                // cv::imshow(cv::format("IMG_Grid: seg:%d  %d%d", seg, width,
                // height), img(grid_rect));
                // cv::waitKey(0);

                mask_Cells.push_back(grid_rect);
                // cv::rectangle(mask, grid_rect, cv::Scalar(0, 255, 0), 0);
                // cv::imshow("mask", mask);
                // cv::imshow(cv::format("MASK_Grid: seg:%d  %d%d", seg, width,
                // height), mask(grid_rect));
                // cv::waitKey(0);
                {
                    tempimg = cal_and_cut(img(grid_rect), mask(grid_rect), 10);
                    rect_roi = img(cv::Rect(x, y, tempimg.cols, tempimg.rows));
                    tempimg.copyTo(rect_roi, mask(grid_rect));
                }
                width++;
            }

            height++;
        }
        height = 0;
    }

    return img;
}

cv::Mat planefit(cv::Mat img, cv::Mat mask)
{
    int height = img.rows;
    int width = img.cols;

    vector<vector<cv::Point>> contours;
    findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    vector<vector<pair<int, int>>> rect_coord = rect_contours(img, contours);
    int num_of_sample[contours.size()]; // seg幾 左定右region

    cv::Mat m_roi;
    cv::Mat mask_roi;

    for (int seg = 0; seg < contours.size(); seg++) {
        m_roi = img(cv::Rect(rect_coord[seg][0].first, rect_coord[seg][0].second,
            rect_coord[seg][1].first - rect_coord[seg][0].first,
            rect_coord[seg][2].second - rect_coord[seg][0].second));

        mask_roi = mask(cv::Rect(rect_coord[seg][0].first, rect_coord[seg][0].second,
            rect_coord[seg][1].first - rect_coord[seg][0].first,
            rect_coord[seg][2].second - rect_coord[seg][0].second));

        cout << "seg: " << seg << endl;
        num_of_sample[seg] = get_num_sample(m_roi, mask_roi, 5);
        cout << "num_of_sample:  " << num_of_sample[seg] << endl;
        LR_planefit(m_roi, mask_roi, 5, num_of_sample[seg]);
        // cv::imshow("final", m_roi);
        // cv::waitKey(0);
    }
    return img;
}

cv::Mat LR_planefit(cv::Mat img, cv::Mat mask_img, int sample_size, int num_of_sample)
{
    cv::Mat final = cv::Mat::zeros(img.size(), CV_8UC1);
    cv::Mat result = cv::Mat::zeros(6, 1, CV_32FC1);
    cv::Mat matrix_a = cv::Mat::zeros(num_of_sample, 6, CV_32FC1);
    cv::Mat matrix_b = cv::Mat::zeros(num_of_sample, 1, CV_32FC1);
    // int num_of_sample = 0;

    int height = img.rows;
    int width = img.cols;
    int count = 0;

    float dx, dy, temp;
    int y_0 = 0, y_1 = 0, x_0 = 0, x_1 = 0;

    // for (int y = 0; y < height; y += sample_size) {
    //   for (int x = 0; x < width; x += sample_size) {
    //     if (mask_img.at<uchar>(y, x) > 0) {
    //       matrix_b.at<float>(count, 0) = img.at<uchar>(y, x);  // y
    //       行 第x個 count++;
    //       // cout<<matrix_b.at<float>(count, 0)<<endl;
    //     }
    //   }
    // }

    count = 0;
    for (int current_row = 0; current_row < height; current_row += sample_size) {
        dy = (height - 0);
        for (int current_col = 0; current_col < width; current_col += sample_size) {
            // cout << "dx:  " << dx << endl;
            // cout << "dy:  " << dy << endl;
            // cout << "------------------" << endl;

            if (mask_img.at<uchar>(current_row, current_col) > 0) {
                dx = current_col / 2;
                // temp = 1 / (dx * dy);
                temp = (height * (width / 2.0));

                if (current_col <= (width / 2)) { // 0134

                    matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                    matrix_a.at<float>(count, 0) = (((width / 2) - current_col) * (height - current_row)) / temp; // (x2-x)(y-y1)
                    matrix_a.at<float>(count, 1) = ((current_col - 0) * (height - current_row)) / temp; //(x-x1)(y-y1)
                    matrix_a.at<float>(count, 3) = ((width / 2 - current_col) * (current_row - 0)) / temp; // (x2-x)(y2-y)
                    matrix_a.at<float>(count, 4) = ((current_col - 0) * (current_row - 0)) / temp; //(x-x1)(y2-y)

                    cout << "val :   " << ((width / 2) - current_col) * (height - current_row) / temp << endl; // (x2-x)(y-y1)
                    cout << "val :   " << (current_col - 0) * (height - current_row) / temp << endl; //(x-x1)(y-y1)
                    cout << "val :   " << (width / 2 - current_col) * (current_row - 0) / temp << endl; // (x2-x)(y2-y)
                    cout << "val :   " << (current_col - 0) * (current_row - 0) / temp << endl; //(x-x1)(y2-y)
                    cout << ">>>>>>>>" << endl;
                    count++;

                } else if (current_col > (width / 2)) {
                    matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                    matrix_a.at<float>(count, 1) = ((width - current_col) * (height - current_row)) / temp; // (x2-x)(y-y1)
                    matrix_a.at<float>(count, 2) = ((current_col - width / 2) * (height - current_row)) / temp; //(x-x1)(y-y1)
                    matrix_a.at<float>(count, 4) = ((width - current_col) * (current_row - 0)) / temp; // (x2-x)(y2-y)
                    matrix_a.at<float>(count, 5) = ((current_col - width / 2) * (current_row - 0)) / temp; //(x-x1)(y2-y)
                    // cout<<"val2: "<< ((width - current_col) * (height - current_row))
                    // / temp <<endl;		// (x2-x)(y-y1) cout<<"val2: "<<
                    // ((current_col - width / 2) * (height - current_row)) / temp
                    // <<endl;
                    // //(x-x1)(y-y1) cout<<"val2: "<< ((width - current_col) *
                    // (current_row - 0)) / temp <<endl;			//
                    // (x2-x)(y2-y) cout<<"val2: "<< ((current_col - width / 2) *
                    // (current_row - 0))/ temp <<endl;		//(x-x1)(y2-y)
                    // cout<<">>>>>>>>>>>"<<endl;
                    count++;
                }
            }
        }
    }
    cv::Mat dst;
    cv::Mat dst2;
    cv::Mat test = cv::Mat::zeros(2, 2, CV_8UC1);
    cv::Mat test2 = cv::Mat::zeros(2, 2, CV_8UC1);

    cv::solve(matrix_a, matrix_b, result, cv::DECOMP_NORMAL);

    cout << "result matrix:    " << result.at<float>(0, 0) << endl;
    cout << "result matrix:    " << result.at<float>(1, 0) << endl;
    cout << "result matrix:    " << result.at<float>(3, 0) << endl;
    cout << "result matrix:    " << result.at<float>(4, 0) << endl;
    cout << "<<<<<<" << endl;

    cout << "result matrix:    " << result.at<float>(0, 0) << endl;
    cout << "result matrix:    " << result.at<float>(0, 1) << endl;
    cout << "result matrix:    " << result.at<float>(0, 3) << endl;
    cout << "result matrix:    " << result.at<float>(0, 4) << endl;
    // cout<<"<<<<<<"<<endl;
    // cout << "result matrix:    " << result.at<float>(1, 0) << endl;
    // cout << "result matrix:    " << result.at<float>(2, 0) << endl;
    // cout << "result matrix:    " << result.at<float>(4, 0) << endl;
    // cout << "result matrix:    " << result.at<float>(5, 0) << endl;

    test.at<uchar>(0, 0) = result.at<float>(0, 0);
    test.at<uchar>(0, 1) = result.at<float>(1, 0);
    test.at<uchar>(1, 0) = result.at<float>(3, 0);
    test.at<uchar>(1, 1) = result.at<float>(4, 0);

    test2.at<uchar>(0, 0) = result.at<float>(1, 0);
    test2.at<uchar>(0, 1) = result.at<float>(2, 0);
    test2.at<uchar>(1, 0) = result.at<float>(4, 0);
    test2.at<uchar>(1, 1) = result.at<float>(5, 0);

    cv::imshow("test", test);
    cv::imshow("test2", test2);

    cv::Size dsize = cv::Size(width / 2, height);

    cv::resize(test, dst, dsize, 0, 0, cv::INTER_LINEAR);
    cv::resize(test2, dst2, dsize, 0, 0, cv::INTER_LINEAR);
    for (int i = 0; i < dst.rows; i++) {
        cout << "L:  " << (int)dst.at<uchar>(0, dst.cols - 1)
             << " R:   " << (int)dst2.at<uchar>(0, 0) << endl;
        cout << "L:  " << (int)dst.at<uchar>(dst.rows, dst.cols - 1)
             << " R:   " << (int)dst2.at<uchar>(dst2.rows, 0) << endl;
    }

    cout << "test: " << (int)test.at<uchar>(0, 1)
         << " test2:  " << (int)test2.at<uchar>(0, 0) << endl;

    cout << "test: " << (int)test.at<uchar>(1, 1)
         << " test2:  " << (int)test2.at<uchar>(1, 0) << endl;

    dst.copyTo(final(cv::Rect(0, 0, dst.cols, dst.rows)));
    dst2.copyTo(final(cv::Rect(dst.cols, 0, dst2.cols, dst.rows)));

    final.copyTo(img, mask_img);

    // cv::imshow("111111", dst);
    // cv::imshow("22222", dst2);
    // cv::imshow("333333", final);
    // cv::imshow("mask", mask_img);
    // cv::imshow("img", img);

    // cv::waitKey(0);

    return img;
}

int get_num_sample(cv::Mat img, cv::Mat mask, int sample_size)
{
    int height = img.rows;
    int width = img.cols;

    int num_of_sample = 0;
    // int num_of_region = 2;

    for (int y = 0; y < height; y += sample_size) {
        for (int x = 0; x < width; x += sample_size) {
            if (mask.at<uchar>(y, x) >= 0) {
                num_of_sample++;
            }
        }
    }
    return num_of_sample;
}

cv::Mat gen2_planefit(cv::Mat img, cv::Mat mask)
{
    int height = img.rows;
    int width = img.cols;

    vector<vector<cv::Point>> contours;
    findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    vector<vector<pair<int, int>>> rect_coord = rect_contours(img, contours);
    int num_of_sample[contours.size()]; // seg幾 左定右region

    cv::Mat m_roi;
    cv::Mat mask_roi;

    for (int seg = 0; seg < contours.size(); seg++) {
        m_roi = img(cv::Rect(rect_coord[seg][0].first, rect_coord[seg][0].second,
            rect_coord[seg][1].first - rect_coord[seg][0].first,
            rect_coord[seg][2].second - rect_coord[seg][0].second));

        mask_roi = mask(cv::Rect(rect_coord[seg][0].first, rect_coord[seg][0].second,
            rect_coord[seg][1].first - rect_coord[seg][0].first,
            rect_coord[seg][2].second - rect_coord[seg][0].second));

        cout << "seg: " << seg << endl;
        num_of_sample[seg] = get_num_sample(m_roi, mask_roi, 4);
        cout << "num_of_sample:  " << num_of_sample[seg] << endl;
        general_planefit(m_roi, mask_roi, 4, num_of_sample[seg], 2, 2);
        //  TDLR_planefit(m_roi, mask_roi, 5, num_of_sample[seg]);

        // cv::imshow("final", m_roi);
        // cv::waitKey(0);
    }
    return img;
}

cv::Mat TDLR_planefit(cv::Mat img, cv::Mat mask_img, int sample_size, int num_of_sample)
{
    cv::Mat final = cv::Mat::zeros(img.size(), CV_8UC1);
    cv::Mat result = cv::Mat::zeros(9, 1, CV_32FC1);
    cv::Mat matrix_a = cv::Mat::zeros(num_of_sample, 9, CV_32FC1);
    cv::Mat matrix_b = cv::Mat::zeros(num_of_sample, 1, CV_32FC1);

    int height = img.rows;
    int width = img.cols;
    int count = 0;

    float dx, dy, temp;
    int y_0 = 0, y_1 = 0, x_0 = 0, x_1 = 0;

    //012
    //345
    //678

    count = 0;
    for (int current_row = 0; current_row < height; current_row += sample_size) {
        for (int current_col = 0; current_col < width; current_col += sample_size) {

            if (mask_img.at<uchar>(current_row, current_col) > 0) {

                temp = (height / 2 * (width / 2.0));

                if (current_col <= (width / 2)) { // 左
                    if (current_row < (height / 2)) {
                        //上
                        matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                        matrix_a.at<float>(count, 0) = (((width / 2) - current_col) * (height / 2 - current_row)) / temp; // (x2-x)(y-y1)
                        matrix_a.at<float>(count, 1) = ((current_col - 0) * (height / 2 - current_row)) / temp; //(x-x1)(y-y1)
                        matrix_a.at<float>(count, 3) = ((width / 2 - current_col) * (current_row - 0)) / temp; // (x2-x)(y2-y)
                        matrix_a.at<float>(count, 4) = ((current_col - 0) * (current_row - 0)) / temp; //(x-x1)(y2-y)
                    } else if (current_row >= (height / 2)) {
                        matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                        matrix_a.at<float>(count, 3) = (((width / 2) - current_col) * (height - current_row)) / temp; // (x2-x)(y-y1)
                        matrix_a.at<float>(count, 4) = ((current_col - 0) * (height - current_row)) / temp; //(x-x1)(y-y1)
                        matrix_a.at<float>(count, 6) = ((width / 2 - current_col) * (current_row - height / 2)) / temp; // (x2-x)(y2-y)
                        matrix_a.at<float>(count, 7) = ((current_col - 0) * (current_row - height / 2)) / temp; //(x-x1)(y2-y)
                    }

                    count++;

                } else if (current_col > (width / 2)) {
                    if (current_row < (height / 2)) {

                        matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                        matrix_a.at<float>(count, 1) = ((width - current_col) * (height / 2 - current_row)) / temp; // (x2-x)(y-y1)
                        matrix_a.at<float>(count, 2) = ((current_col - width / 2) * (height / 2 - current_row)) / temp; //(x-x1)(y-y1)
                        matrix_a.at<float>(count, 4) = ((width - current_col) * (current_row - 0)) / temp; // (x2-x)(y2-y)
                        matrix_a.at<float>(count, 5) = ((current_col - width / 2) * (current_row - 0)) / temp; //(x-x1)(y2-y)

                    } else if (current_row >= (height / 2)) {

                        matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                        matrix_a.at<float>(count, 4) = ((width - current_col) * (height - current_row)) / temp; // (x2-x)(y-y1)
                        matrix_a.at<float>(count, 5) = ((current_col - width / 2) * (height - current_row)) / temp; //(x-x1)(y-y1)
                        matrix_a.at<float>(count, 7) = ((width - current_col) * (current_row - height / 2)) / temp; // (x2-x)(y2-y)
                        matrix_a.at<float>(count, 8) = ((current_col - width / 2) * (current_row - height / 2)) / temp; //(x-x1)(y2-y)
                    }

                    count++;
                }
            }
        }
    }
    cv::Mat dst;
    cv::Mat dst2;
    cv::Mat dst3;
    cv::Mat dst4;

    cv::Mat test = cv::Mat::zeros(2, 2, CV_8UC1);
    cv::Mat test2 = cv::Mat::zeros(2, 2, CV_8UC1);
    cv::Mat test3 = cv::Mat::zeros(2, 2, CV_8UC1);
    cv::Mat test4 = cv::Mat::zeros(2, 2, CV_8UC1);

    cv::solve(matrix_a, matrix_b, result, cv::DECOMP_SVD);
    cout << "hi" << endl;
    for (int i = 0; i < 9; i++) {
        cout << "result matrix:    " << result.at<float>(i, 0) << endl;
    }
    cout << "hi" << endl;

    // 012
    // 345
    // 678

    test.at<uchar>(0, 0) = result.at<float>(0, 0);
    test.at<uchar>(0, 1) = result.at<float>(1, 0);
    test.at<uchar>(1, 0) = result.at<float>(3, 0);
    test.at<uchar>(1, 1) = result.at<float>(4, 0);

    test2.at<uchar>(0, 0) = result.at<float>(1, 0);
    test2.at<uchar>(0, 1) = result.at<float>(2, 0);
    test2.at<uchar>(1, 0) = result.at<float>(4, 0);
    test2.at<uchar>(1, 1) = result.at<float>(5, 0);

    test3.at<uchar>(0, 0) = result.at<float>(3, 0);
    test3.at<uchar>(0, 1) = result.at<float>(4, 0);
    test3.at<uchar>(1, 0) = result.at<float>(6, 0);
    test3.at<uchar>(1, 1) = result.at<float>(7, 0);

    test4.at<uchar>(0, 0) = result.at<float>(4, 0);
    test4.at<uchar>(0, 1) = result.at<float>(5, 0);
    test4.at<uchar>(1, 0) = result.at<float>(7, 0);
    test4.at<uchar>(1, 1) = result.at<float>(8, 0);

    cv::imshow("test", test);
    cv::imshow("test2", test2);

    cv::Size dsize = cv::Size(width / 2, height / 2);

    cv::resize(test, dst, dsize, 0, 0, cv::INTER_LINEAR);
    cv::resize(test2, dst2, dsize, 0, 0, cv::INTER_LINEAR);
    cv::resize(test3, dst3, dsize, 0, 0, cv::INTER_LINEAR);
    cv::resize(test4, dst4, dsize, 0, 0, cv::INTER_LINEAR);

    dst.copyTo(final(cv::Rect(0, 0, dst.cols, dst.rows)));
    dst2.copyTo(final(cv::Rect(dst.cols, 0, dst2.cols, dst2.rows)));

    dst3.copyTo(final(cv::Rect(0, dst.rows, dst3.cols, dst3.rows)));
    dst4.copyTo(final(cv::Rect(dst.cols, dst.rows, dst4.cols, dst4.rows)));

    final.copyTo(img, mask_img);

    return img;
}

cv::Mat TMDLMR_planefit(cv::Mat img, cv::Mat mask_img, int sample_size, int num_of_sample)
{
    cv::Mat final = cv::Mat::zeros(img.size(), CV_8UC1);
    cv::Mat result = cv::Mat::zeros(16, 1, CV_32FC1);
    cv::Mat matrix_a = cv::Mat::zeros(num_of_sample, 16, CV_32FC1);
    cv::Mat matrix_b = cv::Mat::zeros(num_of_sample, 1, CV_32FC1);

    int height = img.rows;
    int width = img.cols;
    int count = 0;

    float temp = 0;
    //0123
    //4567
    //891011
    //12131415

    count = 0;
    for (int current_row = 0; current_row < height; current_row += sample_size) {
        for (int current_col = 0; current_col < width; current_col += sample_size) {

            if (mask_img.at<uchar>(current_row, current_col) > 0) {

                temp = ((height / 3) * (width / 3.0));

                if (current_col <= (width / 3)) { // 左

                    if (current_row < (height / 3)) {
                        //上
                        matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個
                        cout << (int)img.at<uchar>(current_row, current_col) << endl;

                        matrix_a.at<float>(count, 0) = (((width / 3) - current_col) * (height / 3 - current_row)) / temp; // (x2-x)(y-y1)
                        matrix_a.at<float>(count, 1) = ((current_col - 0) * (height / 3 - current_row)) / temp; //(x-x1)(y-y1)
                        matrix_a.at<float>(count, 4) = ((width / 3 - current_col) * (current_row - 0)) / temp; // (x2-x)(y2-y)
                        matrix_a.at<float>(count, 5) = ((current_col - 0) * (current_row - 0)) / temp; //(x-x1)(y2-y)

                    } else if ((current_row >= (height / 3)) && (current_row <= (height / 3 * 2))) {
                        matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                        matrix_a.at<float>(count, 4) = (((width / 3) - current_col) * (height / 3 * 2 - current_row)) / temp; // (x2-x)(y-y1)
                        matrix_a.at<float>(count, 5) = ((current_col - 0) * (height / 3 * 2 - current_row)) / temp; //(x-x1)(y-y1)
                        matrix_a.at<float>(count, 8) = ((width / 3 - current_col) * (current_row - height / 3)) / temp; // (x2-x)(y2-y)
                        matrix_a.at<float>(count, 9) = ((current_col - 0) * (current_row - height / 3)) / temp; //(x-x1)(y2-y)

                    } else if (current_row > (height / 3 * 2)) {
                        matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                        matrix_a.at<float>(count, 8) = (((width / 3) - current_col) * (height - current_row)) / temp; // (x2-x)(y-y1)
                        matrix_a.at<float>(count, 9) = ((current_col - 0) * (height - current_row)) / temp; //(x-x1)(y-y1)
                        matrix_a.at<float>(count, 12) = ((width / 3 - current_col) * (current_row - height / 3 * 2)) / temp; // (x2-x)(y2-y)
                        matrix_a.at<float>(count, 13) = ((current_col - 0) * (current_row - height / 3 * 2)) / temp; //(x-x1)(y2-y)
                    }

                    count++;

                } else if (current_col > (width / 3) && current_col < (width / 3 * 2)) {

                    if (current_row < (height / 3)) {

                        matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                        matrix_a.at<float>(count, 1) = ((width / 3 * 2 - current_col) * (height / 3 - current_row)) / temp; // (x2-x)(y-y1)
                        matrix_a.at<float>(count, 2) = ((current_col - width / 3) * (height / 3 - current_row)) / temp; //(x-x1)(y-y1)
                        matrix_a.at<float>(count, 5) = ((width / 3 * 2 - current_col) * (current_row - 0)) / temp; // (x2-x)(y2-y)
                        matrix_a.at<float>(count, 6) = ((current_col - width / 3) * (current_row - 0)) / temp; //(x-x1)(y2-y)

                    } else if ((current_row >= (height / 3)) && (current_row <= (height / 3 * 2))) {

                        matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                        matrix_a.at<float>(count, 5) = ((width / 3 * 2 - current_col) * (height / 3 * 2 - current_row)) / temp; // (x2-x)(y-y1)
                        matrix_a.at<float>(count, 6) = ((current_col - width / 3) * (height / 3 * 2 - current_row)) / temp; //(x-x1)(y-y1)
                        matrix_a.at<float>(count, 9) = ((width / 3 * 2 - current_col) * (current_row - height / 3)) / temp; // (x2-x)(y2-y)
                        matrix_a.at<float>(count, 10) = ((current_col - width / 3) * (current_row - height / 3)) / temp; //(x-x1)(y2-y)

                    } else if (current_row > (height / 3 * 2)) {

                        matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                        matrix_a.at<float>(count, 9) = ((width / 3 * 2 - current_col) * (height - current_row)) / temp; // (x2-x)(y-y1)
                        matrix_a.at<float>(count, 10) = ((current_col - width / 3) * (height - current_row)) / temp; //(x-x1)(y-y1)
                        matrix_a.at<float>(count, 13) = ((width / 3 * 2 - current_col) * (current_row - height / 3 * 2)) / temp; // (x2-x)(y2-y)
                        matrix_a.at<float>(count, 14) = ((current_col - width / 3) * (current_row - height / 3 * 2)) / temp; //(x-x1)(y2-y)
                    }

                    count++;

                } else if (current_col > (width / 3 * 2)) {
                    if (current_row < (height / 3)) {

                        matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                        matrix_a.at<float>(count, 2) = ((width - current_col) * (height / 3 - current_row)) / temp; // (x2-x)(y-y1)
                        matrix_a.at<float>(count, 3) = ((current_col - width / 3 * 2) * (height / 3 - current_row)) / temp; //(x-x1)(y-y1)
                        matrix_a.at<float>(count, 6) = ((width - current_col) * (current_row - 0)) / temp; // (x2-x)(y2-y)
                        matrix_a.at<float>(count, 7) = ((current_col - width / 3 * 2) * (current_row - 0)) / temp; //(x-x1)(y2-y)

                    } else if ((current_row >= (height / 3)) && (current_row <= (height / 3 * 2))) {

                        matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                        matrix_a.at<float>(count, 6) = ((width - current_col) * (height / 3 * 2 - current_row)) / temp; // (x2-x)(y-y1)
                        matrix_a.at<float>(count, 7) = ((current_col - width / 3 * 2) * (height / 3 * 2 - current_row)) / temp; //(x-x1)(y-y1)
                        matrix_a.at<float>(count, 10) = ((width - current_col) * (current_row - height / 3)) / temp; // (x2-x)(y2-y)
                        matrix_a.at<float>(count, 11) = ((current_col - width / 3 * 2) * (current_row - height / 3)) / temp; //(x-x1)(y2-y)

                    } else if (current_row > (height / 3 * 2)) {

                        matrix_b.at<float>(count, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                        matrix_a.at<float>(count, 10) = ((width - current_col) * (height - current_row)) / temp; // (x2-x)(y-y1)
                        matrix_a.at<float>(count, 11) = ((current_col - width / 3 * 2) * (height - current_row)) / temp; //(x-x1)(y-y1)
                        matrix_a.at<float>(count, 14) = ((width - current_col) * (current_row - height / 3 * 2)) / temp; // (x2-x)(y2-y)
                        matrix_a.at<float>(count, 15) = ((current_col - width / 3 * 2) * (current_row - height / 3 * 2)) / temp; //(x-x1)(y2-y)
                    }

                    count++;
                }
            }
        }
    }
    cv::Mat dst1;
    cv::Mat dst2;
    cv::Mat dst3;
    cv::Mat dst4;
    cv::Mat dst5;
    cv::Mat dst6;
    cv::Mat dst7;
    cv::Mat dst8;
    cv::Mat dst9;

    cv::Mat test1 = cv::Mat::zeros(2, 2, CV_8UC1);
    cv::Mat test2 = cv::Mat::zeros(2, 2, CV_8UC1);
    cv::Mat test3 = cv::Mat::zeros(2, 2, CV_8UC1);
    cv::Mat test4 = cv::Mat::zeros(2, 2, CV_8UC1);
    cv::Mat test5 = cv::Mat::zeros(2, 2, CV_8UC1);
    cv::Mat test6 = cv::Mat::zeros(2, 2, CV_8UC1);
    cv::Mat test7 = cv::Mat::zeros(2, 2, CV_8UC1);
    cv::Mat test8 = cv::Mat::zeros(2, 2, CV_8UC1);
    cv::Mat test9 = cv::Mat::zeros(2, 2, CV_8UC1);

    cv::solve(matrix_a, matrix_b, result, cv::DECOMP_SVD);

    for (int i = 0; i < 16; i++) {
        cout << "resut martix  i : " << i << "  " << result.at<float>(i, 0) << endl;
    }

    // 0123
    // 4567
    // 891011
    //12131415

    test1.at<uchar>(0, 0) = result.at<float>(0, 0);
    test1.at<uchar>(0, 1) = result.at<float>(1, 0);
    test1.at<uchar>(1, 0) = result.at<float>(4, 0);
    test1.at<uchar>(1, 1) = result.at<float>(5, 0);

    test4.at<uchar>(0, 0) = result.at<float>(4, 0);
    test4.at<uchar>(0, 1) = result.at<float>(5, 0);
    test4.at<uchar>(1, 0) = result.at<float>(8, 0);
    test4.at<uchar>(1, 1) = result.at<float>(9, 0);

    test7.at<uchar>(0, 0) = result.at<float>(8, 0);
    test7.at<uchar>(0, 1) = result.at<float>(9, 0);
    test7.at<uchar>(1, 0) = result.at<float>(12, 0);
    test7.at<uchar>(1, 1) = result.at<float>(13, 0);

    test2.at<uchar>(0, 0) = result.at<float>(1, 0);
    test2.at<uchar>(0, 1) = result.at<float>(2, 0);
    test2.at<uchar>(1, 0) = result.at<float>(5, 0);
    test2.at<uchar>(1, 1) = result.at<float>(6, 0);

    test5.at<uchar>(0, 0) = result.at<float>(5, 0);
    test5.at<uchar>(0, 1) = result.at<float>(6, 0);
    test5.at<uchar>(1, 0) = result.at<float>(9, 0);
    test5.at<uchar>(1, 1) = result.at<float>(10, 0);

    test8.at<uchar>(0, 0) = result.at<float>(9, 0);
    test8.at<uchar>(0, 1) = result.at<float>(10, 0);
    test8.at<uchar>(1, 0) = result.at<float>(13, 0);
    test8.at<uchar>(1, 1) = result.at<float>(14, 0);

    test3.at<uchar>(0, 0) = result.at<float>(2, 0);
    test3.at<uchar>(0, 1) = result.at<float>(3, 0);
    test3.at<uchar>(1, 0) = result.at<float>(6, 0);
    test3.at<uchar>(1, 1) = result.at<float>(7, 0);

    test6.at<uchar>(0, 0) = result.at<float>(6, 0);
    test6.at<uchar>(0, 1) = result.at<float>(7, 0);
    test6.at<uchar>(1, 0) = result.at<float>(10, 0);
    test6.at<uchar>(1, 1) = result.at<float>(11, 0);

    test9.at<uchar>(0, 0) = result.at<float>(10, 0);
    test9.at<uchar>(0, 1) = result.at<float>(11, 0);
    test9.at<uchar>(1, 0) = result.at<float>(14, 0);
    test9.at<uchar>(1, 1) = result.at<float>(15, 0);

    cout << "result matrix:    " << result.at<float>(0, 0) << endl;
    cout << "result matrix:    " << result.at<float>(0, 1) << endl;
    cout << "result matrix:    " << result.at<float>(0, 4) << endl;
    cout << "result matrix:    " << result.at<float>(0, 5) << endl;
    cout << "<<<<<<" << endl;
    cout << "result matrix:    " << result.at<float>(5, 0) << endl;
    cout << "result matrix:    " << result.at<float>(6, 0) << endl;
    cout << "result matrix:    " << result.at<float>(9, 0) << endl;
    cout << "result matrix:    " << result.at<float>(10, 0) << endl;

    // cv::imshow("test", test);
    // cv::imshow("test2", test2);

    cv::Size dsize = cv::Size(width / 3, height / 3);

    cv::resize(test1, dst1, dsize, 0, 0, cv::INTER_LINEAR);
    cv::resize(test2, dst2, dsize, 0, 0, cv::INTER_LINEAR);
    cv::resize(test3, dst3, dsize, 0, 0, cv::INTER_LINEAR);
    cv::resize(test4, dst4, dsize, 0, 0, cv::INTER_LINEAR);
    cv::resize(test5, dst5, dsize, 0, 0, cv::INTER_LINEAR);
    cv::resize(test6, dst6, dsize, 0, 0, cv::INTER_LINEAR);
    cv::resize(test7, dst7, dsize, 0, 0, cv::INTER_LINEAR);
    cv::resize(test8, dst8, dsize, 0, 0, cv::INTER_LINEAR);
    cv::resize(test9, dst9, dsize, 0, 0, cv::INTER_LINEAR);

    // cv::imshow("test1", dst1);
    // cv::waitKey(0);
    // for (int i = 0; i < dst.rows; i++) {
    //     cout << "L:  " << (int)dst.at<uchar>(0, dst.cols - 1)
    //          << " R:   " << (int)dst2.at<uchar>(0, 0) << endl;
    //     cout << "L:  " << (int)dst.at<uchar>(dst.rows, dst.cols - 1)
    //          << " R:   " << (int)dst2.at<uchar>(dst2.rows, 0) << endl;
    // }

    // cout << "test: " << (int)test.at<uchar>(0, 1)
    //      << " test2:  " << (int)test2.at<uchar>(0, 0) << endl;

    // cout << "test: " << (int)test.at<uchar>(1, 1)
    //      << " test2:  " << (int)test2.at<uchar>(1, 0) << endl;

    cout << "hello" << endl;

    dst1.copyTo(final(cv::Rect(0, 0, dst1.cols, dst1.rows)));
    dst2.copyTo(final(cv::Rect(dst1.cols, 0, dst2.cols, dst2.rows)));
    dst3.copyTo(final(cv::Rect(dst1.cols * 2, 0, dst3.cols, dst3.rows)));

    dst4.copyTo(final(cv::Rect(0, dst1.rows, dst4.cols, dst4.rows)));
    dst5.copyTo(final(cv::Rect(dst4.cols, dst4.rows, dst5.cols, dst5.rows)));
    dst6.copyTo(final(cv::Rect(dst4.cols * 2, dst4.rows, dst6.cols, dst6.rows)));

    dst7.copyTo(final(cv::Rect(0, dst6.rows * 2, dst1.cols, dst7.rows)));
    dst8.copyTo(final(cv::Rect(dst7.cols, dst7.rows * 2, dst8.cols, dst8.rows)));
    dst9.copyTo(final(cv::Rect(dst7.cols * 2, dst7.rows * 2, dst9.cols, dst9.rows)));

    final.copyTo(img, mask_img);

    // cv::imshow("111111", dst);
    // cv::imshow("22222", dst2);
    // cv::imshow("333333", final);
    // cv::imshow("mask", mask_img);
    // cv::imshow("img", img);

    // cv::waitKey(0);

    return img;
}

cv::Mat general_planefit(cv::Mat img, cv::Mat mask_img, int sample_size, int num_of_sample, int num_row, int num_col)
{

    int num_of_cp = 0;
    num_of_cp = (num_row + 1) * (num_col + 1);
    // cout << "num of cp= " << num_of_cp << endl;

    cv::Mat final = cv::Mat::zeros(img.size(), CV_8UC1);
    cv::Mat result = cv::Mat::zeros(num_of_cp, 1, CV_32FC1);
    cv::Mat matrix_a = cv::Mat::zeros(num_of_sample, num_of_cp, CV_32FC1);
    cv::Mat matrix_b = cv::Mat::zeros(num_of_sample, 1, CV_32FC1);

    int height = img.rows;
    int width = img.cols;
    int count = 0;

    float size_of_block;

    int index_for_rect[num_row * num_col][4];
    memset(index_for_rect, 0, num_row * num_col * sizeof(int) * 4);
    int go_to_next_row = 0;
    int num_of_in_row = num_col + 1;

    for (int i = 0; i < num_row * num_col; i++) {

        index_for_rect[i][0] = i + go_to_next_row;
        index_for_rect[i][1] = i + go_to_next_row + 1;
        index_for_rect[i][2] = i + go_to_next_row + num_of_in_row;
        index_for_rect[i][3] = i + go_to_next_row + 1 + num_of_in_row;

        if ((i + 2 + go_to_next_row) % (num_of_in_row) == 0) {
            go_to_next_row++;
        }
        cout << i << ": " << index_for_rect[i][0];
        cout << " " << index_for_rect[i][1];
        cout << " " << index_for_rect[i][2];
        cout << " " << index_for_rect[i][3] << endl;
    }
    go_to_next_row = 0;

    int row_num_in_matrix = 0;
    float y_0 = 0, y_1 = 0, y_2 = 0, x_0 = 0, x_1 = 0, x_2 = 0;
    int count_width = 0, count_height = 0;
    int pos_index = 0;

    for (int current_row = 0; current_row < height; current_row += sample_size) {
        for (int current_col = 0; current_col < width; current_col += sample_size) {

            if (mask_img.at<uchar>(current_row, current_col) > 0) {
                size_of_block = (height / num_row * (width * 1.0 / num_col));
                matrix_b.at<float>(row_num_in_matrix, 0) = img.at<uchar>(current_row, current_col); // y 行 第x個

                count_height = current_row / (height / num_row * 1.0);
                count_width = current_col / (width / num_col * 1.0);
                if (count_height == num_row) {
                    count_height--;
                }
                if (count_width == num_col) {
                    count_width--;
                }

                cout << "ch:  " << count_height << endl;
                // cout<< "  cw:  " << count_width << endl;
                x_0 = current_col;
                x_2 = (width / num_col * 1.0) * count_width;
                x_1 = x_2 - width / num_col * 1.0;

                y_0 = current_row;
                y_1 = (height / num_row * 1.0) * count_height;
                y_2 = y_1 - height / num_row * 1.0; //small;

                pos_index = count_height * num_col + count_width;

                matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][0]) = (x_2 - x_0) * (y_1 - y_0) / size_of_block;
                matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][1]) = (x_0 - x_1) * (y_1 - y_0) / size_of_block;
                matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][2]) = (x_2 - x_0) * (y_0 - y_2) / size_of_block;
                matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][3]) = (x_0 - x_1) * (y_0 - y_2) / size_of_block;

                cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][0]) << endl;
                cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][1]) << endl;
                cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][2]) << endl;
                cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][3]) << endl;

                cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][0]) +
                matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][1]) +
                matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][2]) +
                matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][3]) << endl;
                row_num_in_matrix++;
            }
        }
    }

    cv::Size dsize = cv::Size(width / (num_col * 1.0), height / (num_row * 1.0));

    vector<cv::Mat> dst;
    for (int i = 0; i < (num_col * num_row); i++) {
        dst.push_back(cv::Mat::zeros(2, 2, CV_8UC1));
    }
    vector<cv::Mat> block;
    for (int i = 0; i < (num_col * num_row); i++) {
        block.push_back(cv::Mat::zeros(2, 2, CV_8UC1));
    }

    cv::solve(matrix_a, matrix_b, result, cv::DECOMP_SVD);
    cout << "hi" << endl;
    for (int i = 0; i < num_of_cp; i++) {
        cout << "result matrix: " << i << "  " << result.at<float>(i, 0) << endl;
    }
    cout << "hi" << endl;

    int index_in_result_matrix[4] = { 0 };
    int num_of_next_row = 0;
    int cp_num_in_row = num_col + 1;

    for (int i = 0; i < num_row * num_col; i++) {

        index_in_result_matrix[0] = i + num_of_next_row;
        index_in_result_matrix[1] = i + num_of_next_row + 1;
        index_in_result_matrix[2] = i + num_of_next_row + cp_num_in_row;
        index_in_result_matrix[3] = i + num_of_next_row + 1 + cp_num_in_row;

        if ((i + 2 + num_of_next_row) % (cp_num_in_row) == 0) {
            num_of_next_row++;
        }

        block[i].at<uchar>(0, 0) = result.at<float>(index_in_result_matrix[0], 0);
        block[i].at<uchar>(0, 1) = result.at<float>(index_in_result_matrix[1], 0);
        block[i].at<uchar>(1, 0) = result.at<float>(index_in_result_matrix[2], 0);
        block[i].at<uchar>(1, 1) = result.at<float>(index_in_result_matrix[3], 0);

        // cout << i << ": " << index_in_result_matrix[0];
        // cout << " " << index_in_result_matrix[1];
        // cout << " " << index_in_result_matrix[2];
        // cout << " " << index_in_result_matrix[3] << endl;
    }

    num_of_next_row = 0;

    for (int i = 0; i < num_row * num_col; i++) {
        cv::resize(block[i], dst[i], dsize, 0, 0, cv::INTER_LINEAR);
    }

    for (int i = 0; i < num_row * num_col; i++) { //4

        dst[i].copyTo(final(cv::Rect((i % num_col) * dst[0].cols, num_of_next_row * dst[0].rows, dst[i].cols, dst[i].rows)));
        //  cout<<" dst["<<i<<"].copyTo(final(cv::Rect(("<<(i % num_col) <<"  * dst[0].cols  "<< num_of_next_row<<"   * dst[0].rows"<< "dst["<<i<<"].cols, dst["<<i<<"].rows)))"<<endl;

        if ((i + 2 + num_of_next_row) % (cp_num_in_row) == 0) {
            num_of_next_row++;
        }
    }

    final.copyTo(img, mask_img);

    return img;
}

```




```
cv::Mat dark_otsu(cv::Mat old, cv::Mat thenew);
cv::Mat bright_otsu(cv::Mat old, cv::Mat thenew);
cv::Mat Binarize_otsu(cv::Mat gray);
```

```
cv::Mat dark_otsu(cv::Mat old, cv::Mat refplane)
{
    cv::Mat dark;

    cv::subtract(refplane, old, dark);

    dark = Binarize_otsu(dark);

    cv::imshow("dark", dark);

    return dark;
}

cv::Mat bright_otsu(cv::Mat old, cv::Mat refplane)
{
    cv::Mat bright;

    cv::subtract(old, refplane, bright);

    bright = Binarize_otsu(bright);

    cv::imshow("Bright", bright);

    return bright;
}


cv::Mat Binarize_otsu(cv::Mat gray)
{
    int width = gray.cols;
    int height = gray.rows;

    // determine threshold
    double w0 = 0, w1 = 0;
    double m0 = 0, m1 = 0;
    double max_sb = 0, sb = 0;
    int th = 0;
    int val;

    // Get threshold
    for (int t = 0; t < 255; t++) {
        w0 = 0;
        w1 = 0;
        m0 = 0;
        m1 = 0;
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                val = (int)(gray.at<uchar>(y, x));

                if (val < t) {
                    w0++;
                    m0 += val;
                } else {
                    w1++;
                    m1 += val;
                }
            }
        }

        m0 /= w0;
        m1 /= w1;
        w0 /= (height * width);
        w1 /= (height * width);
        sb = w0 * w1 * pow((m0 - m1), 2);

        if (sb > max_sb) {
            max_sb = sb;
            th = t;
        }
    }

    std::cout << "threshold:" << th << std::endl;

    // prepare output
    cv::Mat out = cv::Mat::zeros(height, width, CV_8UC1);

    // each y, x
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Binarize
            if (gray.at<uchar>(y, x) > th) {
                out.at<uchar>(y, x) = 255;
            } else {
                out.at<uchar>(y, x) = 0;
            }
        }
    }

    return out;
}

```