#include "processing.h"

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

int get_num_sample(cv::Mat img, cv::Mat mask, int sample_size)
{
    int height = img.rows;
    int width = img.cols;

    int num_of_sample = 0;
    // int num_of_region = 2;

    for (int y = 0 + 1; y < height - 1; y += sample_size) {
        for (int x = 0 + 1; x < width - 1; x += sample_size) {
            if (mask.at<uchar>(y, x) >= 0) {
                num_of_sample++;
            }
        }
    }
    return num_of_sample;
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

final_struct planefit(cv::Mat img, cv::Mat mask, int num_row, int num_col)
{
    int height = img.rows;
    int width = img.cols;

    vector<vector<cv::Point>> contours;
    findContours(img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    vector<vector<pair<int, int>>> rect_coord = rect_contours(img, contours);
    int num_of_sample[contours.size()]; // seg幾 左定右region

    cv::Mat m_roi;
    cv::Mat mask_roi;
    final_struct final_result;
    grid_struct grid;

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

        grid = gen2_general_planefit(m_roi, mask_roi, 4, num_of_sample[seg], num_row, num_col, rect_coord[seg]);

        final_result.grid_vector.push_back(grid);

        // imshow("888",grid.ref_plane);
        // cout<<"TMDLMR"<<endl;
        //  TMDLMR_planefit(m_roi, mask_roi, 4, num_of_sample[seg]);

        // cv::imshow("final", m_roi);
        // cv::waitKey(0);
    }

    // int temp = (num_col + 1) * (num_row + 1);
    // cout << ">>>>>>>>>>>>>>>>" << endl;
    // for (int i = 0; i < temp; i++) {
    //     cout << final_result.grid_vector[3].point[i].z_value << endl;
    // }
    // cout << ">>>>>>>>>>>>>>>>" << endl;

    final_result.whole_plane = img;
    return final_result;
}

cv::Mat LR_planefit(cv::Mat img, cv::Mat mask_img, int sample_size, int num_of_sample) // 1x2
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

cv::Mat TDLR_planefit(cv::Mat img, cv::Mat mask_img, int sample_size, int num_of_sample) // 2x2
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
    for (int current_row = 0 + 1; current_row < height - 1; current_row += sample_size) {
        for (int current_col = 0 + 1; current_col < width - 1; current_col += sample_size) {

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

cv::Mat TMDLMR_planefit(cv::Mat img, cv::Mat mask_img, int sample_size, int num_of_sample) // 3x3
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

grid_struct general_planefit(cv::Mat img, cv::Mat mask_img, int sample_size, int num_of_sample, int num_row, int num_col, vector<pair<int, int>> rect)
{
    int height = img.rows;
    int width = img.cols;
    int num_of_cp = (num_row + 1) * (num_col + 1);

    cv::Mat final = cv::Mat::zeros(img.size(), CV_8UC1);
    cv::Mat result = cv::Mat::zeros(num_of_cp, 1, CV_32FC1);
    cv::Mat matrix_a = cv::Mat::zeros(num_of_sample, num_of_cp, CV_32FC1);
    cv::Mat matrix_b = cv::Mat::zeros(num_of_sample, 1, CV_32FC1);

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

                // cout << "ch:  " << count_height << endl;
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
                row_num_in_matrix++;

                // cout<<"hi" <<(x_2 - x_0) * (y_1 - y_0) / size_of_block <<endl;
                if (current_col < (width / 2)) {
                    if (current_row < (height / 2)) {
                        cout << "value " << matrix_b.at<float>(row_num_in_matrix - 1, 0)
                             << " " << matrix_a.at<float>(row_num_in_matrix - 1, index_for_rect[pos_index][0])
                             << " " << matrix_a.at<float>(row_num_in_matrix - 1, index_for_rect[pos_index][1])
                             << " " << matrix_a.at<float>(row_num_in_matrix - 1, index_for_rect[pos_index][2])
                             << " " << matrix_a.at<float>(row_num_in_matrix - 1, index_for_rect[pos_index][3]) << endl;

                        cout << "index for  " << index_for_rect[pos_index][0] << " >>> " << index_for_rect[pos_index][1] << ">> " << index_for_rect[pos_index][2] << " >>>>" << index_for_rect[pos_index][3] << endl;
                    }
                }

                // cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][0]) << endl;
                // cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][1]) << endl;
                // cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][2]) << endl;
                // cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][3]) << endl;
                // cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][0]) + matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][1]) + matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][2]) + matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][3]) << endl;
            }
        }
    }

    vector<cv::Mat> dst;
    vector<cv::Mat> block;
    cv::Size dsize = cv::Size(width / (num_col * 1.0), height / (num_row * 1.0));

    for (int i = 0; i < (num_col * num_row); i++) {
        dst.push_back(cv::Mat::zeros(2, 2, CV_8UC1));
        block.push_back(cv::Mat::zeros(2, 2, CV_8UC1));
    }

    cv::solve(matrix_a, matrix_b, result, cv::DECOMP_SVD);

    cout << "<<<<<<<<<<<<" << endl;
    for (int i = 0; i < num_of_cp; i++) {
        cout << "result matrix: " << i << "  " << result.at<float>(i, 0) << endl;
    }
    cout << "<<<<<<<<<<<<" << endl;

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
    }

    num_of_next_row = 0;

    for (int i = 0; i < num_row * num_col; i++) {
        cv::resize(block[i], dst[i], dsize, 0, 0, cv::INTER_LINEAR);
    }

    for (int i = 0; i < num_row * num_col; i++) {
        dst[i].copyTo(final(cv::Rect((i % num_col) * dst[0].cols, num_of_next_row * dst[0].rows, dst[i].cols, dst[i].rows)));
        //  cout<<" dst["<<i<<"].copyTo(final(cv::Rect(("<<(i % num_col) <<"  * dst[0].cols  "<< num_of_next_row<<"   * dst[0].rows"<< "dst["<<i<<"].cols, dst["<<i<<"].rows)))"<<endl;

        if ((i + 2 + num_of_next_row) % (cp_num_in_row) == 0) {
            num_of_next_row++;
        }
    }

    cp_struct cp;
    grid_struct grid;

    for (int k = 0; k <= num_row; k++) {
        for (int i = 0; i <= num_col; i++) {

            cp.x_coord = rect[0].first + width / num_col * i;
            cp.y_coord = rect[0].second + height / num_row * k;
            cp.z_value = result.at<float>((num_col + 1) * k + i, 0);
            // cout << "value x: " << cp.x_coord << endl;
            // cout << "value y: " << cp.y_coord << endl;
            // cout << "value z: " << cp.z_value << endl;
            grid.point.push_back(cp);
        }
    }

    grid.ref_plane = final;

    final.copyTo(img, mask_img);

    return grid;
}

