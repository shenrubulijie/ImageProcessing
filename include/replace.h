// Copyright 2014-4 xtu
#ifndef INCLUDE_REPLACE_H_
#define INCLUDE_REPLACE_H_
#include<string>
#include "include/ImageData.h"
#include "include/RWImage.h"
typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned int   U32;
typedef unsigned int COLORREF;
class replacePic{
 public:
  ImageData *Image_data;
  replacePic();
  ~replacePic();
  replacePic(ImageData *oldImageData , std::string fileName);
  std::string Copy(ImageData *resizeImageData);

 private:
  std::string fileName;
  int m_width;
  int m_height;
  int * m_mark;
  double * m_r;
  double * m_g;
  double * m_b;
  int m_top, m_bottom, m_left, m_right;
  void DrawBoundary();
};

#endif  // INCLUDE_REPLACE_H_
