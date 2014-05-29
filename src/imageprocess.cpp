// Copyright 2014-4 xtu
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <stdio.h>
#include <string>
#include "include/RWImage.h"
#include "include/ImageData.h"
#include "include/imageprocess.h"
#include "include/inpainting.h"
#include "include/scale.h"
#include "include/replace.h"

#define SCALING 0.618  //  缩放倍数为0.618倍
#define GREEN 0x0000ff00
#define IMAGE_POS_X 0
#define IMAGE_POS_Y 33
#define SCRIBBLE_SIZE 5
#define SCRIBBLE_SIZE_INC 50
#define FILENAME "pix.bmp"
#define AMPLIFYFILENAME "amplify.bmp"
imageProcess::imageProcess() {
}
void imageProcess::LoadAndpProcessImage(const char* filename, const char* filename2) {
  if (filename == NULL) {
    printf("error: no file name\n");
  }
sketch *origin_sk = new sketch();
origin_sk->Edge(filename2);
ImageData origin_green_pic;
RWImage origin_green_pic_rw(filename, &origin_green_pic);
inpainting origin_inpaint(&origin_green_pic);
origin_green_pic_rw.Save("edge_inpaint5.bmp");
ImageData origin_canny_pic;
RWImage origin_canny_pic_rw("canny.bmp", &origin_canny_pic);
origin_green_pic_rw.Save("edge_inpaint4.bmp");
 std::cout << "draw1" << std::endl;
 origin_inpaint.Convert2Gray();
origin_inpaint.DrawBoundary();
origin_green_pic_rw.Save("edge_inpaint3.bmp");
std::cout << "draw2" << std::endl;
origin_inpaint.DrawEdge(&origin_canny_pic);
origin_green_pic_rw.Save("edge_inpaint2.bmp");
std::cout << "draw3" << std::endl;
origin_inpaint.ConnectEdge();
origin_green_pic_rw.Save("edge_inpaint.bmp");
//////////////////////////////////
  ImageData m_image_data;
  m_image = new RWImage(filename, &m_image_data);
  m_image_width = m_image_data.GetWidth();
  m_image_height = m_image_data.GetHeight();
  //  save as tmp.bmp for opencv scale process
  m_image->Save("tmp.bmp");
  scale scaling("tmp.bmp", &m_image_data);
  std::string scaling_save_path = scaling.Scaling();
  //  use the opencv scaling result for inpainting
  const  char *scaling_save_path_char = scaling_save_path.c_str();
  ImageData inpaint_Data;
  RWImage inpaint_Data_rw(scaling_save_path_char, &inpaint_Data);
  inpainting test(&inpaint_Data);
  std::string inpainting_result = test.Process();
  inpaint_Data_rw.Save(inpainting_result.c_str());
  std::cout << "amplify" << std::endl;
  //  amplify the inpainting result to original size
  Amplify(inpainting_result);
  std::cout << "amplifyend" << std::endl;
  ImageData lastImageData;
  RWImage* lastImage= new RWImage(AMPLIFYFILENAME, &lastImageData);
  std::string fileName = filename;
  fileName = fileName.substr(0, fileName.length()-4);
  std::cout << "replace" << filename << std::endl;
  replacePic test2(&m_image_data, fileName);
  //  replace the green part
  std::string replaceResult = test2.Copy(&lastImageData);
  m_image->Save(replaceResult.c_str());
}
void imageProcess::Amplify(std::string inpainting_result_string) {
    char *inpainting_result;
    int len = inpainting_result_string.length();
    inpainting_result =reinterpret_cast<char *>(malloc((len+1)*sizeof(char)));
    inpainting_result_string.copy(inpainting_result, len, 0);
    //  将修复完的结果放大回原来的大小
    CvSize dst_cvsize;   //  目标图像尺寸
    IplImage *src2 = 0;
    IplImage *dst2 = 0;
    double scale2 = 1.0/SCALING;
    std::cout << "amplify 1" << inpainting_result << std::endl;
    src2 = cvLoadImage(inpainting_result_string.c_str());
    dst_cvsize.width = src2->width * scale2;
    dst_cvsize.height = src2->height * scale2;
    std::cout << "amplify 3" << std::endl;
    dst2 = cvCreateImage(dst_cvsize, src2->depth, src2->nChannels);
    cvResize(src2, dst2, CV_INTER_LINEAR);
    cvSaveImage(AMPLIFYFILENAME, dst2);
    std::cout << "amplify end" << std::endl;
}
imageProcess::~imageProcess() {
  delete m_image;
}