// cv::Mat frameDiff(cv::Mat prevFrame, cv::Mat curFrame, cv::Mat nextFrame)
// {
//     cv::Mat diffFrames1, diffFrames2, output;
//     // Compute absolute difference between current frame and the next frame
//     cv::absdiff(nextFrame, curFrame, diffFrames1);
//     // Compute absolute difference between current frame and the previous frame
//     cv::absdiff(curFrame, prevFrame, diffFrames2);
//     // Bitwise "AND" operation between the above two diff images
//     cv:::bitwise_and(diffFrames1, diffFrames2, output);
//     return output;
// }

cv::Mat Diff(cv::Mat filter, cv::Mat refplane)
{
    cv::Mat output;
    // Compute absolute difference between current frame and the next frame
    cv::absdiff(filter, refplane, output);
    cv::imshow("diff", output);

    return output;
}

cv::Mat bright_and_dark(cv::Mat old, cv::Mat refplane, int threshold)
{
    cv::Mat dark, bright;

    cv::subtract(refplane, old, dark); //dark def
    cv::subtract(old, refplane, bright); //bright def

    // background 係  0  = black , defect in white

    dark = Binarize(dark, threshold);
    bright = Binarize(bright, threshold);

    cv::imshow("dark", dark);
    cv::imshow("Bright", bright);
    // cv::imwrite("./images/new/1by2/1by2_threshold35_dark.bmp", dark);
    // cv::imwrite("./images/new/1by2/1by2_threshold35_bright.bmp", bright);

    return dark;

    //when the value of threshold goes up , the defect cannot be recognised.
    // 10 就太細  35 淨係見到一條彎
}

cv::Mat dark(cv::Mat old, cv::Mat refplane, int threshold)
{
    cv::Mat dark;

    cv::subtract(refplane, old, dark);

    dark = Binarize(dark, threshold);

    cv::imshow("dark", dark);

    return dark;
}

