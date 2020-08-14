#include "processing.h"

int main(int argc, char** argv)
{
    cv::Mat img = cv::imread("./Surface_Fitting_image/1/Input3.bmp"); //source img
    cv::Mat mask = cv::imread("./Surface_Fitting_image/1/mask.bmp"); //mask of source img

    if (img.empty()) { // warning if cannot read the file
        cout << "!! Failed imread(): image not found !!" << endl;
        return -1;
    }

    img = BGR2GRAY(img); // change the img to grayscale first

    cv::Mat out_mask = BGR2GRAY(~mask); // inverse the mask and RGB to grayscale image
    cv::Mat after_filter = filter(img, out_mask); // masking
    final_struct ref_img = planefit(after_filter.clone(), out_mask, 1, 2); //gen2

   
    // cv::imshow("input image", img);
    cv::imshow("image after filter", after_filter);
    cv::imshow("Ref_planefit_result_image", ref_img.whole_plane);
    Diff(after_filter,ref_img.whole_plane);

    // cv::imwrite("./images/test/filter.bmp", after_filter);
    // cv::imwrite("./images/test/result.bmp", ref_img);

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
