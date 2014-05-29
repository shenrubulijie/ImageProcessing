// Copyright 2014-4 xtu
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <string>
#include <algorithm>
#include "include/inpainting.h"
#include "include/RWImage.h"
#include "include/ImageData.h"
#include "include/imageprocess.h"
#define PAINT_COLOR  RGB(0, 255, 0)
#define SOURCE 0
#define TARGET -1
#define BOUNDARY -2
#define myWinSize 2
#define save_path "result"
#define GET_R_VALUE(rgb) static_cast<U8>(rgb)
#define GET_G_VALUE(rgb) static_cast<U8>((static_cast<U16>(rgb)) >> 8)
#define GET_B_VALUE(rgb) static_cast<U8>((rgb)>>16)
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define RGB(r, g, b) (static_cast<int>(static_cast<U8>(r)|(static_cast<U16>(static_cast<U8>(g)) << 8)|(static_cast<U32>(static_cast<U8>(b)) << 16)))
#define RED 0xff0000
#define GREEN 0x00ff00
#define BLUE 0x0000ff
#define MARK 0x00ffff
#define MARK2 0xffff00
#define LOOKUP 20
/*inpainting::inpainting(const char *name) {
  Image_data = new ImageData();
  Image = new RWImage(name,Image_data);
  //bOpen = Image->load(name);
  m_width = Image_data->GetWidth();
  m_height = Image_data->GetHeight();
  m_mark = new int[m_width * m_height];
  m_confid = new double[m_width * m_height];
  memset( m_confid, 0, m_width * m_height * sizeof(double) );
  m_pri = new double[m_width * m_height];
  m_gray  = new double[m_width * m_height];
  m_source = new bool[m_width * m_height];
  m_color = new int[m_width * m_height];
  m_r = new double[m_width * m_height];
  m_g = new double[m_width * m_height];
  m_b = new double[m_width * m_height];
  m_top = m_height;
  m_bottom = 0;
  m_left = m_width;
  m_right = 0;
  int temp;

  boundary_top = m_height;
  boundary_bottom = 0;
  boundary_left = m_width;
  boundary_right = 0;
  int x,y;
  for(y = 0; y < m_height; ++y) {
    for(x = 0; x < m_width; ++x) {
      temp = Image_data->GetPixel(y * m_width + x);
      m_color[y * m_width + x] = temp;
      m_r[y * m_width + x] = GET_R_VALUE(temp);
      m_g[y * m_width + x] = GET_G_VALUE(temp);
      m_b[y * m_width + x] = GET_B_VALUE(temp);
    }
  }
}*/
inpainting::inpainting(ImageData *oldImageData) {
  //  Image = new RWImage(*oldImage);//oldImage;
  Image_data =  oldImageData;
  m_width = Image_data->GetWidth();
  m_height = Image_data->GetHeight();
  m_mark = new int[m_width * m_height];
  m_confid = new double[m_width * m_height];
  memset(m_confid, 0, m_width * m_height * sizeof(double));
  m_pri = new double[m_width * m_height];
  m_gray  = new double[m_width * m_height];
  m_source = new bool[m_width * m_height];
  m_color = new int[m_width * m_height];
  m_r = new double[m_width * m_height];
  m_g = new double[m_width * m_height];
  m_b = new double[m_width * m_height];
  m_edge_mark = new int[m_width * m_height];
  m_top = m_height;
  m_bottom = 0;
  m_left = m_width;
  m_right = 0;
  int temp;

  boundary_top = m_height;
  boundary_bottom = 0;
  boundary_left = m_width;
  boundary_right = 0;
  int x, y;
  for (y = 0; y < m_height; ++y) {
    for (x = 0; x < m_width; ++x) {
      temp = Image_data->GetPixel(y * m_width + x);
      m_color[y * m_width + x] = temp;
      m_r[y * m_width + x] = GET_R_VALUE(temp);
      m_g[y * m_width + x] = GET_G_VALUE(temp);
      m_b[y * m_width + x] = GET_B_VALUE(temp);
    }
  }
}
std::string inpainting::Process(void) {
  char path[200];
  char temp[30];
  pix_to_inpaint = 0;
  pix_been_inpainted = 0;
  Convert2Gray();
  DrawBoundary();
  DrawSource();
  memset(m_pri, 0, m_width*m_height*sizeof(double));
  for (int j= m_top; j <= m_bottom; ++j) {
      for (int i = m_left; i <= m_right; ++i) {
        if (m_mark[j * m_width + i] == BOUNDARY) {
          m_pri[j * m_width + i] = ComputePriority(i, j);
        }
    }
  }
  int count = 0;
  count_pic =1;
  int temp_color = 0;
  int temp_color2 = 0;
  int *temp_colors = new int[LOOKUP];
  int *temp_colors_2 = new int[LOOKUP];
  int **most_similar_patch = new int*[LOOKUP];
  for (int o = 0; o < LOOKUP; ++o) {
      most_similar_patch[o] = new int[2];
  }
  int **most_similar_patch_2 = new int*[LOOKUP];
  for (int o = 0; o < LOOKUP; ++o) {
      most_similar_patch_2[o] = new int[2];
  }
  int start = clock();
  int *colum_in_height = new int[m_height];
  for (int i = 0; i < m_height; ++i) {
    colum_in_height[i] = 0;
  }
  int *colum_in_width = new int[m_width];
  for (int i = 0; i < m_width; ++i) {
    colum_in_width[i] = 0;
  }
  double max_pri = 0;
  int pri_x, pri_y;
  int patch_x, patch_y;
  while (TargetExist()) {
    count++;
    count_pic++;
    max_pri = 0;
    for (int j= 0; j < m_height; ++j) {
      for (int i = 0; i < m_width; ++i) {
        colum_in_height[j]++;
        colum_in_width[i]++;
        if (m_mark[j * m_width + i] == BOUNDARY && m_pri[j * m_width + i] > max_pri) {
            pri_x = i;
            pri_y = j;
            max_pri = m_pri[j * m_width + i];
            colum_in_height[j]--;
            colum_in_width[i]--;
        }
        if (m_mark[j * m_width + i] == TARGET) {
            colum_in_height[j]--;
            colum_in_width[i]--;
        }
      }
    }
  height_diff = m_bottom - m_top + 1;
  width_diff = m_right - m_left + 1;
  //  缩小搜索框
  for (int i = m_top; i <= m_bottom; ++i) {
    if (colum_in_height[i] == width_diff) {
      m_top++;
    } else {
      break;
    }
  }
  for (int i = m_bottom-1; i >= m_top; --i) {
     if (colum_in_height[i] == width_diff) {
       m_bottom--;
     } else {
       break;
     }
  }
  for (int i = m_left; i <= m_right; ++i) {
    if (colum_in_width[i] == height_diff) {
      m_left++;
    } else {
      break;
    }
  }
  for (int i = m_right; i >= m_left; --i) {
    if (colum_in_width[i] == height_diff) {
      m_right--;
    } else {
      break;
    }
  }
    PatchTexture(pri_x, pri_y, patch_x, patch_y, most_similar_patch, most_similar_patch_2);
    FillTarget(pri_x, pri_y, patch_x, patch_y, ComputeConfidence(pri_x, pri_y));
    UpdateBoundary(pri_x, pri_y);
    UpdatePriority(pri_x, pri_y);
    char str[10];
    sprintf(str, "%d", count);
    strcpy(path, save_path);
    strcat(path, str);
    strcat(path, ".bmp");
    //  cout<<" path = "<<path<<endl;
    //  Image -> save(path);

    //  下面的是为了画出最匹配的20个点的图片的中间步骤
    /*temp_color = Image->pixel(pri_x,pri_y);
    temp_color2 = Image->pixel(patch_x,patch_y);
    for(int o=0;o<LOOKUP;++o) {
        temp_colors[o]=Image->pixel(most_similar_patch[o][0],most_similar_patch[o][1]);
       // temp_colors_2[o]=Image->pixel(most_similar_patch_2[o][0],most_similar_patch_2[o][1]);
        Image -> setPixel(most_similar_patch[o][0],most_similar_patch[o][1],MARK);
       // Image -> setPixel(most_similar_patch_2[o][0],most_similar_patch_2[o][1],MARK2);

    }
    Image -> setPixel(pri_x,pri_y,RED);
    Image -> setPixel(patch_x,patch_y,BLUE);
    //cout<<" patch_x = "<<patch_x<<endl;
    //cout<<" patch_y = "<<patch_y<<endl;
    Image -> save(path);

    for(int o=0;o<LOOKUP;++o) {
        Image -> setPixel(most_similar_patch[o][0],most_similar_patch[o][1],temp_colors[o]);
      //  Image -> setPixel(most_similar_patch_2[o][0],most_similar_patch_2[o][1],temp_colors_2[o]);

    }
    Image -> setPixel(pri_x,pri_y,temp_color);
    Image -> setPixel(patch_x,patch_y,temp_color2);*/
  }
  int end = clock();
  strcpy(path, save_path);
  strcat(path, ".bmp");
  //  Image->Save(path);
  std::string res = path;
  return res;
}
double inpainting::ComputePriority(int i, int j) {
  double confidence, data;
  confidence = ComputeConfidence(i, j);
  data = ComputeData(i, j);
  double res = (0.35 + 0.6 * pix_been_inpainted / pix_to_inpaint + 0.01) * (0.7 + 0.3 * confidence - 0.6 * pix_been_inpainted / pix_to_inpaint - 0.01) + 0.65 * data;
  return res;
  //  return confidence*data;
}

