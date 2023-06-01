/*
 * This file is part of Shell Shape Generator 2.
 *
 * Copyright ©2023 Olivia Jenkins
 *
 * Shell Shape Generator 2 is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * Shell Shape Generator 2 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Shell Shape Generator 2. If not, see <https://www.gnu.org/licenses/>.
 */

#include "LoadedGrayPNG.h"
#include "cook_path.h"

#include <libPNG/libPNG-1.5.27/png.h>

ULoadedGrayPNG::~ULoadedGrayPNG() {}

static void read_from_ifile(png_structp libpng, png_bytep buf, size_t len) {
  IFileHandle* file = (IFileHandle*)png_get_io_ptr(libpng);
  if(!file->Read(buf, len)) png_longjmp(libpng, 456);
}

// This is like the fifteenth time I've copied-and-pasted SubCritical's PNG loading
// code. Heh. -SB
ULoadedGrayPNG* ULoadedGrayPNG::LoadGrayPNG(const FString& filename) {
  IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
  FString path = shellgen_cook_path(filename);
  std::unique_ptr<IFileHandle> file(PlatformFile.OpenRead(*path));
  if(!file) return nullptr;
  png_structp libpng = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                              nullptr, nullptr, nullptr);
  if(!libpng) return nullptr;
  png_infop info = png_create_info_struct(libpng);
  if(!info) {
    png_destroy_read_struct(&libpng, nullptr, nullptr);
    return nullptr;
  }
  auto image = std::make_shared<loaded_gray_png>();
