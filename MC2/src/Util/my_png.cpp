#define _POSIX_SOURCE 1

#include <png.h>
#include <string.h>
#include <cstdlib>
#include <cstdio>

/* The png_jmpbuf() macro, used in error handling, became available in
* libpng version 1.0.6.  If you want to be able to run your code with older
* versions of libpng, you must define the macro yourself (but only if it
* is not already defined by libpng!).
*/
#ifndef png_jmpbuf
#define png_jmpbuf(png_ptr) ((png_ptr)->png_jmpbuf)
#endif

extern "C" {

unsigned char* read_png(const char* file_name, unsigned int* width, unsigned int* height, int* bit_depth) {
  FILE *fp;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  int color_type;
  int interlace_type;
  unsigned long rowbytes;
  png_bytepp row_pointers;
  unsigned char* buffer;
  int y;

  if ((fp = fopen(file_name, "rb")) == NULL) {
    return NULL;
  }

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

  if (!png_ptr) {
    return NULL;
  }

  info_ptr = png_create_info_struct(png_ptr);

  if (!info_ptr) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    return NULL;
  }

#pragma warning(push)
#pragma warning(disable:4611)
  if (setjmp(png_jmpbuf(png_ptr))) {
#pragma warning(pop)
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    return NULL;
  }

  png_init_io(png_ptr, fp);

  png_read_info(png_ptr, info_ptr);

  png_get_IHDR(png_ptr, info_ptr, width, height, bit_depth, &color_type, &interlace_type, NULL, NULL);

  if (color_type != PNG_COLOR_TYPE_RGBA) {
    png_set_expand(png_ptr);
    png_set_filler(png_ptr, 0, PNG_FILLER_AFTER);
  }

  png_read_update_info(png_ptr, info_ptr);

  *height = png_get_image_height(png_ptr, info_ptr);
  *width = png_get_image_width(png_ptr, info_ptr);
  *bit_depth = png_get_bit_depth(png_ptr, info_ptr);

  row_pointers = (png_byte**)malloc(sizeof(png_byte*) * (*height));

  rowbytes = png_get_rowbytes(png_ptr, info_ptr);
  buffer = (png_byte*)malloc((*height) * rowbytes);

  for (y = *height - 1; y >= 0; y--) {
    row_pointers[*height - 1 - y] = (png_byte*)(buffer + (y * rowbytes));
  }

  png_set_rows(png_ptr, info_ptr, row_pointers);
  png_read_image(png_ptr, row_pointers);
  png_read_end(png_ptr, NULL);

  fclose(fp);
  free(row_pointers);
  png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

  return buffer;
}

unsigned char* read_png_wacky(char* file_name, unsigned int* width, unsigned int* height, int* bit_depth) {
  png_image image; /* The control structure used by libpng */

  /* Initialize the 'png_image' structure. */
  memset(&image, 0, (sizeof image));
  image.version = PNG_IMAGE_VERSION;

  /* The first argument is the file to read: */
  if (png_image_begin_read_from_file(&image, file_name)) {
    png_bytep buffer;

    /* Set the format in which to read the PNG file; this code chooses a
    * simple sRGB format with a non-associated alpha channel, adequate to
    * store most images.
    */
    image.format = PNG_FORMAT_RGBA;

    /* Now allocate enough memory to hold the image in this format; the
    * PNG_IMAGE_SIZE macro uses the information about the image (width,
    * height and format) stored in 'image'.
    */
    buffer = (png_bytep)malloc(PNG_IMAGE_SIZE(image));

    /* If enough memory was available read the image in the desired format
    * then write the result out to the new file.  'background' is not
    * necessary when reading the image because the alpha channel is
    * preserved; if it were to be removed, for example if we requested
    * PNG_FORMAT_RGB, then either a solid background color would have to
    * be supplied or the output buffer would have to be initialized to the
    * actual background of the image.
    *
    * The fourth argument to png_image_finish_read is the 'row_stride' -
    * this is the number of components allocated for the image in each
    * row.  It has to be at least as big as the value returned by
    * PNG_IMAGE_ROW_STRIDE, but if you just allocate space for the
    * default, minimum, size using PNG_IMAGE_SIZE as above you can pass
    * zero.
    *
    * The final argument is a pointer to a buffer for the colormap;
    * colormaps have exactly the same format as a row of image pixels (so
    * you choose what format to make the colormap by setting
    * image.format).  A colormap is only returned if
    * PNG_FORMAT_FLAG_COLORMAP is also set in image.format, so in this
    * case NULL is passed as the final argument.  If you do want to force
    * all images into an index/color-mapped format then you can use:
    *
    *    PNG_IMAGE_COLORMAP_SIZE(image)
    *
    * to find the maximum size of the colormap in bytes.
    */
    if (buffer != NULL &&
      png_image_finish_read(&image, NULL/*background*/, buffer,
      0/*row_stride*/, NULL/*colormap*/)) {
        *height = image.height;
        *width = image.width;
        *bit_depth = 8;
        return buffer;
    } else {
      //png_free_image(&image);
    }
  }
  return NULL;
}

}