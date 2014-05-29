// Copyright 2014-4 xtu
#ifndef INCLUDE_INPAINTING_H_
#define INCLUDE_INPAINTING_H_
#include <string>
#include <vector>
#include <iostream>
#include "include/RWImage.h"
#include "include/ImageData.h"
typedef unsigned char  U8;
typedef unsigned short U16;
typedef unsigned int   U32;
typedef unsigned int COLORREF;
typedef struct {
  double grad_x;
  double grad_y;
}gradient;

typedef struct {
  long map_x;
  long map_y;
}mapping_point;

typedef struct {
  double norm_x;
  double norm_y;
}norm;

class inpainting {
 public:
    //  RWImage *Image;
    ImageData *Image_data;
    std::string Process();
    explicit inpainting(ImageData *Image_data);
    void DrawBoundary();
    void DrawEdge(ImageData *origin_canny_edge);
    void ConnectEdge();
     void Convert2Gray(void);
    //  inpainting(const char *name);
    ~inpainting();
 private :
  //  ofstream myfile;
  std::vector<int> vec;
  std::vector<gradient> vec_gradient;
  double pix_to_inpaint;
  double pix_been_inpainted;
  int boundary_top;
  int boundary_bottom;
  int boundary_left;
  int boundary_right;
  int max_height;
  int max_width;
  int m_width;
  int m_height;
  int *number_to_fill_x;
  int *number_to_fill_y;
  bool bOpen;
  int height_diff;
  int width_diff;
  U8 m_pDIBBits;
  int count_pic;
  U32 m_dwBytesPerLine;
  double * m_r;
  double * m_g;
  double * m_b;
  int * m_mark;
  double * m_confid;
  double * m_pri;
  double * m_gray;
  bool * m_source;
  int m_top, m_bottom, m_left, m_right;
  int *m_color;
  int * m_edge_mark;
  double ComputeConfidence(int i, int j);
  double ComputePriority(int x, int y);
  double ComputeData(int i, int j);
  gradient GetGradient(int i, int j);
  norm GetNorm(int i, int j);
  bool DrawSource(void);
  bool PatchTexture(int x , int y , int &patch_x , int &patch_y , int** &most_similar_patch , int** &most_similar_patch_2);
  bool FillTarget(int target_x , int target_y , int source_x , int source_y , double confid);
  bool TargetExist(void);
  void UpdateBoundary(int i, int j);
  void UpdatePriority(int i, int j);
  int *CountCR();
  bool isEdgeConnected(int x, int y, ImageData *origin_canny_edg);
};
#endif  // INCLUDE_INPAINTING_H_