double inpainting::ComputeConfidence(int i, int j) {
  double confidence = 0;
  for (int y = MAX(j - myWinSize, 0); y <= MIN(j + myWinSize, m_height - 1); ++y) {
    for (int x = MAX(i - myWinSize, 0); x <= MIN(i + myWinSize, m_width - 1); ++x) {
      confidence += m_confid[y * m_width + x];
    }
  }
  confidence /= (myWinSize * 2 + 1) * (myWinSize * 2 + 1);
  return confidence;
}
double inpainting::ComputeData(int i, int j) {
  gradient grad, temp, grad_T;
  grad.grad_x = 0;
  grad.grad_y = 0;
  double result;
  double magnitude;
  double max = 0;
  int x, y;
  for (y = MAX(j - myWinSize, 0); y <= MIN(j + myWinSize, m_height - 1); ++y) {
    for (x = MAX(i - myWinSize, 0); x <= MIN(i + myWinSize, m_width - 1); ++x) {
      if (m_mark[y * m_width + x] >=0) {
        if (m_mark[y * m_width + x + 1] < 0 ||
           m_mark[y * m_width + x - 1] < 0 ||
           m_mark[(y + 1) * m_width + x] < 0 ||
           m_mark[(y - 1) * m_width + x] < 0) continue;
        temp = GetGradient(x, y);
        magnitude = temp.grad_x * temp.grad_x + temp.grad_y * temp.grad_y;
        if (magnitude > max) {
          grad.grad_x = temp.grad_x;
          grad.grad_y = temp.grad_y;
          max = magnitude;
        }
      }
    }
  }
  grad_T.grad_x = grad.grad_y;
  grad_T.grad_y = -grad.grad_x;
  norm nn = GetNorm(i, j);
  result = nn.norm_x * grad_T.grad_x + nn.norm_y * grad_T.grad_y;
  result /= 255;
  result = fabs(result);
  return result;
}

