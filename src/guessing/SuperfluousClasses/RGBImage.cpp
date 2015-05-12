#include "RGBImage.hpp"

using namespace std;

RGBImage::RGBImage(int width, int height) throw(std::bad_alloc) :
  Image::Image(height,width)
{
  rowPointers = new unsigned char *[height];
  for(int i=0; i<height; i++) {
    rowPointers[i] = new unsigned char[width*3];
  }
}

RGBImage::~RGBImage() {
  for(int i=0; i<height; i++) 
    delete [] rowPointers[i];
  delete [] rowPointers;
}

unsigned char **RGBImage::getRowPointers() { 
  return rowPointers; 
}

Pixel *RGBImage::getScanLine(unsigned scanLine) {
  return NULL;
}
