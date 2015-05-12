#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__
#include <ios>
#include <stdexcept>
#include <stdio.h>

namespace std {
  class Pixel {
    public:
    unsigned long int getValue() {return 0;};
  };

  class Image {
    protected:
    int height, width;
    Image();

    public:
    Image(int width, int height) throw(std::bad_alloc): 
      height(height), width(width) {};
    virtual ~Image() {};
    unsigned getHeight() { return height; };
    unsigned getWidth() { return width; };
    virtual Pixel *getScanLine(unsigned scanLine) = 0;
  };

  class ImageLoader {
    public:
    ImageLoader() {};
    virtual Image *loadFrom(FILE *infile) throw (std::runtime_error) = 0;
  }; 
}
#endif
