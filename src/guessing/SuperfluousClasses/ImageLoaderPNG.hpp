#ifndef __IMAGELOADERPNG_HPP__
#define __IMAGELOADERPNG_HPP__
#include "Image.hpp"

namespace std {
  class ImageLoaderPNG : public ImageLoader {
    public:
    virtual Image *loadFrom(FILE *infile) throw (std::runtime_error);
  };
}
#endif
