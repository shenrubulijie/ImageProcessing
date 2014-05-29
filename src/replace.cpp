// Copyright 2014-4 xtu
#include "include/replace.h"
#include<string>
#include "include/RWImage.h"
#include "include/ImageData.h"
#define SOURCE 0
#define TARGET -1
#define BOUNDARY -2
#define GET_R_VALUE(rgb) static_cast<U8>(rgb)
#define GET_G_VALUE(rgb) static_cast<U8>((static_cast<U16>(rgb)) >> 8)
#define GET_B_VALUE(rgb) static_cast<U8>((rgb) >> 16)
#define RED 0xffff0000
#define GREEN 0xff00ff00
#define BLUE 0xff0000ff
#define BLACK 0xff000000
#define NO_ALPHA 0x00ffffff
replacePic::replacePic() {
}
replacePic::replacePic(ImageData *oldImageData, std::string file) {
    fileName = file;
    //  Image = new RWImage(*oldImage);//oldImage;
    Image_data = oldImageData;
    m_width = Image_data->GetWidth();
    m_height = Image_data->GetHeight();
    m_mark = new int[m_width * m_height];
    m_r = new double[m_width * m_height];
    m_g = new double[m_width * m_height];
    m_b = new double[m_width * m_height];
    m_top = m_height;
    m_bottom = 0;
    m_left = m_width;
    m_right = 0;
    int temp;
    int x, y;
    for (y = 0; y < m_height; ++y) {
      for (x = 0; x < m_width; ++x) {
        temp = Image_data->GetPixel(y * m_width + x);
        m_r[y * m_width + x] = GET_R_VALUE(temp);
        m_g[y * m_width + x] = GET_G_VALUE(temp);
        m_b[y * m_width + x] = GET_B_VALUE(temp);
      }
    }
}
std::string replacePic::Copy(ImageData *resizeImageData) {
    DrawBoundary();
    for (int y = 0; y < m_height; ++y) {
      for (int x = 0; x < m_width; ++x) {
          int xx = x;
          int yy = y;
          if (xx >= resizeImageData->GetWidth()) {
              xx =resizeImageData->GetWidth()-1;
          }
          if (yy >= resizeImageData->GetHeight()) {
              yy =resizeImageData->GetHeight()-1;
          }
          if (m_mark[y * m_width + x] == TARGET || m_mark[y * m_width + x] == BOUNDARY) {
          //  cout<<resizeImageData->GetPixel(yy * resizeImageData->GetWidth() + xx)<<endl;
          Image_data->SetPixel(y * m_width + x, resizeImageData->GetPixel(yy * resizeImageData->GetWidth() + xx));
        }
       }
     }
     std::string lastFile = fileName.append("last.bmp");
     // cout<<lastFile<<endl;
     // char *inpainting_result;
     // int len = lastFile.length();
     // inpainting_result = (char *)malloc((len+1)*sizeof(char));
     // lastFile.copy(inpainting_result,len,0);
     // cout<<inpainting_result<<endl;
     // Image->Save(lastFile.c_str());
     return lastFile.c_str();
}

void replacePic::DrawBoundary(void) {
  int color;
  for (int y = 0; y < m_height; ++y) {
    for (int x = 0; x < m_width; ++x) {
     color = Image_data->GetPixel(y * m_width + x);
     //  cout<<"color = "<<hex<<color<<endl;
      if (color == GREEN || color == 0x0000ff00) {
        // cout<<"GREEN = "<<y <<" "<< x<<endl;
        m_mark[y * m_width + x] = TARGET;
      } else {
       m_mark[y * m_width + x] = SOURCE;
      }
    }
  }
  for (int j= 0; j < m_height; ++j)  {
    for (int i = 0; i < m_width; ++i)  {
      if (m_mark[j * m_width + i] == TARGET) {
        if (i < m_left) m_left = i;
        if (i > m_right) m_right = i;
        if (j > m_bottom) m_bottom = j;
        if (j < m_top) m_top = j;
        if (j == m_height-1 ||
           j == 0 ||
           i == 0 ||
           i == m_width - 1 ||
           m_mark[(j - 1) * m_width + i] == SOURCE ||
           m_mark[j * m_width + i - 1] == SOURCE ||
           m_mark[j * m_width + i + 1] == SOURCE ||
           m_mark[(j + 1) * m_width + i] == SOURCE) {
             m_mark[j * m_width + i] = BOUNDARY;
        }
      }
    }
  }
}
replacePic::~replacePic() {
}
