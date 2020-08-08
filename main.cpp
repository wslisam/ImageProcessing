#include "processing.h"

int main(int argc, char** argv)
{
    // cv::Mat img = cv::imread("./Surface_Fitting_image/colorplane/test.bmp");
    // cv::Mat mask = cv::imread("./Surface_Fitting_image/colorplane/mask.bmp");
    cv::Mat img = cv::imread("./Surface_Fitting_image/1/Input3.bmp");
    cv::Mat mask = cv::imread("./Surface_Fitting_image/1/mask.bmp");

    cv::Mat fit_img;

    if (img.empty()) {
        cout << "!!!!! Failed imread(): image not found" << endl;
        return -1;
    }
    // cv::Mat test2 = color_bilinear(img);
    // cv::Mat test3 = color_bilinear_buildin(img);
    // cv::imshow("Bilinear Interpolation2", test2);
    // cv::imshow("Bilinear Interpolation3", test3);

    // change the img to grayscale first
    img = BGR2GRAY(img);

    cv::Mat test2 = gray_bilinear(img);
    cv::Mat test3 = gray_bilinear_buildin(img);

    // inverse the mask
    cv::Mat out_mask = ~mask;
    out_mask = BGR2GRAY(out_mask);

    cv::Mat after_filter = filter(img, out_mask);

    // int num_objects = find_num_obj_using_ConnectedComponents(after_filter);
    // int num_objects = find_num_obj_using_contours(after_filter);

    int G_size_x = 0, G_size_y = 0;
    int num_region;
    // cv::Mat after_planefit = segmentation(after_filter.clone(), out_mask,
    // &G_size_x, &G_size_y , 100);//50,40,55-59 cv::Mat after_seg_planefit =
    // gen2_segmentation(after_filter.clone(), out_mask, &G_size_x, &G_size_y, 2,
    // &num_region); //50,40,55-59 cv::Mat hello =
    // cal_and_cut(after_filter.clone(),out_mask,10);
    cv::Mat final = gen2_planefit(after_filter.clone(), out_mask);

    // cout << "G_x: " << G_size_x << " y: " << G_size_y << endl;
    // int after_planefit = cal_and_cut(after_filter, out_mask,5);

    // cv::imshow("sample ", img);
    // cv::imshow("mask after inverse", out_mask);
    cv::imshow("after filter", after_filter);

    // cv::Mat test = gray_bilinear(after_filter);
    // cv::imshow("Bilinear Interpolation", test);

    // cv::imshow("planefit", after_seg_planefit);
    cv::imshow("general_planefit", final);
    // imwrite( "./images/test/test2_filter_.bmp", after_filter );
    // imwrite( "./images/test/test2_fit_5_1.bmp", final );

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
