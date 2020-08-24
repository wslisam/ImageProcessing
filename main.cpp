#include "processing.h"

int main(int argc, char** argv)
{
    cv::Mat img = cv::imread("./Surface_Fitting_image/6/Input3.bmp"); //source img
    cv::Mat mask = cv::imread("./Surface_Fitting_image/6/mask.bmp"); //mask of source img

    if (img.empty()) { // warning if cannot read the file
        cout << "!! Failed imread(): image not found !!" << endl;
        return -1;
    }

    img = BGR2GRAY(img); // change the img to grayscale first
    // cv::imwrite("./images/new/8_gray.bmp", img);

    cv::Mat out_mask = BGR2GRAY(~mask); // inverse the mask and RGB to grayscale image
    cv::Mat after_filter = filter(img, out_mask); // masking
    final_struct ref_img = planefit(after_filter.clone(), out_mask, 3, 3); //gen4

    // cv::imshow("input image", img);
    // cv::imshow("image after filter", after_filter);
    cv::imshow("Ref_planefit_result_image", ref_img.whole_plane);
    // Diff(after_filter,ref_img.whole_plane);
    // sub(after_filter,ref_img.whole_plane);
    ref_img.dark = dark(after_filter.clone(), ref_img.whole_plane.clone(), 12);
    // find_defects_using_contours(ref_img.dark, ref_img.area);
    find_defects_using_ConnectedComponents(ref_img.dark, ref_img.area);

    // ref_img.bright = bright(after_filter, ref_img.whole_plane, 15);
    // find_defects_using_contours(ref_img.bright, ref_img.area));

    // cv::imwrite("./images/new/8_dark.bmp", ref_img.dark);
    // cv::imwrite("./images/new/4_bright.bmp", ref_img.bright);

    // cv::imwrite("./images/new/8_ref.bmp", ref_img.whole_plane);

    // ref_img.dark = dark_otsu(after_filter, ref_img.whole_plane);
    // ref_img.bright = bright_otsu(after_filter, ref_img.whole_plane);

    // cv::imwrite("./images/new/2by3_filter.bmp", after_filter);
    // cv::imwrite("./images/new/2by3_result.bmp", ref_img.whole_plane);

    cv::waitKey(0);

    cv::destroyAllWindows();

    return 0;
}

//defect area / rect area 滿唔滿   area ratio , defect 特性   , center  列表  , defect info / point住 /  cropp 佢出黎 (rect )
// width height ratio -> point line defect

//HSV colour domain defect
//set colour range   ／／ HSV  -> v 決定光定暗

//save point into a vector