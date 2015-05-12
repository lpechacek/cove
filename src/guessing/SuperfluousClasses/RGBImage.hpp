#ifndef __RGBIMAGE_HPP__
#define __RGBIMAGE_HPP__
#include "Image.hpp"

namespace std {
  class RGBImage: public Image {
    unsigned char **rowPointers;
    public:
    RGBImage(int width, int height) throw(std::bad_alloc);
    virtual ~RGBImage();
    unsigned char **getRowPointers();
    virtual Pixel *getScanLine(unsigned scanLine);
  };
}
#endif
