#pragma once

#include <cstdint>
#include <cmath>
#include <memory>

namespace {
  // modular modulo, assuming q is positive
  int32_t umod(int32_t d, int32_t q) {
    if(d < 0) {
      auto ret = q - -d % q;
      if(ret == q) return 0;
      else return ret;
    } else return d % q;
  }
}

extern const float BYTE_TO_FLOAT[256];
struct SHELLGEN2_API loaded_gray_png {
  uint32_t width, height;
  std::unique_ptr<uint8_t[]> pixels;
  std::unique_ptr<uint8_t*[]> rows;
  // note: u and v are assumed not to be normalized!
  float sample(float u, float v) {
    float ufloor, vfloor, ufract, vfract;
    int32_t x, y;
    ufloor = floorf(u);
    vfloor = floorf(v);
    ufract = u - ufloor;
    vfract = v - vfloor;
    x = static_cast<int32_t>(ufloor);
    y = static_cast<int32_t>(vfloor);
    // common subexpression elimination is something I can trust the compiler
    // to do, right? ...right?
    auto row1 = rows[umod(y,height)];
    auto row2 = rows[umod(y+1,height)];
    float a = BYTE_TO_FLOAT[row1[umod(x,width)]];
    float b = BYTE_TO_FLOAT[row1[umod(x+1,width)]];
    float c = BYTE_TO_FLOAT[row2[umod(x,width)]];
    float d = BYTE_TO_FLOAT[row2[umod(x+1,width)]];
    float ab = (a * (1.0f - ufract)) + (b * ufract);
    float cd = (c * (1.0f - ufract)) + (d * ufract);
    return (ab * (1.0f - vfract)) + (cd * vfract);
  }
};
