
#include <opencv2/highgui/highgui_c.h>
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char **argv) {
	Mat z = imread("color2.webp", IMREAD_COLOR);

	Mat M = Mat_<double>(z.rows * z.cols, 6);
	Mat I = Mat_<double>(z.rows * z.cols, 1);
	for (int i = 0; i < z.rows; i++)
		for (int j = 0; j < z.cols; j++) {
			double x = (j - z.cols / 2) / double(z.cols),
				   y = (i - z.rows / 2) / double(z.rows);
			M.at<double>(i * z.cols + j, 0) = x * x;
			M.at<double>(i * z.cols + j, 1) = y * y;
			M.at<double>(i * z.cols + j, 2) = x * y;
			M.at<double>(i * z.cols + j, 3) = x;
			M.at<double>(i * z.cols + j, 4) = y;
			M.at<double>(i * z.cols + j, 5) = 1;
			I.at<double>(i * z.cols + j, 0) = z.at<uchar>(i, j);
		}
	SVD s(M);
	Mat q;
	s.backSubst(I, q);
	std::cout << q;
	imshow("Orignal", z);
	std::cout << q.at<double>(2, 0);
	Mat background(z.rows, z.cols, CV_8UC1);
	for (int i = 0; i < z.rows; i++)
		for (int j = 0; j < z.cols; j++) {
			double x = (j - z.cols / 2) / double(z.cols),
				   y = (i - z.rows / 2) / double(z.rows);
			double quad = q.at<double>(0, 0) * x * x +
						  q.at<double>(1, 0) * y * y +
						  q.at<double>(2, 0) * x * y;
			quad += q.at<double>(3, 0) * x + q.at<double>(4, 0) * y +
					q.at<double>(5, 0);
			background.at<uchar>(i, j) = saturate_cast<uchar>(quad);
		}
	imshow("Simulated background", background);
	waitKey();
	return 0;
}