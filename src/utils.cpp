// Copyright 2014-4 sxniu
#include "include/utils.h"
#include <algorithm>
#include <iostream>
#include <utility>
#include <math.h>
#include <stdio.h>
#include "include/ImageData.h"
#include "include/region_filling_by_edge_tracing.h"
#include "include/colour.h"

namespace utils {

void TurnGray(ImageData* image) {
  int height = image->GetHeight();
  int width = image->GetWidth();
  for (int y = 0; y < height; ++y) {
    for (int x  = 0; x < width; ++x) {
      int index = y * width + x;
      int red = (image->GetPixel(index) & RED) >> 16;
      int green = (image->GetPixel(index) & GREEN) >> 8;
      int blue = image->GetPixel(index) & BLUE;
      int gray = red * 0.3 + green * 0.59 + blue * 0.11;
      int colour = (gray << 16) + (gray << 8) + gray;
      image->SetPixel(index, colour);
    }
  }
}

}  // namespace utils
