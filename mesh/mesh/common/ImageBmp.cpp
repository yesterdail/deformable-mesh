#include "ImageBmp.h"
#include "Image.h"

#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <assert.h>
#include <algorithm>
#include <tchar.h>

namespace hj
{
  static bool SwapChannels(uint8_t *pixels, Pixel::Type pixel_type,
    int pixel_count)
  {
    if (!pixels || pixel_count <= 0) {
      assert(0);
      return false;
    }
    int channel_count = Pixel::GetChannelCount(pixel_type);
    if (channel_count < 3)  return false;
    for (int i = 0; i < pixel_count; i++) {
      uint8_t tmp = pixels[2];
      pixels[2] = pixels[0];
      pixels[0] = tmp;
      pixels += channel_count;
    }
    return true;
  }//-----------

  BmpImageIOImplement::BmpImageIOImplement(void)
  {
    AddSupportedExtension(std::string("BMP"));
    AddSupportedExtension(std::string("bmp"));
    AddSupportedExtension(std::string("bitmap"));
    AddSupportedExtension(std::string("BITMAP"));
    AddSupportedExtension(std::string("dib"));
    AddSupportedExtension(std::string("DIB"));
    return;
  }


  BmpImageIOImplement::~BmpImageIOImplement(void)
  {
  }

  BaseImageIOImplement* BmpImageIOImplement::NewIO() const
  {
    return new BmpImageIOImplement();
  }

  bool BmpImageIOImplement::Read(const std::string &file_name, Image *dst_ptr)
  {
    BITMAPFILEHEADER file_hdr;
    int file_handle = ParseFileHeader(file_name, &file_hdr);
    if (file_handle < 0)  return false;
    // Retrieves information header size and parses the header.
    unsigned long info_hdr_size = 0;
    _lseek(file_handle, sizeof(file_hdr), SEEK_SET);
    _read(file_handle, &info_hdr_size, sizeof(info_hdr_size));
    glm::ivec2 img_size(0, 0);
    glm::dvec2 img_unit(1, 1);
    int bit_count = ParseInfoHeader(file_handle, info_hdr_size, img_size,
      img_unit);
    if (bit_count <= 0) {
      _close(file_handle);
      return false;
    }
    bool top_down = false;
    if (img_size.y < 0) {
      img_size.y = -img_size.y;
      top_down = true;
    }
    // Reads palette data, determines pixel type
    RGBQUAD color_table[256];
    int palette_size = file_hdr.bfOffBits - sizeof(file_hdr)
      - info_hdr_size;
    if (palette_size > 0) {
      _lseek(file_handle, sizeof(file_hdr) + info_hdr_size, SEEK_SET);
      _read(file_handle, color_table, palette_size);
    }
    Pixel::Type pixel_type = CheckPixelTypeForReading(bit_count, color_table);
    if (pixel_type == Pixel::kUnknown) {
      _close(file_handle);
      return false;
    }
    // Creates Image object accordingly and copies palette data if necessary.
    dst_ptr->Create(img_size, pixel_type, NULL);
    if (pixel_type == Pixel::kPalette8u) {
      SetPalette(color_table, bit_count, dst_ptr);
    }
    // Reads pixel data to a temporary buffer.
    _lseek(file_handle, file_hdr.bfOffBits, SEEK_SET);
    bool result = true;
    int bytes_per_line = BmpBytesPerLine(bit_count, img_size.x);
    int data_size = bytes_per_line*img_size.y;
    if (dst_ptr->GetDataSize() == data_size && top_down) {
      _read(file_handle, dst_ptr->GetPixels(), data_size);
      SwapChannels(static_cast<uint8_t*>(dst_ptr->GetPixels()), pixel_type,
        img_size.x*img_size.y);
    } else {
      uint8_t *pixels = new uint8_t[data_size];
      _read(file_handle, pixels, data_size);
      // Reads pixel data to Image object.
      switch (bit_count) {
      case 1:
        result = Read1BitPixels(pixels, bytes_per_line, top_down, dst_ptr);
        break;

      case 4:
        result = Read4BitPixels(pixels, bytes_per_line, top_down, dst_ptr);
        break;

      case 8:
      case 24:
      case 32:
        result = ReadNormalPixels(pixels, bytes_per_line, top_down, dst_ptr);
        break;

      default:
        break;
      }
      delete[] pixels;
    }
    _close(file_handle);
    return result;
  }//-------------

