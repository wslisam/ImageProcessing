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

int find_num_obj_using_ConnectedComponents(cv::Mat img)
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

vector<vector<pair<int, int>>> rect_contours(cv::Mat img, vector<vector<cv::Point>> contours)
{
	vector<cv::Rect> boundRect(contours.size());
	//  0    1
	//  2    3
	vector<vector<pair<int, int>>> coord(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		boundRect[i] = boundingRect(contours[i]); //  次次都右至左  下至上

		rectangle(img, boundRect[i], cv::Scalar(255, 0, 255));

		imshow("Rect", img);

		cout << "Rect " << i << endl;

		coord[i].push_back(make_pair(boundRect[i].x, boundRect[i].y));
		coord[i].push_back(make_pair(boundRect[i].x + boundRect[i].width, boundRect[i].y));
		coord[i].push_back(make_pair(boundRect[i].x, boundRect[i].y + boundRect[i].height));
		coord[i].push_back(make_pair(boundRect[i].x + boundRect[i].width, boundRect[i].y + boundRect[i].height));
		for (int j = 0; j <= 3; j++)
		{
			cout << "Point " << j << " :" << coord[i][j].first << " " << coord[i][j].second << endl;
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

	if (contours.size() == 0)
	{
		cout << "No objects detected" << endl;
		return 0;
	}
	else
	{
		cout << "Number of objects detected: " << contours.size() << endl;
	}

	rect_contours(img, contours);

	// cv::RNG rng(0xFFFFFFFF);
	//label the contours
	for (int i = 0; i < contours.size(); i++)
	{
		cv::drawContours(output, contours, i, 0x0000BBBB);
	}

	cv::imshow("Contours Result", output);

	return contours.size();
}

cv::Mat single_planefit(cv::Mat img, cv::Mat mask, int Grid_size)
{
	int height = img.rows;
	int width = img.cols;
	int num_of_sample = 0;

	vector<vector<cv::Point>> contours;
	findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
	vector<vector<pair<int, int>>> rect_coord = rect_contours(img, contours);

	vector<int> M_B;
	vector<vector<float>> M_A;
	float dx, dy;

	for (int y = 0 + 1; y < height - 1; y += Grid_size)
	{
		dy = (y * 1.0 / height);

		for (int x = 0 + 1; x < width - 1; x += Grid_size)
		{
			if (mask.at<uchar>(y, x) != 0)
			{
				dx = (x * 1.0 / width);

				M_B.push_back((int)img.at<uchar>(y, x));
				cout << M_B[num_of_sample] << endl;

				vector<float> temp_row;
				// M_A[num_of_sample].resize(4);

				temp_row.push_back((1.0 - dx) * dy);		 //00
				temp_row.push_back(dx * dy);				 //10
				temp_row.push_back((1.0 - dx) * (1.0 - dy)); //01
				temp_row.push_back(dx * (1.0 - dy));		 //11
				M_A.push_back(temp_row);

				//Print the value in M_A
				// cout << "value" << endl;
				// cout << M_A[num_of_sample][0] << endl;
				// cout << M_A[num_of_sample][1] << endl;
				// cout << M_A[num_of_sample][2] << endl;
				// cout << M_A[num_of_sample][3] << endl;

				num_of_sample++;
			}
		}
	}

	// cout << "num" << num_of_sample << endl;

	cv::Mat result = cv::Mat::zeros(4, 1, CV_32FC1);
	cv::Mat matrix_a = cv::Mat::zeros(num_of_sample, 4, CV_32FC1);
	cv::Mat matrix_b = cv::Mat::zeros(num_of_sample, 1, CV_32FC1);

	// matrix_b = cv::Mat(M_B).reshape(1, num_of_sample);

	for (int f = 0; f < num_of_sample; f++)
	{

		matrix_b.at<float>(f, 1) = M_B[f];
	}

	// matrix_a = cv::Mat(M_A).reshape(1, num_of_sample);

	for (int current_row = 0; current_row < num_of_sample; current_row++)
	{
		matrix_a.at<float>(current_row, 0) = M_A[current_row][0];
		matrix_a.at<float>(current_row, 1) = M_A[current_row][1];
		matrix_a.at<float>(current_row, 2) = M_A[current_row][2];
		matrix_a.at<float>(current_row, 3) = M_A[current_row][3];
	}

	// check the src type
	// cout<< typeid(matrix_b).name()<<endl;
	// cout<< typeid(matrix_a).name()<<endl;
	// cout<< typeid(result).name()<<endl;
	// cout<<(matrix_a.type()==CV_32F)<<endl;

	cv::solve(matrix_a, matrix_b, result, 1); // cv::DECOMP_SVD

	cout << "result" << endl;
	for (int n = 0; n <= 3; n++)
	{
		cout << result.at<float>(n, 1) << endl;
	}

	return result;
}
