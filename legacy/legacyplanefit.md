```
cv::Mat LR_planefit(cv::Mat contour_region, cv::Mat mask_region, int sample_size, int num_of_sample); // 1x2
cv::Mat TDLR_planefit(cv::Mat contour_region, cv::Mat mask_region, int sample_size, int num_of_sample); // 2x2
cv::Mat TMDLMR_planefit(cv::Mat contour_region, cv::Mat mask_region, int sample_size, int num_of_sample); // 3x3
```

```
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

```

```
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
```
