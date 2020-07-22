#include "processing.h"

bool light_diff(int current, int last)
{
	// calculate the difference of that block / pixel
	int diff = abs(current - last);

	if (diff > 10) //big difference !?
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
				val = (1.0 - dx) * (1.0 - dy) * img.at<cv::Vec3b>(y_before, x_before)[c] +
					  dx * (1.0 - dy) * img.at<cv::Vec3b>(y_before, x_before + 1)[c] +
					  (1.0 - dx) * dy * img.at<cv::Vec3b>(y_before + 1, x_before)[c] +
					  dx * dy * img.at<cv::Vec3b>(y_before + 1, x_before)[c];

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

cv::Mat single_planefit(cv::Mat img)
{
	int height = img.rows;
	int width = img.cols;
	int num_of_sample = 9 * 7;
	cv::Mat result = cv::Mat::zeros(4, 1, CV_32FC1);
	cv::Mat matrix_a = cv::Mat::zeros(num_of_sample, 4, CV_32FC1);
	cv::Mat matrix_b = cv::Mat::zeros(num_of_sample, 1, CV_32FC1);

	int current_row = 0;
	int current_col = 0;
	double dx, dy;

	for (int y = 0 + 1; y < height - 1; y += 50)
	{
		dy = (y * 1.0 / height);

		for (int x = 0 + 1; x < width - 1; x += 200)
		{
			if (img.at<uchar>(y, x) != 0)
			{

				dx = (x * 1.0 / width);
				// cout << "dx" << dx << endl;

				matrix_b.at<float>(current_row, 1) = img.at<uchar>(y, x);
				// cout<<matrix_b.at<float>(current_row, 1) <<endl;

				matrix_a.at<float>(current_row, current_col + 0) = (1.0 - dx) * dy;			//00
				matrix_a.at<float>(current_row, current_col + 1) = dx * dy;					//10
				matrix_a.at<float>(current_row, current_col + 2) = (1.0 - dx) * (1.0 - dy); //01
				matrix_a.at<float>(current_row, current_col + 3) = dx * (1.0 - dy);			//11

				//(p00)*  *  (p10)
				//(p01)*  *  (p11)

				// cout << "sasm" << endl;
				// cout << setprecision(2) << matrix_a.at<float>(current_row, current_col + 0) << endl; //00
				// cout << setprecision(2) << matrix_a.at<float>(current_row, current_col + 1) << endl; //10
				// cout << setprecision(2) << matrix_a.at<float>(current_row, current_col + 2) << endl; //01
				// cout << setprecision(2) << matrix_a.at<float>(current_row, current_col + 3) << endl; //11

				current_row++; // move to next line for next sample
			}
		}
	}
	cv::solve(matrix_a, matrix_b, result, 1); //cv::DECOMP_SVD
	// cout << "result" << endl;
	// cout << result.at<float>(0, 1) << endl;
	// cout << result.at<float>(1, 1) << endl;

	// cout << result.at<float>(2, 1) << endl;

	// cout << result.at<float>(3, 1) << endl;

	return result;
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

cv::Mat segmentation(cv::Mat input, int x_pos, int y_pos, int w, int h)
{

	cv::Mat input_roi = input(cv::Rect(x_pos, y_pos, w, h)); //e.g function contour to cv rect
															 // depend on w , h  , not area
															 //    cv::Rect(x_pos,y_pos,w,h)
															 //    cv::imwrite("ROI.bmp",input_roi);
	return input_roi;
}

int Contours(cv::Mat img)
{
	vector<vector<cv::Point>> contours;

	findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	cv::Mat output = cv::Mat::zeros(img.rows, img.cols, CV_8UC3);

	if (contours.size() == 0)
	{
		cout << "No objects detected" << endl;
		return 0;
	}
	else
	{
		cout << "Number of objects detected: " << contours.size() << endl;
	}

	//    vector<cv::Point> ctr = contours.at(3);
	//    for (int i = 0; i < ctr.size(); i++) {
	//     // cv::Point coordinate_i_ofcontour = ctr.size();
	//     cout << endl << "contour with coordinates: x = " << ctr[i].x << " y = " << ctr[i].y;
	// }

	vector<cv::Rect> boundRect(contours.size());
	//01
	//23

	for (int i = 0; i < contours.size(); i++)
	{
		boundRect[i] = boundingRect(contours[i]); //  次次都右至左  下至上

		rectangle(img, boundRect[i], cv::Scalar(255, 0, 255));

		cout << "Rect " << i << endl;
		cout << "Point 0  :" << boundRect[i].x << " , " << boundRect[i].y << endl;
		cout << "Point 1  :" << boundRect[i].x + boundRect[i].width << " , " << boundRect[i].y << endl;
		cout << "Point 2  :" << boundRect[i].x << " , " << boundRect[i].y + boundRect[i].height << endl;
		cout << "Point 3  :" << boundRect[i].x + boundRect[i].width << " , " << boundRect[i].y + boundRect[i].height << endl;

		imshow("rect", img);
	}

	// cv::RNG rng(0xFFFFFFFF);
	for (int i = 0; i < contours.size(); i++)
	{
		cv::drawContours(output, contours, i, 0x0000BBBB);
	}

	cv::imshow("Contours Result", output);

	return contours.size();
}