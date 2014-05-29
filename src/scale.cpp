// Copyright 2014-4 xtu
#include "include/scale.h"
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <string>
#include "include/RWImage.h"
#include "include/ImageData.h"
#include "include/inpainting.h"
#define GetRValue(rgb) ((U8)(rgb))^0x00000000
#define GetGValue(rgb) ((U8)(((U16)(rgb)) >> 8))^0x00000000
#define GetBValue(rgb) ((U8)((rgb)>>16))^0x00000000
#define RED 0x00ff0000
#define GREEN 0x0000ff00
#define BLUE 0x000000ff
#define BLACK 0x00000000
#define NO_ALPHA 0x00ffffff
#define SCALING 0.618  //  缩放倍数为0.618倍
#define scaling_save_path "opencvsave.bmp"
scale::scale() {
}
scale::scale(std::string oldImageName, ImageData *oldImageData) {
    //  Image = new RWImage(*oldImage);
    ImageName = oldImageName;
    Image_Data = new ImageData(*oldImageData);
}
std::string scale::Scaling(void) {
    //  Image->Save("tmp.bmp");
    IplImage *src = 0;   //  源图像指针
    IplImage *dst = 0;   //  目标图像指针
    IplImage *dst_opencv = 0;   //  目标图像指针
    CvSize dst_cvsize;   //  目标图像尺寸
    src = cvLoadImage(ImageName.c_str());
    dst_cvsize.width = src->width * SCALING;  //  目标图像的宽为源图象宽的scale倍
    dst_cvsize.height = src->height * SCALING;  //  目标图像的高为源图象高的scale倍
    double xx, yy;
    int a, b;
    int rr, gg, bb;
    dst = cvCreateImage(dst_cvsize, src->depth, src->nChannels);  //  构造目标图象
    dst_opencv = cvCreateImage(dst_cvsize, src->depth, src->nChannels);
    double k = SCALING;
    double widthScale = static_cast<double>(1.0 / k);
    double heightScale = static_cast<double>(1.0 / k);
    for (int x = static_cast<int>(k); x<dst_cvsize.width -k ; ++x) {
      for (int y = static_cast<int>(k); y<dst_cvsize.height - k ; ++y) {
         xx = x * widthScale;
         yy = y * heightScale;
         if (xx <= 1e-8) {
           xx = 0;
         }
         if (xx > src->width - 2) {
           xx = static_cast<double>(src->width - 2);
         }
         if (yy <= 1e-8) {
           yy = 0;
         }
         if (yy > src->height - 2) {
           yy = static_cast<double>(src->height - 2);
         }
         a = static_cast<int>(xx);
         b = static_cast<int>(yy);
         //  分别得到对应像素的R、G、B值并用双线性插值得到新像素的R、G、B值
         int r11, r12, r21, r22;
         r11 = GetRValue(Image_Data->GetPixel(b * src->width + a));
         r12 = GetRValue(Image_Data->GetPixel(b * src->width + a + 1));
         r21 = GetRValue(Image_Data->GetPixel((b + 1) * src->width + a));
         r22 = GetRValue(Image_Data->GetPixel((b + 1) * src->width + a + 1));
         rr = static_cast<int>(r11 * (a + 1 - xx) * (b + 1 - yy) + r12 * (a + 1 - xx) * (yy - b) + r21 * (xx - a) * (b + 1 - yy) + r22 *(xx - a) * (yy - b));
         int b11, b12, b21, b22;
         b11 = GetBValue(Image_Data->GetPixel(b * src->width + a));
         b12 = GetBValue(Image_Data->GetPixel(b * src->width + a + 1));
         b21 = GetBValue(Image_Data->GetPixel((b + 1) * src->width + a));
         b22 = GetBValue(Image_Data->GetPixel((b + 1) * src->width + a + 1));
         bb = static_cast<int>(b11 * (a + 1 - xx) * (b + 1 - yy) + b12 * (a + 1 - xx) * (yy - b) + b21 * (xx - a) * (b + 1 - yy) + b22 * (xx - a) * (yy - b));

         int g11, g12, g21, g22;
         g11 = GetGValue(Image_Data->GetPixel(b * src->width + a));
         g12 = GetGValue(Image_Data->GetPixel(b * src->width + a + 1));
         g21 = GetGValue(Image_Data->GetPixel((b + 1) * src->width + a));
         g22 = GetGValue(Image_Data->GetPixel((b + 1) * src->width + a + 1));
         gg = static_cast<int>(g11 * (a + 1 - xx) * (b + 1 - yy) + g12 * (a + 1 - xx) *(yy - b) +g21 * (xx - a) * (b + 1 - yy) + g22 *(xx - a) * (yy - b));
         if ((g11 == 255 && r11 == 0 && b11 ==0) ||(g12 == 255 && r12 == 0 && b12 ==0) || (g21 == 255 && r21 == 0 && b21 ==0) || (g22 == 255 && r22 == 0 && b22 ==0)) {
           (reinterpret_cast<uchar*>(dst->imageData + dst->widthStep*y))[x * 3 + 2] = 0;
           (reinterpret_cast<uchar*>(dst->imageData + dst->widthStep*y))[x * 3 + 1] = 255;
           (reinterpret_cast<uchar*>(dst->imageData + dst->widthStep*y))[x * 3] = 0;
           continue;
         }
             //  将得到的新R、G、B值写到新图像中
         (reinterpret_cast<uchar*>(dst->imageData + dst->widthStep * y))[x * 3 + 2] = MIN(255, bb);
         (reinterpret_cast<uchar*>(dst->imageData + dst->widthStep * y))[x * 3 + 1] = MIN(255, gg);
         (reinterpret_cast<uchar*>(dst->imageData + dst->widthStep * y))[x * 3] = MIN(255, rr);
              }
          }
      cvResize(src, dst_opencv, CV_INTER_LINEAR);  //  缩放源图像到目标图像
      //  覆盖掉openc得到的修补区域
      for (int x = 0; x < dst_cvsize.width; ++x) {
         for (int y = 0; y < dst_cvsize.height; ++y) {
            if ((reinterpret_cast<uchar*>(dst->imageData + dst->widthStep * y))[x * 3 + 1] == 255 && (reinterpret_cast<uchar*>(dst->imageData + dst->widthStep * y))[x * 3] == 0 && (reinterpret_cast<uchar*>(dst->imageData + dst->widthStep * y))[x * 3 + 2] == 0) {
              (reinterpret_cast<uchar*>(dst_opencv->imageData + dst_opencv->widthStep * y))[x * 3 + 2] = 0;
              (reinterpret_cast<uchar*>(dst_opencv->imageData + dst_opencv->widthStep * y))[x * 3 + 1] = 255;
              (reinterpret_cast<uchar*>(dst_opencv->imageData + dst_opencv->widthStep * y))[x * 3] = 0;
            }
        }
      }
    cvSaveImage(scaling_save_path, dst_opencv);
    std::string returnString = scaling_save_path;
    return returnString;
}
scale::~scale() {
    delete Image_Data;
}
