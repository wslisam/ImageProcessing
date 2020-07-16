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
Then, we can use the ${PROJECT_NAME} variable as an executable output name.

image watch opencv


#include_directories("/Users/wsli1998/Documents/ImageProcessing")