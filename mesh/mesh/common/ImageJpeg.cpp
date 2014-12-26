#include "ImageJpeg.h"
#include <assert.h>
#include <tchar.h>

namespace hj
{
  JpegImageIOImplement::JpegImageIOImplement(void)
  {
    AddSupportedExtension(std::string("jpg"));
    AddSupportedExtension(std::string("JPG"));
    AddSupportedExtension(std::string("jpeg"));
    AddSupportedExtension(std::string("JPEG"));
    compression_info_.err = jpeg_std_error(&err_mgr_);
    jpeg_create_compress(&compression_info_);
    decompression_info_.err = jpeg_std_error(&err_mgr_);
    jpeg_create_decompress(&decompression_info_);
    return;
  }

  JpegImageIOImplement::~JpegImageIOImplement(void)
  {
    jpeg_destroy_compress(&compression_info_);
    jpeg_destroy_decompress(&decompression_info_);
    return;
  }

  BaseImageIOImplement* JpegImageIOImplement::NewIO() const
  {
    return new JpegImageIOImplement();
  }

  bool JpegImageIOImplement::Read(const std::string &file_name, Image *dst_ptr)
  {
    if (file_name.empty() || !dst_ptr) {
      assert(0);
      return false;
    }
    FILE *in_file = NULL;
    if (_tfopen_s(&in_file, file_name.c_str(), _T("rb"))) {
      _tprintf_s(_T("Can't open file.\n"));
      return false;
    }
    jpeg_stdio_src(&decompression_info_, in_file);
    jpeg_read_header(&decompression_info_, TRUE);
    jpeg_start_decompress(&decompression_info_);
    Pixel::Type pixel_type = Pixel::kRGB8u;
    bool successful = true;
    if (1 == decompression_info_.output_components) {
      pixel_type = Pixel::kGrey8u;
    } else if (3 != decompression_info_.output_components) {
      successful = false;
    }
    if (successful) {
      successful &= dst_ptr->Create(glm::ivec2(decompression_info_.output_width,
        decompression_info_.output_height), pixel_type, NULL);
    }
    if (successful) {
      JSAMPROW row_pointer[1] = {static_cast<JSAMPROW>(dst_ptr->GetPixels())};
      int row_stride = dst_ptr->GetStep().y;
      while (decompression_info_.output_scanline
        < decompression_info_.output_height) {
          jpeg_read_scanlines(&decompression_info_, row_pointer, 1);
          row_pointer[0] += row_stride;

      }
      jpeg_destroy_decompress(&decompression_info_);
    }
    successful &= (fclose(in_file) == 0);
    return successful;
  }

  bool JpegImageIOImplement::Write(const std::string &file_name, const Image &src, int quality)
  {
    if (file_name.empty()) {
      assert(0);
      return false;
    }
    // Checks and sets size.
    glm::ivec2 size(src.GetSize());
    if (size.x <= 0 || size.y <= 0) {
      assert(0);
      return false;
    }
    compression_info_.image_width = size.x;
    compression_info_.image_height = size.y;
    // Checks and sets format.
    Pixel::Type pixel_type = src.GetPixelType();
    if (Pixel::kGrey8u == pixel_type) {
      compression_info_.input_components = 1;
      compression_info_.in_color_space = JCS_GRAYSCALE;
    } else if (Pixel::kRGB8u == pixel_type) {
      compression_info_.input_components = 3;
      compression_info_.in_color_space = JCS_RGB;
    } else {
      return false;
    }
    // Checks and sets pointer to RGB raw data.    
    JSAMPROW row_pointer[1] = {(JSAMPLE*)(src.GetPixels())};
    if (!row_pointer[0]) {
      assert(0);
      return false;
    }
    // Opens file.
    FILE *out_file = NULL;
    if (_tfopen_s(&out_file, file_name.c_str(), _T("wb"))) {
      _tprintf_s(_T("Can't open file.\n"));
      return false;
    }
    jpeg_stdio_dest(&compression_info_, out_file);
    jpeg_set_defaults(&compression_info_);
    jpeg_set_quality(&compression_info_, quality, TRUE);
    int row_stride = src.GetStep().y;
    // Starts compression.
    jpeg_start_compress(&compression_info_, TRUE);
    while (compression_info_.next_scanline < compression_info_.image_height) {
      jpeg_write_scanlines(&compression_info_, row_pointer, 1);
      row_pointer[0] += row_stride;
    }
    jpeg_finish_compress(&compression_info_);
    return fclose(out_file) == 0;
  }

}
