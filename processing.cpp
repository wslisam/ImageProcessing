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
	//  0    1   00  10
	//  2    3   01  11
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
		{ //    X                                 Y
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

int cal_and_cut(cv::Mat img, cv::Mat mask, int Grid_size)
{
	int height = img.rows;
	int width = img.cols;

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

	for (int seg = 0; seg < num_of_region; seg++) // seg = region index
	{
		y_0 = rect_coord[seg][0].second;
		y_1 = rect_coord[seg][2].second;
		num_of_sample[seg] = 0;

		for (int y = y_0 + 1; y < y_1; y += Grid_size) // size can the same
		{
			dy = ((y - y_0) / (y_1 - y_0 * 1.0));

			x_0 = rect_coord[seg][0].first;
			x_1 = rect_coord[seg][1].first;

			for (int x = x_0 + 1; x < x_1; x += Grid_size)
			{ // rect seg  point 0
				dx = ((x - x_0) / (x_1 - x_0 * 1.0));

				if (mask.at<uchar>(y, x) != 0)
				{
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

					temp_row.push_back((1.0 - dx) * dy);		 //00
					temp_row.push_back(dx * dy);				 //10
					temp_row.push_back((1.0 - dx) * (1.0 - dy)); //01
					temp_row.push_back(dx * (1.0 - dy));		 //11
					M_A[seg].push_back(temp_row);

					(num_of_sample[seg])++;
				}
			}
		}
		cv::Mat m_roi = img(cv::Rect(rect_coord[seg][0].first,
									 rect_coord[seg][0].second,
									 rect_coord[seg][1].first - rect_coord[seg][0].first,
									 rect_coord[seg][2].second - rect_coord[seg][0].second));

		cv::Mat mask_roi = mask(cv::Rect(rect_coord[seg][0].first,
										 rect_coord[seg][0].second,
										 rect_coord[seg][1].first - rect_coord[seg][0].first,
										 rect_coord[seg][2].second - rect_coord[seg][0].second));

		// cout << "num" << num_of_sample[seg] << endl;

		single_planefit(m_roi, mask_roi, Grid_size, M_B[seg], M_A[seg], num_of_sample[seg]);
	}

	// for (vector<vector<vector<float>>>::const_iterator i = M_A.begin(); i != M_A.end(); ++i)
	// 				{
	// 					for (vector<vector<float>>::const_iterator j = i->begin(); j != i->end(); ++j)
	// 					{
	// 						for (vector<float>::const_iterator k = j->begin(); k != j->end(); ++k)
	// 						{
	// 							cout << *k << ' '<<endl;
	// 						}
	// 					}
	// }

	return 0;
}

void single_planefit(cv::Mat contour_region, cv::Mat mask_region, int Grid_size, vector<vector<int>> M_B, vector<vector<float>> M_A, int num_of_sample)
{
	cv::imshow("ROI", contour_region);
	cv::imshow("MASKROI", mask_region);
	cv::Mat result = cv::Mat::zeros(4, 1, CV_32FC1);
	cv::Mat matrix_a = cv::Mat::zeros(num_of_sample, 4, CV_32FC1);
	cv::Mat matrix_b = cv::Mat::zeros(num_of_sample, 1, CV_32FC1);

	//圖入黎,seg唔seg ,／ 直斬，每一份掉入去一個reg ,掉function cut格

	for (int sample = 0; sample < num_of_sample; sample++)
	{
		matrix_b.at<float>(sample, 0) = M_B[sample][0];
		// cout<<M_B[sample][0]<<endl;
	}

	for (int current_row = 0; current_row < num_of_sample; current_row++)
	{
		for (int i = 0; i < 4; i++)
		{
			matrix_a.at<float>(current_row, i) = M_A[current_row][i];
		}
	}

	cv::solve(matrix_a, matrix_b, result, cv::DECOMP_SVD);

   	cout << "result" << endl;
	for (int n = 0; n <= 3; n++)
	{ 
		cout << result.at<float>(n, 1) << endl;
	}
	// cv::imshow("result", result);

	return;
}