gradient inpainting::GetGradient(int i, int j) {
  gradient result;
  result.grad_x = (m_gray[j * m_width + i + 1] - m_gray[j * m_width + i - 1]) / 2.0;
  result.grad_y = (m_gray[(j + 1) * m_width + i] - m_gray[(j - 1) * m_width + i]) / 2.0;
  if (i == 0)result.grad_x = m_gray[j * m_width + i + 1] - m_gray[j * m_width + i];
  if (i == m_width - 1)result.grad_x = m_gray[j * m_width + i] - m_gray[j * m_width + i - 1];
  if (j == 0)result.grad_y = m_gray[(j + 1) * m_width + i] - m_gray[j * m_width + i];
  if (j == m_height - 1)result.grad_y = m_gray[j * m_width + i] - m_gray[(j - 1) * m_width + i];
  return result;
}

norm inpainting::GetNorm(int i, int j) {
  norm result;
  int num = 0;
  int neighbor_x[9];
  int neighbor_y[9];
  int record[9];
  int count = 0;
  for (int y = MAX(j - 1, 0); y<= MIN(j + 1, m_height - 1); ++y) {
    for (int x = MAX(i - 1, 0); x<= MIN(i + 1, m_width - 1); ++x) {
      count++;
      if (x == i && y== j) continue;
      if (m_mark[y * m_width + x] == BOUNDARY) {
        num++;
        neighbor_x[num] = x;
        neighbor_y[num] = y;
        record[num] = count;
      }
    }
  }
  if (num ==0 || num == 1) {
    result.norm_x = 0.6;
    result.norm_y = 0.8;
    return result;
  }
  int n_x = neighbor_x[2]-neighbor_x[1];
  int n_y = neighbor_y[2]-neighbor_y[1];
  int temp = n_x;
  n_x = n_y;
  n_y = temp;
  double square = pow(static_cast<double>(n_x * n_x + n_y * n_y), 0.5);
  result.norm_x = n_x / square;
  result.norm_y =n_y / square;
  return result;
}
bool inpainting::DrawSource(void) {
  bool flag;
  for (int j = 0; j < m_height; ++j) {
    for (int i = 0; i < m_width; ++i) {
      flag = 1;
      if (i < myWinSize ||
         j < myWinSize ||
         i >= m_width - myWinSize ||
         j >= m_height - myWinSize) {
        m_source[j * m_width + i] = false;
      } else {
        for (int y = j - myWinSize; y <= j + myWinSize; ++y) {
          for (int x = i - myWinSize; x <= i + myWinSize; ++x) {
            if (m_mark[y * m_width + x] != SOURCE) {
              m_source[j * m_width + i] = false;
              flag = false;
              break;
            }
          }
          if (flag == false) break;
        }
        if (flag != false) m_source[j * m_width + i] = true;
      }
    }
  }
  return true;
}
void inpainting::DrawBoundary(void) {
  int color;
  for (int y = 0; y < m_height; ++y) {
    for (int x = 0; x < m_width; ++x) {
     color =  Image_data->GetPixel(y * m_width + x);
     //  cout<<"color = "<<hex<<color<<endl;
      if (color == GREEN || color == 0x0000ff00) {
          //  cout<<"GREEN = "<<hex<<color<<endl;
        m_mark[y * m_width + x] = TARGET;
        m_confid[y * m_width + x] = 0;
        pix_to_inpaint++;
      } else {
       m_mark[y * m_width + x] = SOURCE;
       m_confid[y * m_width + x] = 1;
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
           m_mark[(j - 1) * m_width + i] == SOURCE||
           m_mark[j * m_width + i - 1] == SOURCE||
           m_mark[j * m_width + i + 1] == SOURCE||
           m_mark[(j + 1) * m_width + i] == SOURCE) {
             m_mark[j * m_width + i] = BOUNDARY;
        }
      }
    }
  }
}
void inpainting::Convert2Gray() {
  int cc;
  double r, g, b;
  for (int y = 0; y < m_height; ++y)
    for (int x = 0; x < m_width; ++x) {
      cc = Image_data->GetPixel(y * m_width + x);
      r = GET_R_VALUE(cc);
      g = GET_G_VALUE(cc);
      b = GET_B_VALUE(cc);
      m_gray[y * m_width + x] = static_cast<double>((r * 3735 + g * 19267 + b * 9765) / 32767);
    }
}