#pragma warning(suppress : 4611) // yes, thanks, MSVC, I know!
  if(setjmp(png_jmpbuf(libpng))) {
    png_destroy_read_struct(&libpng, &info, nullptr);
    return nullptr;
  }
  png_set_read_fn(libpng, file.get(), (png_rw_ptr)read_from_ifile);
  png_read_info(libpng, info);
  png_uint_32 width, height;
  int depth, color_type, ilace_method, compression_method, filter_method;
  png_get_IHDR(libpng, info, &width, &height, &depth, &color_type, &ilace_method,
               &compression_method, &filter_method);
  // For a 16-bpc image, strip off 8 bits.
  if(depth >= 16) png_set_strip_16(libpng);
  // For a palettized image, convert to RGB. (grayscale will be next)
  if(color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(libpng);
  // If there's a tRNS chunk, convert it to alpha.
  if(png_get_valid(libpng, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(libpng);
  // If there's an alpha channel, bake it.
  // (We're doing everything in proper alpha, even though the final values are
  // interpreted as linear values, because... because.
  png_color_16 my_background = {0, 0, 0, 0, 0}; // blackness!
  png_color_16p image_background;
  if(png_get_bKGD(libpng, info, &image_background))
    png_set_background(libpng, image_background, PNG_BACKGROUND_GAMMA_FILE, 1, 1.0);
  else
    png_set_background(libpng, &my_background, PNG_BACKGROUND_GAMMA_SCREEN, 0, 1.0);
  // For a color image, convert to grayscale.
  if(color_type & PNG_COLOR_MASK_COLOR)
    png_set_rgb_to_gray_fixed(libpng, 1, 21260, 71520);
  // For a less-than-8-bit grayscale image, convert it to 8-bit.
  // (Not a thing anymore? Huh?)
  //if(color_type == PNG_COLOR_TYPE_GRAY && depth < 8) png_set_gray_1_2_4_to_8(libpng);
  // For a 16-bit image, strip it down to 8 bits.
  if(depth > 8) png_set_strip_16(libpng);
  // At this point, we should now have 8-bit grayscale data.
  // Load interlaced PNGs properly.
  (void)png_set_interlace_handling(libpng);
  // Inform libpng of our changes and proceed.
  png_read_update_info(libpng, info);
  image->width = width;
  image->height = height;
  // don't check width*height for overflow because... oh well
  image->pixels = std::unique_ptr<uint8_t[]>(new uint8_t[width * height]);
  image->rows = std::unique_ptr<uint8_t*[]>(new uint8_t*[height]);
  uint8_t* rowp = image->pixels.get();
  for(png_uint_32 n = 0; n < height; ++n) {
    image->rows[n] = rowp;
    rowp += width;
  }
  png_read_image(libpng, (png_bytepp)image->rows.get());
  // we tested this, and determined that this code was needed. later this code
  // turned out not to be needed. Ooooookay.
  /*
  // und flip! (PNG and graphics hardware disagree about the meaning of Y)
  for(png_uint_32 n = 0; n < height/2; ++n) {
    uint8_t* temp = image->rows[n];
    image->rows[n] = image->rows[(height-n-1)];
    image->rows[(height-n-1)] = temp;
  }
  */
  png_read_end(libpng, info);
  // no more errors!
  ULoadedGrayPNG* ret = NewObject<ULoadedGrayPNG>();
  ret->image = std::move(image);
  return ret;
}

// Why is this here? Everybody's gotta be somewhere
const float BYTE_TO_FLOAT[256] = {
  0.0f / 255.0f,
  1.0f / 255.0f,
  2.0f / 255.0f,
  3.0f / 255.0f,
  4.0f / 255.0f,
  5.0f / 255.0f,
  6.0f / 255.0f,
  7.0f / 255.0f,
  8.0f / 255.0f,
  9.0f / 255.0f,
  10.0f / 255.0f,
  11.0f / 255.0f,
  12.0f / 255.0f,
  13.0f / 255.0f,
  14.0f / 255.0f,
  15.0f / 255.0f,
  16.0f / 255.0f,
  17.0f / 255.0f,
  18.0f / 255.0f,
  19.0f / 255.0f,
  20.0f / 255.0f,
  21.0f / 255.0f,
  22.0f / 255.0f,
  23.0f / 255.0f,
  24.0f / 255.0f,
  25.0f / 255.0f,
  26.0f / 255.0f,
  27.0f / 255.0f,
  28.0f / 255.0f,
  29.0f / 255.0f,
  30.0f / 255.0f,
  31.0f / 255.0f,
  32.0f / 255.0f,
  33.0f / 255.0f,
  34.0f / 255.0f,
  35.0f / 255.0f,
  36.0f / 255.0f,
  37.0f / 255.0f,
  38.0f / 255.0f,
  39.0f / 255.0f,
  40.0f / 255.0f,
  41.0f / 255.0f,
  42.0f / 255.0f,
  43.0f / 255.0f,
  44.0f / 255.0f,
  45.0f / 255.0f,
  46.0f / 255.0f,
  47.0f / 255.0f,
  48.0f / 255.0f,
  49.0f / 255.0f,
  50.0f / 255.0f,
  51.0f / 255.0f,
  52.0f / 255.0f,
  53.0f / 255.0f,
  54.0f / 255.0f,
  55.0f / 255.0f,
  56.0f / 255.0f,
  57.0f / 255.0f,
  58.0f / 255.0f,
  59.0f / 255.0f,
  60.0f / 255.0f,
  61.0f / 255.0f,
  62.0f / 255.0f,
  63.0f / 255.0f,
  64.0f / 255.0f,
  65.0f / 255.0f,
  66.0f / 255.0f,
  67.0f / 255.0f,
  68.0f / 255.0f,
  69.0f / 255.0f,
  70.0f / 255.0f,
  71.0f / 255.0f,
  72.0f / 255.0f,
  73.0f / 255.0f,
  74.0f / 255.0f,
  75.0f / 255.0f,
  76.0f / 255.0f,
  77.0f / 255.0f,
  78.0f / 255.0f,
  79.0f / 255.0f,
  80.0f / 255.0f,
  81.0f / 255.0f,
  82.0f / 255.0f,
  83.0f / 255.0f,
  84.0f / 255.0f,
  85.0f / 255.0f,
  86.0f / 255.0f,
  87.0f / 255.0f,
  88.0f / 255.0f,
  89.0f / 255.0f,
  90.0f / 255.0f,
  91.0f / 255.0f,
  92.0f / 255.0f,
  93.0f / 255.0f,
  94.0f / 255.0f,
  95.0f / 255.0f,
  96.0f / 255.0f,
  97.0f / 255.0f,
  98.0f / 255.0f,
  99.0f / 255.0f,
  100.0f / 255.0f,
  101.0f / 255.0f,
  102.0f / 255.0f,
  103.0f / 255.0f,
  104.0f / 255.0f,
  105.0f / 255.0f,
  106.0f / 255.0f,
  107.0f / 255.0f,
  108.0f / 255.0f,
  109.0f / 255.0f,
  110.0f / 255.0f,
  111.0f / 255.0f,
  112.0f / 255.0f,
  113.0f / 255.0f,
  114.0f / 255.0f,
  115.0f / 255.0f,
  116.0f / 255.0f,
  117.0f / 255.0f,
  118.0f / 255.0f,
  119.0f / 255.0f,
  120.0f / 255.0f,
  121.0f / 255.0f,
  122.0f / 255.0f,
  123.0f / 255.0f,
  124.0f / 255.0f,
  125.0f / 255.0f,
  126.0f / 255.0f,
  127.0f / 255.0f,
  128.0f / 255.0f,
  129.0f / 255.0f,
  130.0f / 255.0f,
  131.0f / 255.0f,
  132.0f / 255.0f,
  133.0f / 255.0f,
  134.0f / 255.0f,
  135.0f / 255.0f,
  136.0f / 255.0f,
  137.0f / 255.0f,
  138.0f / 255.0f,
  139.0f / 255.0f,
  140.0f / 255.0f,
  141.0f / 255.0f,
  142.0f / 255.0f,
  143.0f / 255.0f,
  144.0f / 255.0f,
  145.0f / 255.0f,
  146.0f / 255.0f,
  147.0f / 255.0f,
  148.0f / 255.0f,
  149.0f / 255.0f,
  150.0f / 255.0f,
  151.0f / 255.0f,
  152.0f / 255.0f,
  153.0f / 255.0f,
  154.0f / 255.0f,
  155.0f / 255.0f,
  156.0f / 255.0f,
  157.0f / 255.0f,
  158.0f / 255.0f,
  159.0f / 255.0f,
  160.0f / 255.0f,
  161.0f / 255.0f,
  162.0f / 255.0f,
  163.0f / 255.0f,
  164.0f / 255.0f,
  165.0f / 255.0f,
  166.0f / 255.0f,
  167.0f / 255.0f,
  168.0f / 255.0f,
  169.0f / 255.0f,
  170.0f / 255.0f,
  171.0f / 255.0f,
  172.0f / 255.0f,
  173.0f / 255.0f,
  174.0f / 255.0f,
  175.0f / 255.0f,
  176.0f / 255.0f,
  177.0f / 255.0f,
  178.0f / 255.0f,
  179.0f / 255.0f,
  180.0f / 255.0f,
  181.0f / 255.0f,
  182.0f / 255.0f,
  183.0f / 255.0f,
  184.0f / 255.0f,
  185.0f / 255.0f,
  186.0f / 255.0f,
  187.0f / 255.0f,
  188.0f / 255.0f,
  189.0f / 255.0f,
  190.0f / 255.0f,
  191.0f / 255.0f,
  192.0f / 255.0f,
  193.0f / 255.0f,
  194.0f / 255.0f,
  195.0f / 255.0f,
  196.0f / 255.0f,
  197.0f / 255.0f,
  198.0f / 255.0f,
  199.0f / 255.0f,
  200.0f / 255.0f,
  201.0f / 255.0f,
  202.0f / 255.0f,
  203.0f / 255.0f,
  204.0f / 255.0f,
  205.0f / 255.0f,
  206.0f / 255.0f,
  207.0f / 255.0f,
  208.0f / 255.0f,
  209.0f / 255.0f,
  210.0f / 255.0f,
  211.0f / 255.0f,
  212.0f / 255.0f,
  213.0f / 255.0f,
  214.0f / 255.0f,
  215.0f / 255.0f,
  216.0f / 255.0f,
  217.0f / 255.0f,
  218.0f / 255.0f,
  219.0f / 255.0f,
  220.0f / 255.0f,
  221.0f / 255.0f,
  222.0f / 255.0f,
  223.0f / 255.0f,
  224.0f / 255.0f,
  225.0f / 255.0f,
  226.0f / 255.0f,
  227.0f / 255.0f,
  228.0f / 255.0f,
  229.0f / 255.0f,
  230.0f / 255.0f,
  231.0f / 255.0f,
  232.0f / 255.0f,
  233.0f / 255.0f,
  234.0f / 255.0f,
  235.0f / 255.0f,
  236.0f / 255.0f,
  237.0f / 255.0f,
  238.0f / 255.0f,
  239.0f / 255.0f,
  240.0f / 255.0f,
  241.0f / 255.0f,
  242.0f / 255.0f,
  243.0f / 255.0f,
  244.0f / 255.0f,
  245.0f / 255.0f,
  246.0f / 255.0f,
  247.0f / 255.0f,
  248.0f / 255.0f,
  249.0f / 255.0f,
  250.0f / 255.0f,
  251.0f / 255.0f,
  252.0f / 255.0f,
  253.0f / 255.0f,
  254.0f / 255.0f,
  255.0f / 255.0f,
};
