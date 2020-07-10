#ifndef threshold_hpp
#define threshold_hpp

#include <stdio.h>

#include <opencv2/opencv.hpp>

using namespace cv;

void meanBinaryThreshold(const Mat& src, Mat& dst, const int blockSize,
						 const double C);
void gaussianBinaryThreshold(const Mat& src, Mat& dst, const int blockSize,
							 const double C);

#endif /* threshold_hpp */