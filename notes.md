typedef unsigned char uchar;

```cpp
#define CHAR_BIT      8         /* number of bits in a char */
#define SCHAR_MIN   (-128)      /* minimum signed char value */
#define SCHAR_MAX     127       /* maximum signed char value */
#define UCHAR_MAX     0xff      /* maximum unsigned char value */
```

Img_mat is a 3 channeled image. Each channel consists of pixel values uchar in data type. So with split(Img_mat, BGR) the Img_mat is split into 3 planes of blue, green and red which are collectively stored in a vector BGR. So BGR[0] is the first (blue) plane with uchar data type pixels...hence it will be

```cpp
int dataB = (int)BGR[0].at<uchar>(i,j);
int dataG = (int)BGR[1].at<uchar>(i,j);
```

cv :: Mat :: ptr
Imagej 2 (https://fiji.sc/)

CV\_<bit_depth>(S|U|F)C<number_of_channels>

```cpp
1--bit_depth---代表8bit,16bites,32bites,64bites---比如说,
        如果你现在创建了一个存储--灰度图片的Mat对象,这个图像的大小为宽100,高100,那么,现在这张
        灰度图片中有10000个像素点，它每一个像素点在内存空间所占的空间大小是8bite,8位--所以它对
        应的就是CV_8
2--S|U|F--S--代表---signed int
               U--代表--unsigned int
               F--代表--float
3--C<number_of_channels>----代表--number of channel,example:
         1--灰度图片--grayImg----single channel
         2--RGB_color_image----------three channel
         3--with Alph channel的 RGB image--four channel
```

```cpp
// CvMat *res  = cvCreateMat(3, 1, CV_32FC1);
// CvMat *matX = cvCreateMat(10000, 3, CV_32FC1);
// CvMat *matZ = cvCreateMat(10000, 1, CV_32FC1);

```

Automated Optical Inspection (AOI)

CMake uses configuration files called CMakeLists.txt, where the compilation and
dependency processes are defined. For a basic project, based on an executable build
from one source code file, a two-line CMakeLists.txt file is all that is needed. The
file looks like this:

```
cmake_minimum_required (VERSION 2.6)
project (CMakeTest)
add_executable(${PROJECT_NAME} main.cpp)
```

The first line defines the minimum version of CMake required. This line is
mandatory in our CMakeLists.txt file and allows you to use the cmake functionality
defined from a given version defined in the second line; it defines the project name.
This name is saved in a variable called PROJECT_NAME.

The last line creates an executable command (add_executable()) in the main.cpp
file, gives it the same name as our project (${PROJECT_NAME}), and compiles our
source code into an executable called CMakeTest, which we set as the project name.
The ${} expression allows access to any variable defined in our environment.
Then, we can use the \${PROJECT_NAME} variable as an executable output name.

image watch opencv

#include_directories("/Users/wsli1998/Documents/ImageProcessing")

四角既 value 同 bcd 無關 ab line 同 cv line 無關

---

---

          [0,0,0,0,0,0]
          min square error

no need random subsampling, uniform plane fit

cut 格 number 排列

uchar\* data = image.ptr<uchar>(i);//get the address of row i;

shift command 4 get the coord

```cpp
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;

int main()
{

    Mat src = Mat::zeros( 400, 400, CV_8UC3);

    Rect r(100,100,50,50);

    Point point0 = Point(r.x, r.y);
    Point point1 = Point(r.x + r.width, r.y);
    Point point2 = Point(r.x + r.width, r.y + r.height);
    Point point3 = Point(r.x, r.y + r.height);

    rectangle( src, r, Scalar::all(255));

    circle( src, point0, 10, Scalar( 0, 0, 255) );
    circle( src, point1, 10, Scalar( 0, 0, 255) );
    circle( src, point2, 10, Scalar( 0, 0, 255) );
    circle( src, point3, 10, Scalar( 0, 0, 255) );

    imshow( "coodinates of all corner of rectangle", src );
    waitKey();

    return 0;
}
```

```cpp
// 定義外接矩形 std::vector<Rect> boundRect(contours.size()); for (int i =0;i<contours.size();i++) { // 獲取最小外接矩形 boundRect[i] = boundingRect(contours[i]); // 在原圖像上繪製最小外接矩形 rectangle(src, boundRect[i], Scalar(0, 255, 0)); }

```

```cpp
cv::Rect rect = boundingRect(contours[i]); //  次次都右至左  下至上
rectangle(img, rect, cv::Scalar(255, 0, 0), 0);
cout << " Rect " << i << endl;
cout << "pt 0  :" << rect.x << " , " << rect.y << endl;
cout << "pt 1  :" << rect.x + rect.width << " , " << rect.y << endl;
cout << "pt 2  :" << rect.x + rect.width << " , " << rect.y + rect.height << endl;
cout << "pt 3  :" << rect.x << " , " << rect.y + rect.height << endl;
        cv::imwrite("ROI.bmp",input_roi);
```

```cpp
void rect_contours(cv::Mat img, vector<vector<cv::Point>> contours)
{

	//    vector<cv::Point> ctr = contours.at(3);
	//    for (int i = 0; i < ctr.size(); i++) {
	//     // cv::Point coordinate_i_ofcontour = ctr.size();
	//     cout << endl << "contour with coordinates: x = " << ctr[i].x << " y = " << ctr[i].y;
	// }
}
```

```cpp
cout << "Rect " << i << endl;
cout << "Point 0  :" << boundRect[i].x << " , " << boundRect[i].y << endl;
cout << "Point 1  :" << boundRect[i].x + boundRect[i].width << " , " << boundRect[i].y << endl;
cout << "Point 2  :" << boundRect[i].x << " , " << boundRect[i].y + boundRect[i].height << endl;
cout << "Point 3  :" << boundRect[i].x + boundRect[i].width << " , " << boundRect[i].y + boundRect[i].height << endl;
```

```cpp
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
```

```cpp
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

```

```cpp
	// ver1
					// M_B.at(seg).at(num_of_sample[seg]) = pixel_val;
					//   cout<<M_B[seg].at(num_of_sample[seg])<<endl;
					// cout<<M_B.at(seg).at(num_of_sample[seg])<<endl;

					// ver2
					// vector<int> V;
					// V.push_back(pixel_val);
					// M_B.at(seg).push_back(V);
					// M_B.push_back(M_B[seg]);

					// ver3
					// M_B.at(seg).push_back(pixel_val);
					// M_B.push_back(M_B.at(seg));
    
```

```cpp
	// check the src type
	// cout << typeid(matrix_b).name() << endl;
	// cout << typeid(matrix_a).name() << endl;
	// cout << typeid(result).name() << endl;
	// cout << (matrix_a.type() == CV_32F) << endl;
```