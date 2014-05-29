// Copyright 2014-4 xtu
#ifndef INCLUDE_SKETCH_H_
#define INCLUDE_SKETCH_H_
#include <string>
#include "include/RWImage.h"
#include "include/ImageData.h"
class sketch {
 public:
  sketch();
  std::string Edge(const char* filename);
  ~sketch();
 private:
};

#endif  // INCLUDE_SKETCH_H_
