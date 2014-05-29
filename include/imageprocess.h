// Copyright 2014-4 xtu
#ifndef INCLUDE_IMAGEPROCESS_H_
#define INCLUDE_IMAGEPROCESS_H_
#include <string>
#include "include/RWImage.h"
#include "include/ImageData.h"
#include "include/Sketch.h"
class imageProcess{
 public:
    RWImage* m_image;
    int m_image_width;
    int m_image_height;
    //  ImageData* m_image_data;
    imageProcess();
    ~imageProcess();
    void LoadAndpProcessImage(const char* filename , const char* filename2);
 private:
    void Amplify(std::string);
};

#endif  // INCLUDE_IMAGEPROCESS_H_
