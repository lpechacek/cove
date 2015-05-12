#include "RGBImage.hpp"
#include "ImageLoaderPNG.hpp"
#include <png.h>
#include <stdexcept>

using namespace std;

Image *ImageLoaderPNG::loadFrom(FILE *infile) throw (std::runtime_error) {
  // Create structures
  png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
      NULL,NULL,NULL);
  if (!png_ptr)
    throw runtime_error("ENOMEM");

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr)
  {
    png_destroy_read_struct(&png_ptr,
	(png_infopp)NULL, (png_infopp)NULL);
    throw runtime_error("ENOMEM");
  }

  png_infop end_info = png_create_info_struct(png_ptr);
  if (!end_info)
  {
    png_destroy_read_struct(&png_ptr, &info_ptr,
	(png_infopp)NULL);
    throw runtime_error("ENOMEM");
  }

  if (setjmp(png_jmpbuf(png_ptr)))
  {
    /* Free all of the memory associated with the png_ptr and info_ptr */
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
    /* If we get here, we had a problem reading the file */
    throw runtime_error("READ ERROR");
  }

  // Initialize read
  png_init_io(png_ptr, infile);

  // Perform read
  png_uint_32 width, height;
  int bit_depth, color_type;
  png_read_info(png_ptr, info_ptr);
  png_get_IHDR(png_ptr, info_ptr, &width, &height,
      &bit_depth, &color_type, int_p_NULL, int_p_NULL, int_p_NULL);
  switch (color_type) {
    case PNG_COLOR_TYPE_PALETTE: png_set_palette_to_rgb(png_ptr); break;
    case PNG_COLOR_TYPE_GRAY: png_set_gray_to_rgb(png_ptr); break;
    case PNG_COLOR_TYPE_RGB_ALPHA: 
    case PNG_COLOR_TYPE_GRAY_ALPHA: png_set_strip_alpha(png_ptr); break;
  };

  RGBImage *img = new RGBImage(height, width);
  png_bytep *row_pointers = img->getRowPointers();
  png_read_image(png_ptr, row_pointers);
  png_read_end(png_ptr, info_ptr);

  return img;
}