cv::Mat bright(cv::Mat old, cv::Mat refplane, int threshold)
{
    cv::Mat bright;

    cv::subtract(old, refplane, bright);

    bright = Binarize(bright, threshold);

    cv::imshow("Bright", bright);

    return bright;
}

cv::Mat Binarize(cv::Mat gray, int threshold)
{
    int width = gray.cols;
    int height = gray.rows;

    // prepare output
    cv::Mat out = cv::Mat::zeros(height, width, CV_8UC1);

    // each y, x
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Binarize
            if (gray.at<uchar>(y, x) > threshold) {
                out.at<uchar>(y, x) = 255;
            } else {
                out.at<uchar>(y, x) = 0;
            }
        }
    }

    return out;
}

// 40 /60 /80    // 解釋
//2x2 / 3x3
//general defect inspection
// mearsure size of defect
//diff buffer
// threshold value different    // sensiti

grid_struct gen2_general_planefit(cv::Mat img, cv::Mat mask_img, int sample_size, int num_of_sample, int num_row, int num_col, vector<pair<int, int>> rect)
{
    int height = img.rows;
    int width = img.cols;
    int num_of_cp = (num_row + 1) * (num_col + 1);

    cv::Mat final = cv::Mat::zeros(img.size(), CV_8UC1);
    cv::Mat result = cv::Mat::zeros(num_of_cp, 1, CV_32FC1);
    cv::Mat matrix_a = cv::Mat::zeros(num_of_sample, num_of_cp, CV_32FC1);
    cv::Mat matrix_b = cv::Mat::zeros(num_of_sample, 1, CV_32FC1);

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

    float dx, dy;

    for (int current_row = 0 + 1; current_row < height - 1; current_row += sample_size) {
        for (int current_col = 0 + 1; current_col < width - 1; current_col += sample_size) {

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

                // cout << "ch:  " << count_height << endl;
                // cout<< "  cw:  " << count_width << endl;
                x_0 = current_col;
                x_2 = (width / num_col * 1.0) * (count_width + 1);
                if (count_width >= 1) {
                    x_1 = x_2 - width / num_col * 1.0;
                } else {
                    x_1 = 0;
                }

                y_0 = current_row;
                y_1 = (height / num_row * 1.0) * (count_height + 1);
                if (count_height >= 1) {
                    y_2 = y_1 - height / num_row * 1.0; //small;
                } else {
                    y_2 = 0;
                }

                pos_index = count_height * num_col + count_width;

                dx = (current_col - x_1) / (x_2 - x_1);
                dy = (current_row - y_2) / (y_1 - y_2);
                //  cout << "1dx: " << dx << "1dy:  " << dy << endl;
                // cout << " row 1 : " << current_col << "  x1 " << x_1 << " " << (x_2 - x_1) << endl;
                // cout << " row 1.1 : " << current_col- x_1 << " " << (x_2 - x_1) << endl;

                // cout << " row 2 : " << current_row << "  y2 " << y_2 << " " << (y_1 - y_2) << endl;

                //  cout<<" row 2 : " <<(current_col - x_1)  <<" "<< (x_2 - x_1) <<endl;
                // dx = (x_0-x_1) / (width * 1.0 / num_col);
                // dy = (y_0 - y_2) / (height / num_row * 1.0);
                // cout << "dx: " << dx << "dy:  " << dy << endl;
                // matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][0]) = (1. - dx) * dy;
                // matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][1]) = dx * dy;
                // matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][2]) = (1.0 - dx) * (1.0 - dy);
                // matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][3]) = dx * (1.0 - dy);

                matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][0]) = ((x_2 - x_0) / (width * 1.0 / num_col)) * ((y_1 - y_0) / (height / num_row * 1.0));
                matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][1]) = ((x_0 - x_1) / (width * 1.0 / num_col)) * ((y_1 - y_0) / (height / num_row * 1.0));
                matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][2]) = ((x_2 - x_0) / (width * 1.0 / num_col)) * ((y_0 - y_2) / (height / num_row * 1.0));
                matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][3]) = ((x_0 - x_1) / (width * 1.0 / num_col)) * ((y_0 - y_2) / (height / num_row * 1.0));
                row_num_in_matrix++;

                // cout<<"hi" <<(x_2 - x_0) * (y_1 - y_0) / size_of_block <<endl;
                if (current_col < (width / 2)) {
                    if (current_row < (height / 2)) {
                        // cout << "value "
                        //      << ((x_2 - x_0) / (width * 1.0 / num_col)) << "  " << ((y_1 - y_0) / (height / num_row * 1.0)) << "  "
                        //      << ((x_0 - x_1) / (width * 1.0 / num_col)) << "  " << ((y_1 - y_0) / (height / num_row * 1.0)) << "  "
                        //      << ((x_2 - x_0) / (width * 1.0 / num_col)) << "  " << ((y_0 - y_2) / (height / num_row * 1.0)) << "  "
                        //      << ((x_0 - x_1) / (width * 1.0 / num_col)) << "  " << ((y_0 - y_2) / (height / num_row * 1.0)) << endl;
                        // cout << "value " << matrix_b.at<float>(row_num_in_matrix - 1, 0)<<
                        cout << matrix_a.at<float>(row_num_in_matrix - 1, index_for_rect[pos_index][0])
                                + matrix_a.at<float>(row_num_in_matrix - 1, index_for_rect[pos_index][1])
                                + matrix_a.at<float>(row_num_in_matrix - 1, index_for_rect[pos_index][2])
                                + matrix_a.at<float>(row_num_in_matrix - 1, index_for_rect[pos_index][3])
                             << endl;

                        // cout << "index for  " << index_for_rect[pos_index][0] << " >>> " << index_for_rect[pos_index][1] << ">> " << index_for_rect[pos_index][2] << " >>>>" << index_for_rect[pos_index][3] << endl;
                    }
                }

                // cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][0]) << endl;
                // cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][1]) << endl;
                // cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][2]) << endl;
                // cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][3]) << endl;
                // cout << "value : " << matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][0]) + matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][1]) + matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][2]) + matrix_a.at<float>(row_num_in_matrix, index_for_rect[pos_index][3]) << endl;
            }
        }
    }

    vector<cv::Mat> dst;
    vector<cv::Mat> block;
    cv::Size dsize = cv::Size(width / (num_col * 1.0), height / (num_row * 1.0));

    for (int i = 0; i < (num_col * num_row); i++) {
        dst.push_back(cv::Mat::zeros(2, 2, CV_8UC1));
        block.push_back(cv::Mat::zeros(2, 2, CV_8UC1));
    }

    cv::solve(matrix_a, matrix_b, result, cv::DECOMP_SVD);

    cout << "<<<<<<<<<<<<" << endl;
    for (int i = 0; i < num_of_cp; i++) {
        cout << "result matrix: " << i << "  " << result.at<float>(i, 0) << endl;
    }
    cout << "<<<<<<<<<<<<" << endl;

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
    }

    num_of_next_row = 0;

    for (int i = 0; i < num_row * num_col; i++) {
        cv::resize(block[i], dst[i], dsize, 0, 0, cv::INTER_LINEAR);
    }

    for (int i = 0; i < num_row * num_col; i++) {
        dst[i].copyTo(final(cv::Rect((i % num_col) * dst[0].cols, num_of_next_row * dst[0].rows, dst[i].cols, dst[i].rows)));
        //  cout<<" dst["<<i<<"].copyTo(final(cv::Rect(("<<(i % num_col) <<"  * dst[0].cols  "<< num_of_next_row<<"   * dst[0].rows"<< "dst["<<i<<"].cols, dst["<<i<<"].rows)))"<<endl;

        if ((i + 2 + num_of_next_row) % (cp_num_in_row) == 0) {
            num_of_next_row++;
        }
    }

    cp_struct cp;
    grid_struct grid;

    for (int k = 0; k <= num_row; k++) {
        for (int i = 0; i <= num_col; i++) {

            cp.x_coord = rect[0].first + width / num_col * i;
            cp.y_coord = rect[0].second + height / num_row * k;
            cp.z_value = result.at<float>((num_col + 1) * k + i, 0);
            // cout << "value x: " << cp.x_coord << endl;
            // cout << "value y: " << cp.y_coord << endl;
            // cout << "value z: " << cp.z_value << endl;
            grid.point.push_back(cp);
        }
    }

    grid.ref_plane = final;

    final.copyTo(img, mask_img);

    return grid;
}
