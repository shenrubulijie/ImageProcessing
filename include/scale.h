// Copyright 2014-4 xtu
#ifndef INCLUDE_SCALE_H_
#define INCLUDE_SCALE_H_
#include <string>
#include "include/RWImage.h"
#include "include/ImageData.h"
class scale {
 public:
  scale();
  scale(std::string Image, ImageData *Image_data);
  std::string Scaling(void);
  ~scale();
 private:
  std::string ImageName;
  ImageData *Image_Data;
};

#endif  // INCLUDE_SCALE_H_