  bool BmpImageIOImplement::Write(const std::string &file_name, const Image &src,
    int /*quality*/)
  {  
    if (!CheckImageForWriting(src))  return false;
    // Retrieves palette information.
    RGBQUAD color_table[256];
    int palette_size = GetPalette(src, color_table);
    if (palette_size < 0)  return false;
    // Retrieves dimension of the image.
    glm::ivec2 size(src.GetSize());
    int width = size.x;
    int height = size.y;
    Pixel::Type pixel_type = src.GetPixelType();
    int bit_count = Pixel::GetBitCount(pixel_type);
    int bmp_pixel_data_size = BmpBytesPerLine(bit_count, width)*height;
    // Crates BMP file.
    int file_handle = -1;
    if (_tsopen_s(&file_handle, file_name.c_str(), 
      _O_BINARY | _O_CREAT | _O_RDWR,
      _SH_DENYRW, _S_IREAD | _S_IWRITE)) {
        assert(0);
        return false;
    }
    bool result = true;
    // Configures bitmap file header and writes to file.
    BITMAPFILEHEADER file_hdr = {'MB', 0, 0, 0, 0};
    file_hdr.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
      + palette_size + bmp_pixel_data_size;
    file_hdr.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER)
      + palette_size;
    result = 
      (_write(file_handle, &file_hdr, sizeof(file_hdr)) == sizeof(file_hdr));
    if (result) {   // Configures bitmap info header and writes to file
      BITMAPINFOHEADER info_hdr = {40, 1, 1, 1, 8, 0, 0, 0, 0, 0, 0};
      info_hdr.biWidth = width;
      info_hdr.biHeight = -height;
      info_hdr.biBitCount = (WORD)bit_count;
      info_hdr.biCompression = BI_RGB;
      glm::dvec2 unit(1);
      info_hdr.biXPelsPerMeter = static_cast<long>(1000 / unit.x + 0.5);
      info_hdr.biYPelsPerMeter = static_cast<long>(1000 / unit.y + 0.5);
      result = (_write(file_handle, &info_hdr, sizeof(info_hdr)) == sizeof(info_hdr));
    }
    // Writes palette to file.
    if (result && palette_size > 0) {
      result = (_write(file_handle, color_table, palette_size) == palette_size);
    }
    // Retrieves pixel data and writes to file.
    if (result)  result = WritePixels(src, file_handle);
    _close(file_handle);
    return result;
  }//-------------

  int BmpImageIOImplement::ParseFileHeader(const std::string &file_name,
    BITMAPFILEHEADER *ptr_file_header)
  {
    if (file_name.empty()) {
      assert(0);
      return -1;
    }
    // Opens BMP file.
    int file_handle = -1;
    if (_tsopen_s(&file_handle, file_name.c_str(), 
      _O_BINARY | _O_CREAT | _O_RDWR,
      _SH_DENYRW, _S_IREAD | _S_IWRITE)) {
        assert(0);
        return -1;
    }
    // Loads bitmap file header and checks bitmap file identifier.
    if (_read(file_handle, ptr_file_header, sizeof(BITMAPFILEHEADER)) <= 0) {
      _close(file_handle);
      assert(0);
      return -1;
    }
    if (ptr_file_header->bfType != 0x4D42) {  // This is not a BMP file.
      _close(file_handle);
      assert(0);
      return -1;
    }
    return file_handle;
  }//------------------

  int BmpImageIOImplement::ParseInfoHeader(int file_handle, int info_header_size,
    glm::ivec2 &img_size, glm::dvec2 &img_unit)
  {
    int real_bit_count = 0;
    bool compressed = false;
    _lseek(file_handle, sizeof(BITMAPFILEHEADER), SEEK_SET);
    switch (info_header_size) {
    case sizeof(BITMAPCOREHEADER):
      BITMAPCOREHEADER core_hdr;
      _read(file_handle, &core_hdr, sizeof(core_hdr));
      img_size.x = static_cast<int>(core_hdr.bcWidth);
      img_size.y = static_cast<int>(core_hdr.bcHeight);
      real_bit_count = static_cast<int>(core_hdr.bcBitCount);
      break;

    case sizeof(BITMAPINFOHEADER):
      BITMAPINFOHEADER info_hdr;
      _read(file_handle, &info_hdr, sizeof(info_hdr));
      img_size.x = static_cast<int>(info_hdr.biWidth);
      img_size.y = static_cast<int>(info_hdr.biHeight);
      real_bit_count = static_cast<int>(info_hdr.biBitCount);
      compressed = (info_hdr.biCompression != BI_RGB);
      if (info_hdr.biXPelsPerMeter > 0)  img_unit.x = 1000.0/info_hdr.biXPelsPerMeter;
      if (info_hdr.biYPelsPerMeter > 0)  img_unit.y = 1000.0/info_hdr.biYPelsPerMeter;
      break;

    case sizeof(BITMAPV4HEADER):
      BITMAPV4HEADER v4_hdr;
      _read(file_handle, &v4_hdr, sizeof(v4_hdr));
      img_size.x = static_cast<int>(v4_hdr.bV4Width);
      img_size.y = static_cast<int>(v4_hdr.bV4Height);
      real_bit_count = static_cast<int>(v4_hdr.bV4BitCount);
      compressed = (v4_hdr.bV4V4Compression != BI_RGB);
      if (v4_hdr.bV4XPelsPerMeter > 0)  img_unit.x = 1000.0/v4_hdr.bV4XPelsPerMeter;
      if (v4_hdr.bV4YPelsPerMeter > 0)  img_unit.y = 1000.0/v4_hdr.bV4YPelsPerMeter;
      break;

    default:
      assert(0);
      return 0;
    }
    if (compressed) {
      assert(0);
      return 0;
    }
    return real_bit_count;
  }//---------------------

  Pixel::Type BmpImageIOImplement::CheckPixelTypeForReading(int bit_count,
    const RGBQUAD color_table[256])
  {
    int color_count = 256;
    switch (bit_count) {
    case 1:
      return Pixel::kGrey8u;

    case 4:
      return Pixel::kPalette8u;

    case 8:
      break;

    case 24:
      return Pixel::kRGB8u;

    case 32:
      return Pixel::kRGBA8u;

    default:
      return Pixel::kUnknown;
    }
    int r = 0, g = 0, b = 0;
    int grey_entry = 0;
    for (int i = 0; i < color_count; i++) {
      r = (color_table + i)->rgbRed;
      g = (color_table + i)->rgbGreen;
      b = (color_table + i)->rgbBlue;
      if (r == g && r == b)  grey_entry++;
    }
    if (grey_entry == color_count) {
      return Pixel::kGrey8u;
    } else {
      return Pixel::kPalette8u;
    }
  }//--------------------------

  bool BmpImageIOImplement::SetPalette(const RGBQUAD color_table[256],
    int bit_count, Image *dst_ptr )
  {
    int color_count = MIN<int>(256, (1 << bit_count));
    int entry_idx = 0;
    uint8_t palette[256*3];
    for (int i = 0; i < color_count; ++i) {
      entry_idx = 3*i;
      *(palette + entry_idx) = (color_table + i)->rgbRed;
      *(palette + entry_idx + 1) = (color_table + i)->rgbGreen;
      *(palette + entry_idx + 2) = (color_table + i)->rgbBlue;
    }
    return dst_ptr->SetPalette(palette, 3*color_count);
  }//--------------------------------------------------

  bool BmpImageIOImplement::ReadNormalPixels(const uint8_t *bmp_pixels,
    int src_bytes_per_line, bool top_down, Image *dst_ptr)
  {
    assert(bmp_pixels && src_bytes_per_line > 0);
    glm::ivec2 size(dst_ptr->GetSize());
    int width = size.x;
    int height = size.y;
    assert(width > 0 && height > 0);
    Pixel::Type pixel_type = dst_ptr->GetPixelType();
    int dst_bytes_per_line = Pixel::GetByteCount(pixel_type)*width;
    uint8_t *dst_pixels = static_cast<uint8_t*>(dst_ptr->GetPixels());
    if (top_down) {
      for (int j = 0; j < height; j++) {
        memcpy(dst_pixels, bmp_pixels, dst_bytes_per_line);
        bmp_pixels += src_bytes_per_line;
        dst_pixels += dst_bytes_per_line;
      }
    } else {
      bmp_pixels += src_bytes_per_line*(height - 1);
      for (int j = 0; j < height; j++) {
        memcpy(dst_pixels, bmp_pixels, dst_bytes_per_line);
        bmp_pixels -= src_bytes_per_line;
        dst_pixels += dst_bytes_per_line;
      }
    }
    SwapChannels(dst_pixels - dst_bytes_per_line*height,
      pixel_type, width*height);
    return true;
  }//-----------

  bool BmpImageIOImplement::Read4BitPixels(const unsigned char *bmp_pixels, 
    int src_bytes_per_line, bool top_down, Image *dst_ptr)
  {
    assert(bmp_pixels && src_bytes_per_line > 0);
    glm::ivec2 size(dst_ptr->GetSize());
    int width = size.x;
    int height = size.y;
    assert(width > 0 && height > 0);
    int dst_bytes_per_line = Pixel::GetByteCount(dst_ptr->GetPixelType())*width;
    uint8_t *dst_pixels = static_cast<uint8_t*>(dst_ptr->GetPixels());
    uint8_t src_val = 0, dst_val = 0;
    if (top_down) {
      for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
          src_val = *(bmp_pixels + (j >> 1));
          dst_val = (j%2? (src_val & 0x0f) : (src_val >> 4));
          *(dst_pixels + j) = dst_val;
        }
        bmp_pixels += src_bytes_per_line;
        dst_pixels += dst_bytes_per_line;
      } 
    } else {
      bmp_pixels += src_bytes_per_line*(height - 1);
      for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
          src_val = *(bmp_pixels + (j >> 1));
          dst_val = (j%2? (src_val & 0x0f) : (src_val >> 4));
          *(dst_pixels + j) = dst_val;
        }
        bmp_pixels -= src_bytes_per_line;
        dst_pixels += dst_bytes_per_line;
      }
    }
    return true;
  }//-----------

  bool BmpImageIOImplement::Read1BitPixels(const uint8_t *bmp_pixels,
    int src_bytes_per_line, bool top_down, Image *dst_ptr)
  {
    assert(bmp_pixels && src_bytes_per_line > 0);
    glm::ivec2 size(dst_ptr->GetSize());
    int width = size.x;
    int height = size.y;
    assert(width > 0 && height > 0);
    int dst_bytes_per_line = Pixel::GetByteCount(dst_ptr->GetPixelType())*width;
    uint8_t *dst_pixels = static_cast<uint8_t*>(dst_ptr->GetPixels());
    uint8_t src_val = 0;
    int bit_pos = 0;
    if (top_down) {
      for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
          src_val = *(bmp_pixels + (j >> 3));
          bit_pos = 8 - j%8;
          *(dst_pixels + j) = ((src_val >> (bit_pos - 1)) & 0x1)?
            255 : 0;
        }
        bmp_pixels += src_bytes_per_line;
        dst_pixels += dst_bytes_per_line;
      }
    } else {
      bmp_pixels += src_bytes_per_line*(height - 1);
      for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
          src_val = *(bmp_pixels + (j >> 3));
          bit_pos = 8 - j%8;
          *(dst_pixels + j) = ((src_val >> (bit_pos - 1)) & 0x1)?
            255 : 0;
        }
        bmp_pixels -= src_bytes_per_line;
        dst_pixels += dst_bytes_per_line;
      }
    }
    return true;
  }//-----------

  int BmpImageIOImplement::GetPalette(const Image &image,
    RGBQUAD color_table[256])
  {
    const uint8_t *palette = NULL;
    switch (image.GetPixelType()) {
    case Pixel::kGrey8u:
      for (int i = 0; i < 256; i++) {
        color_table[i].rgbRed = color_table[i].rgbGreen
          = color_table[i].rgbBlue = (BYTE)i;
        color_table[i].rgbReserved = 255;
      }
      return 256*sizeof(RGBQUAD);

    case Pixel::kPalette8u:
      palette = image.GetPalette();
      if (!palette) {
        assert(0);
        return -1;
      }
      for (int i = 0; i < 256; i++) {
        color_table[i].rgbRed = *palette;
        color_table[i].rgbGreen = *(palette + 1);
        color_table[i].rgbBlue = *(palette + 2);
        color_table[i].rgbReserved = 255;
        palette += 3;
      }
      return 256*sizeof(RGBQUAD);

    default:
      return 0;
    }
  }//-----------

  bool BmpImageIOImplement::CheckImageForWriting(const Image &src)
  {
    if (!BaseImageIOImplement::CheckImageForWriting(src))  return false;
    switch (src.GetPixelType()) {
    case Pixel::kGrey8u:
    case Pixel::kPalette8u:
    case Pixel::kRGB8u:
    case Pixel::kRGBA8u:
      return true;
    default:
      return false;
    }
  }//--------------

  bool BmpImageIOImplement::WritePixels(const Image &src_img, int file_handle)
  {
    assert(file_handle >= 0);
    glm::ivec2 src_size(src_img.GetSize());
    int width = src_size.x;
    int height = src_size.y;
    assert(width > 0 && height > 0);
    Pixel::Type pixel_type = src_img.GetPixelType();
    int bmp_bytes_per_line = BmpBytesPerLine(
      Pixel::GetBitCount(pixel_type), width);
    int bmp_pixel_data_size = bmp_bytes_per_line*height;
    const uint8_t *src_pixels = static_cast<const uint8_t*>(
      src_img.GetPixels());
    SwapChannels(const_cast<uint8_t*>(src_pixels), pixel_type, width*height);
    bool result = true;
    if (src_img.GetDataSize() == bmp_pixel_data_size) {
      result = (_write(file_handle, src_pixels, bmp_pixel_data_size) > 0);
    } else {
      uint8_t padding[3] = {0, 0, 0};
      int src_bytes_per_line = Pixel::GetByteCount(src_img.GetPixelType())*width;
      for (int j = 0; j < height; j++) {
        result &= (_write(file_handle, src_pixels, src_bytes_per_line) > 0);
        src_pixels += src_bytes_per_line;
        result &= (_write(file_handle, padding,
          bmp_bytes_per_line - src_bytes_per_line) > 0);
        if (!result)  break;
      }
      src_pixels = static_cast<const uint8_t*>(src_img.GetPixels());
    }
    SwapChannels(const_cast<uint8_t*>(src_pixels), pixel_type, width*height);
    return result;
  }//-------------

}