bool inpainting::PatchTexture(int x, int y, int &patch_x, int &patch_y, int** &most_similar_patch_pos, int** &most_similar_patch_pos_2) {
  double temp_r;
  double temp_g;
  double temp_b;
  COLORREF color_target, color_source, color_diff;
  double r0, g0, b0;
  double r1, g1, b1;
  int  min = 99999999;
  int min2 = 999999999;
  double sum;
  double sum2;
  int source_x, source_y;
  int target_x, target_y;
  //  下面的结果暂时没用到
  /*
  double mean = 0;
  double mean_G =0;
  double mean_B =0;
  double mean_R =0;
  double target_variance =0;
  double target_variance_G =0;
  double target_variance_B =0;
  double target_variance_R =0;

  double source_mean = 0;
  double source_mean_G =0;
  double source_mean_B =0;
  double source_mean_R =0;
  double source_variance =0;
  double source_variance_G =0;
  double source_variance_B =0;
  double source_variance_R =0;
  double num_of_filled = 0;
  double source_num_of_filled = 0;
  */
  double num_of_known = 0;
  //  下面的结果暂时没用到
  /*
  for(int iter_y=(-1) * myWinSize; iter_y <= myWinSize; ++iter_y) {
    for(int iter_x=(-1) * myWinSize; iter_x <= myWinSize; ++iter_x) {
      target_x = x + iter_x;
      target_y = y + iter_y;
      if(target_x < 0||
         target_x >= m_width||
         target_y < 0||
         target_y >= m_height) continue;
        if(m_mark[target_y * m_width + target_x] >= 0) {
          mean += m_gray[target_y * m_width+target_x];
          mean_R += m_r[target_y * m_width+target_x];
          mean_G += m_g[target_y * m_width+target_x];
          mean_B += m_b[target_y * m_width+target_x];
          num_of_filled++;
        }
      }
    }
  mean = mean / num_of_filled;
  mean_R = mean_R / num_of_filled;
  mean_G = mean_G / num_of_filled;
  mean_B = mean_B / num_of_filled;
  */
  //  下面的结果暂时没用到
  /*
  for(int iter_y=(-1) * myWinSize; iter_y <= myWinSize; ++iter_y) {
    for(int iter_x=(-1) * myWinSize; iter_x <= myWinSize; ++iter_x) {
      target_x = x + iter_x;
      target_y = y + iter_y;
      if(target_x < 0||
         target_x >= m_width||
         target_y < 0||
         target_y >= m_height) continue;
        if(m_mark[target_y * m_width + target_x] >= 0) {
          target_variance += (m_gray[target_y * m_width+target_x] - mean) * (m_gray[target_y * m_width+target_x] - mean);
          target_variance_R += (m_r[target_y * m_width+target_x] - mean_R) * (m_r[target_y * m_width+target_x] - mean_R);
          target_variance_G += (m_g[target_y * m_width+target_x] - mean_G) * (m_g[target_y * m_width+target_x] - mean_G);
          target_variance_B += (m_b[target_y * m_width+target_x] - mean_B) * (m_b[target_y * m_width+target_x] - mean_B);
        }
      }
    }
*/
  int ouy = 0;
  int oux = 0;
  for (int j = 0; j < m_height; ++j) {
    for (int i = 0; i < m_width; ++i) {
     if (m_source[j * m_width + i] == false) continue;
        sum = 0;
        sum2 = 0;
        // 下面这些结果暂时没用到
        /*
        for(int iter_y=(-1) * myWinSize; iter_y <= myWinSize; ++iter_y) {
          for(int iter_x=(-1) * myWinSize; iter_x <= myWinSize; ++iter_x) {
            source_x = i + iter_x;
            source_y = j + iter_y;
            if(source_x < 0||
               source_x >= m_width||
               source_y < 0||
               source_y >= m_height) continue;
            if(m_mark[source_y * m_width+source_x]>=0) {
                source_mean += m_gray[source_y * m_width+source_x];
                source_mean_R += m_r[source_y * m_width+source_x];
                source_mean_G += m_g[source_y * m_width+source_x];
                source_mean_B += m_b[source_y * m_width+source_x];
                source_num_of_filled++;
            }
          }
          }
        source_mean = source_mean / source_num_of_filled;
        source_mean_R = source_mean_R / source_num_of_filled;
        source_mean_G = source_mean_G / source_num_of_filled;
        source_mean_B = source_mean_B / source_num_of_filled;
        for(int iter_y=(-1) * myWinSize; iter_y <= myWinSize; ++iter_y) {
          for(int iter_x=(-1) * myWinSize; iter_x <= myWinSize; ++iter_x) {
              source_x = i + iter_x;
              source_y = j + iter_y;
              if(source_x < 0||
                 source_x >= m_width||
                 source_y < 0||
                 source_y >= m_height) continue;
            if(m_mark[source_y * m_width+source_x]>=0) {
                source_variance += (m_gray[source_y * m_width+source_x] - source_mean) * (m_gray[source_y * m_width+source_x] - source_mean);
                source_variance_R += (m_r[source_y * m_width+source_x] - source_mean_R) * (m_r[source_y * m_width+source_x] - source_mean_R);
                source_variance_G += (m_g[source_y * m_width+source_x] - source_mean_G) * (m_g[source_y * m_width+source_x] - source_mean_G);
                source_variance_B += (m_b[source_y * m_width+source_x] - source_mean_B) * (m_b[source_y * m_width+source_x] -source_mean_B);
            }
          }
          }
        source_variance = source_variance / source_num_of_filled;
        source_variance_R = source_variance_R / source_num_of_filled;
        source_variance_G = source_variance_G / source_num_of_filled;
        source_variance_B = source_variance_B / source_num_of_filled;
        */
        num_of_known = 0;
        for (int iter_y=(-1) * myWinSize; iter_y <= myWinSize; ++iter_y) {
          for (int iter_x=(-1) * myWinSize; iter_x <= myWinSize; ++iter_x) {
            source_x = i + iter_x;
            source_y = j + iter_y;
            target_x = x + iter_x;
            target_y = y + iter_y;
            if (target_x < 0 ||
               target_x >= m_width ||
               target_y < 0 ||
               target_y >= m_height) continue;
            if (m_mark[target_y * m_width+target_x] >= 0) {
              temp_r = m_r[target_y * m_width + target_x] - m_r[source_y * m_width + source_x];
              temp_g = m_g[target_y * m_width + target_x] - m_g[source_y * m_width + source_x];
              temp_b = m_b[target_y * m_width + target_x] - m_b[source_y * m_width + source_x];
              sum += temp_r * temp_r + temp_g * temp_g + temp_b * temp_b;
              //  sum2 += ( m_gray[target_y * m_width+target_x] - mean) * (m_gray[target_y * m_width+target_x] - mean);
              num_of_known++;
              //  temp_r = m_r[source_y * m_width + source_x] - mean_R;
              //  temp_g = m_g[source_y * m_width + source_x] - mean_G;
              //  temp_b = m_b[source_y * m_width + source_x] - mean_B;
              //  sum2 += temp_r * temp_r + temp_g * temp_g + temp_b * temp_b;
            }
          }
          }
        double distance= sqrt((source_x - target_x) * (source_x - target_x) + (source_y - target_y) * (source_y - target_y));
        //  平均值和方差一类的差异
        //  double temp_sum=sum;
        //  double temp_sum2=num_of_known *( source_mean - mean) * ( source_mean - mean);
        //  double temp_sum3=num_of_known * ((sqrt(source_variance_R) -sqrt(target_variance_R / num_of_filled)) *(sqrt(source_variance_R) -sqrt(target_variance_R / num_of_filled)) + (sqrt(source_variance_G) -sqrt(target_variance_G / num_of_filled)) *(sqrt(source_variance_G) -sqrt(target_variance_G / num_of_filled)) + (sqrt(source_variance_B) -sqrt(target_variance_B / num_of_filled)) *(sqrt(source_variance_B) -sqrt(target_variance_B / num_of_filled)))/3;
       // sum = sum + sqrt( ( source_mean - mean) * ( source_mean - mean)) +  sqrt(num_of_known * ((source_variance_R -target_variance_R / num_of_filled) *(source_variance_R -target_variance_R / num_of_filled) + (source_variance_G -target_variance_G / num_of_filled) *(source_variance_G -target_variance_G / num_of_filled) + (source_variance_B -target_variance_B / num_of_filled) *(source_variance_B -target_variance_B / num_of_filled))/3);
        //  sum =sqrt(sum) * sqrt(temp_sum3);
        /*  if(sum2 < min2) {
              ouy++;
              min2 = sum2;
            for(int o=LOOKUP-1;o>0;--o){
                most_similar_patch_val_2[o]=most_similar_patch_val_2[o-1];
                most_similar_patch_pos_2[o][0]=most_similar_patch_pos_2[o-1][0];
                most_similar_patch_pos_2[o][1]=most_similar_patch_pos_2[o-1][1];
            }
            most_similar_patch_val_2[0]=sum2;
            most_similar_patch_pos_2[0][0]=i;
            most_similar_patch_pos_2[0][1]=j;
          }*/
        if (sum + distance < min) {
        //  if(sum < min) {
          oux++;
          min = sum;
          patch_x = i;
          patch_y = j;
          for (int o = LOOKUP - 1; o > 0; --o) {
              most_similar_patch_pos[o][0] = most_similar_patch_pos[o - 1][0];
              most_similar_patch_pos[o][1] = most_similar_patch_pos[o - 1][1];
          }
          most_similar_patch_pos[0][0] = patch_x;
          most_similar_patch_pos[0][1] = patch_y;
        }
    }
  }
  return true;
}
bool inpainting::FillTarget(int target_x, int target_y, int source_x, int source_y, double confid) {
  COLORREF color;
  double r, g, b;
  int x0, y0, x1, y1;
  for (int iter_y=(-1) * myWinSize; iter_y <= myWinSize; ++iter_y) {
    for (int iter_x=(-1) * myWinSize; iter_x <= myWinSize; ++iter_x) {
      x0 = source_x + iter_x;
      y0 = source_y + iter_y;
      x1 = target_x + iter_x;
      y1 = target_y + iter_y;
      if (m_mark[y1 * m_width + x1] < 0) {
          pix_been_inpainted++;
        Image_data->SetPixel(y1 * m_width + x1, m_color[y0 * m_width + x0]);  //  inpaint the color
        m_color[y1 * m_width + x1] = m_color[y0 * m_width + x0];
        m_r[y1 * m_width + x1] = m_r[y0 * m_width + x0];
        m_g[y1 * m_width + x1] = m_g[y0 * m_width + x0];
        m_b[y1 * m_width + x1] = m_b[y0 * m_width + x0];
        m_gray[y1 * m_width + x1] = static_cast<double>((m_r[y0 * m_width+x0] * 3735 + m_g[y0 * m_width + x0] * 19267 + m_b[y0 * m_width + x0] * 9765) / 32767);  //  update gray image
        m_confid[y1 * m_width + x1] = confid;  //  update the confidence
      }
    }
  }
  return true;
}

bool inpainting::TargetExist(void) {
    for (int j = 0; j <= m_height; ++j) {
        for (int i = 0; i <= m_width; ++i) {
      if (m_mark[j * m_width + i] < 0) return true;
    }
  }
  return false;
}
void inpainting::UpdateBoundary(int i, int j) {
  int x, y;
  COLORREF color;
  for (y = 0; y <= m_height - 1; ++y) {
      for (x = 0; x <= m_width - 1; ++x) {
      color = Image_data->GetPixel(y * m_width +x);
      if (color == GREEN || color == 0x0000ff00) {
        m_mark[y * m_width + x] = TARGET;
      } else {
        m_mark[y * m_width + x] = SOURCE;
      }
    }
  }

  for (y = 0; y <= m_height - 1; ++y) {
      for (x = 0; x <=m_width - 1; ++x) {
      if (m_mark[y * m_width + x] == TARGET) {
        if (y == m_height - 1 ||
           y == 0 ||
           x == 0 ||
           x == m_width - 1||
           m_mark[(y - 1) * m_width + x] == SOURCE ||
           m_mark[y * m_width + x - 1] == SOURCE ||
           m_mark[y * m_width + x + 1] == SOURCE ||
           m_mark[(y + 1) * m_width + x] == SOURCE) {
             m_mark[y * m_width + x] = BOUNDARY;
        }
      }
    }
  }
}
void inpainting::UpdatePriority(int i, int j) {
  int x, y;
  for (y = MAX(j - myWinSize - 3, 0); y <= MIN(j + myWinSize + 3, m_height - 1); ++y) {
    for (x = MAX(i - myWinSize - 3, 0); x <= MIN(i + myWinSize + 3, m_width - 1); ++x) {
      if (m_mark[y * m_width + x] == BOUNDARY) {
        m_pri[y * m_width + x] = ComputePriority(x, y);
      }
    }
  }
}
void inpainting::DrawEdge(ImageData *origin_canny_edge) {
  int x, y;
  bool isEdgeConnected_b;
  bool isTargetRegion = false;
  int start_x;
  int start_y;
  int end_x;
  int end_y;
  gradient grad, temp, grad_T;
  grad.grad_x = 0;
  grad.grad_y = 0;
  double result;
  double magnitude;
  double max = 0;
  for (y = 0; y < m_height; ++y) {
      for (x = 0; x < m_width; ++x) {
      isTargetRegion = false;
      if (origin_canny_edge->GetPixel(y * m_width + x) == 0xffffff && m_mark[y * m_width + x] == SOURCE) {
        isEdgeConnected_b = isEdgeConnected(x, y, origin_canny_edge);
        start_x = std::max(0, x - 2);
        start_y = std::max(0, y - 2);
        end_x = std::min(x + 2 , m_width - 1);
        end_y = std::min(y + 2 , m_height - 1);
        for (int i = start_x; i <= end_x; ++i) {
          for (int j = start_y; j <= end_y; ++j) {
            if (m_mark[j * m_width + i] < 0) {
              isTargetRegion = true;
            }
          }
        }
        if (isTargetRegion && isEdgeConnected_b) {
        //if (isTargetRegion) {
          m_edge_mark[y * m_width +x] = 1;
          vec.push_back(y * m_width + x);
          grad.grad_x = 0;
          grad.grad_y = 0;
          max = 0;
          for (int j = MAX(y - 2, 0); j <= MIN(y + 2, m_height - 1); ++j) {
            for (int i = MAX(x - 2, 0); i <= MIN(x + 2, m_width - 1); ++i) {
              if (m_mark[j * m_width + i] >= 0) {
                if (m_mark[j * m_width + i + 1] < 0 ||
                  m_mark[j * m_width + i - 1] < 0 ||
                  m_mark[(j + 1) * m_width + i] < 0 ||
                  m_mark[(j - 1) * m_width + i] < 0) continue;
                temp = GetGradient(i, j);
                magnitude = temp.grad_x * temp.grad_x + temp.grad_y * temp.grad_y;
                if (magnitude > max) {
                  grad.grad_x = temp.grad_x;
                  grad.grad_y = temp.grad_y;
                  max = magnitude;
               }
             }
           }
         }
         vec_gradient.push_back(grad);
       }
        /*if (x == 0 && y == 0) {
          if (m_mark[(y + 1) * m_width + x] < 0 ||
           m_mark[y * m_width + x + 1] <0 ||
           m_mark[(y + 1) * m_width + x + 1] < 0) {
             m_edge_mark[0] = 1;
             vec.push_back(0);
          }
        } else if (x == 0 && y == (m_height - 1)) {
          if (m_mark[(y - 1) * m_width + x] < 0 ||
            m_mark[y * m_width + x + 1] < 0 ||
            m_mark[(y - 1) * m_width + x + 1] < 0) {
            m_edge_mark[y * m_width + x] = 1;
            vec.push_back(y * m_width + x);
          }
        } else if (x == (m_width - 1) && y == 0) {
          if (m_mark[(y + 1) * m_width + x] < 0 ||
           m_mark[y * m_width + x - 1] < 0 ||
           m_mark[(y + 1) * m_width + x - 1] < 0) {
             m_edge_mark[y * m_width + x] = 1;
             vec.push_back(y * m_width + x);
          }
        } else if (x == (m_width - 1) && y == (m_height - 1)) {
          if (m_mark[(y - 1) * m_width + x] < 0 ||
            m_mark[y * m_width + x - 1] < 0 ||
            m_mark[(y - 1) * m_width + x - 1] < 0) {
            m_edge_mark[y * m_width + x] = 1;
            vec.push_back(y * m_width + x);
          }
        } else if (x == 0 && y > 0 && y < (m_height - 1)) {
          if (m_mark[(y + 1) * m_width + x] < 0 ||
           m_mark[y * m_width + x + 1] < 0 ||
           m_mark[(y + 1) * m_width + x + 1] < 0 ||
           m_mark[(y - 1) * m_width + x + 1] < 0 ||
           m_mark[(y - 1) * m_width + x ] <0) {
             m_edge_mark[y * m_width + x] = 1;
             vec.push_back(y * m_width + x);
          }
        } else if (x == (m_width - 1) && y > 0 && y < (m_height - 1)) {
          if (m_mark[(y + 1) * m_width + x] < 0 ||
           m_mark[y * m_width + x - 1] < 0 ||
           m_mark[(y + 1) * m_width + x - 1] < 0 ||
           m_mark[(y - 1) * m_width + x - 1] < 0 ||
           m_mark[(y - 1) * m_width + x ] < 0) {
             m_edge_mark[y * m_width + x] = 1;
             vec.push_back(y * m_width + x);
          }
        } else if (x < (m_width - 1) && x > 0 &&y == 0) {
          if (m_mark[(y + 1) * m_width + x] < 0 ||
           m_mark[y * m_width + x + 1] < 0 ||
           m_mark[(y + 1) * m_width + x + 1] < 0 ||
           m_mark[y * m_width + x - 1] < 0 ||
           m_mark[(y + 1) * m_width + x - 1] < 0) {
             m_edge_mark[y * m_width + x] = 1;
             vec.push_back(y * m_width + x);
          }
        } else if (x < (m_width - 1) && x > 0 && y ==(m_height - 1)) {
          if (m_mark[(y - 1) * m_width + x] < 0 ||
           m_mark[y * m_width + x + 1] < 0 ||
           m_mark[(y - 1) * m_width + x + 1] < 0 ||
           m_mark[y * m_width + x - 1] < 0 ||
           m_mark[(y - 1) * m_width + x - 1] < 0) {
             m_edge_mark[y * m_width + x] = 1;
             vec.push_back(y * m_width + x);
          }
        } else {
         if (m_mark[(y - 1) * m_width + x] < 0 ||
           m_mark[y * m_width + x + 1] < 0 ||
           m_mark[(y - 1) * m_width + x + 1] < 0 ||
           m_mark[y * m_width + x - 1] < 0 ||
           m_mark[(y - 1) * m_width + x - 1] < 0 ||
           m_mark[(y + 1) * m_width + x] < 0 ||
           m_mark[(y + 1) * m_width + x + 1] < 0 ||
           m_mark[(y + 1) * m_width + x - 1] < 0) {
             m_edge_mark[y * m_width + x] = 1;
             vec.push_back(y * m_width + x);
          }
        }*/
      }
    }
  }
}
void inpainting::ConnectEdge() {
  int x, y;
  std::vector<int>::iterator it1;
  std::vector<int>::iterator it2;
  std::vector<gradient>::iterator it_g1;
  std::vector<gradient>::iterator it_g2;
  int temp_index;
  int min_diff = 99999999;
  int temp_r;
  int temp_g;
  int temp_b;
  int sum = 0;
  int x1, x2, y1, y2;
  int max_x, max_y, min_x, min_y;
  double k1 = 0 , k2 = 0;
  int temp_x, temp_y;
  std::cout << vec.size() << std::endl;
  for (it1 = vec.begin(), it_g1 = vec_gradient.begin(); it1 != vec.end(); it1++, it_g1++) {
    min_diff = 99999999;
    temp_index = *it1;
    sum = 0;
    if (m_edge_mark[*it1] == 1) {
      for (it2 = vec.begin(), it_g2 = vec_gradient.begin(); it2 != vec.end(); it2++, it_g2++) {
        std::cout << "ttt" << *it1 << " " << *it2 << std::endl;
        temp_r = m_r[*it1] - m_r[*it2];
        temp_g = m_g[*it1] - m_g[*it2];
        temp_b = m_b[*it1] - m_b[*it2];
        sum = temp_r * temp_r + temp_g * temp_g + temp_b * temp_b + (pow((*it_g1).grad_y - (*it_g2).grad_y, 2) + pow((*it_g1).grad_x - (*it_g2).grad_x, 2)) * 3 / 2;
        std::cout << "sum " << sum << std::endl;
        std::cout << "vec_diff_pow " <<  pow((*it_g1).grad_y - (*it_g2).grad_y, 2) + pow((*it_g1).grad_x - (*it_g2).grad_x, 2)<< std::endl;
          if ((sum <= min_diff) && (*it2) != (*it1)) {
            std::cout << "sum2 " << sum << std::endl;
            min_diff = sum;
            temp_index = *it2;
          }
      }
      if (m_edge_mark[temp_index] < 0) {
        continue;
      }
      m_edge_mark[*it1] = -1;
      m_edge_mark[temp_index] = -1;
      x1 = *it1 % m_width;
      y1 = *it1 / m_width;
      x2 = temp_index % m_width;
      y2 = temp_index / m_width;
      std::cout << x2 << " " << y2 << " " << temp_index << " " << m_width << std::endl;
      max_x = std::max(x1, x2);
      min_x = std::min(x1, x2);
      max_y = std::max(y1, y2);
      min_y = std::min(y1, y2);
      if (x1 != x2) {
        k1 = (static_cast<double>(y1 - y2))/(static_cast<double>(x1 - x2));
      }
      if (y1 != y2) {
        k2 = (static_cast<double>(x1 - x2))/(static_cast<double>(y1 - y2));
      }
        Image_data->SetPixel(y1 * m_width + x1,RED);
      for (int i = 1; i < max_x - min_x; ++i) {
        temp_x = min_x + i;
        if (k1 > 0) {
          temp_y = min_y + i * k1;
        } else if (k1 < 0) {
          temp_y = max_y + i * k1;
        }
       // cout<<"temp_x "<<temp_x<<"temp_y "<<temp_y<<endl;
        Image_data->SetPixel(temp_y * m_width + temp_x, m_color[y1 * m_width + x1]);
      }
      for (int i = 1; i < max_y - min_y; ++i) {
        if (k2 > 0) {
          temp_x = min_x + i * k2;
        } else if (k2 < 0) {
          temp_x = max_x + i * k2;
        }
        temp_y = min_y + i;
        //  cout<<"temp_x "<<temp_x<<"temp_y "<<temp_y<<endl;
        Image_data->SetPixel(temp_y * m_width + temp_x, m_color[y1 * m_width + x1]);
      }
    }
  }
  UpdateBoundary(0, 0);
}
bool inpainting::isEdgeConnected(int x, int y, ImageData *origin_canny_edge) {
  int start_x = std::max(0, x - 1);
  int start_y = std::max(0, y - 1);
  int end_x = std::min(x + 1 , m_width - 1);
  int end_y = std::min(y + 1 , m_height - 1);
  for (int i = start_x; i < end_x; ++i) {
    for (int j = start_y; j < end_y; ++j) {
      if (origin_canny_edge->GetPixel(j * m_width + i) == 0xffffff) {
        return true;
      }
    }
  }
  return false;
}
inpainting::~inpainting() {
    //  delete Image;
    //  delete Image_data;
    delete [] m_r;
    delete [] m_g;
    delete [] m_b;
    delete [] m_mark;
    delete [] m_confid;
    delete [] m_pri;
    delete [] m_gray;
    delete [] m_source;
    delete [] m_color;
    delete [] m_edge_mark;
}
