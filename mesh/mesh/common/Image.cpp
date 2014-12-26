#include "Image.h"
#include "GLTexture.h"
#include <algorithm>

namespace hj
{
  Image::Image()
    : pixel_type_(Pixel::kUnknown)
    , data_size_(0)
    , size_(0, 0)
    , step_(0, 0)
    , pixel_data_(NULL)
    , texture_ptr_(NULL)
  {

  }

  Image::~Image()
  {
    DEL_ARRAY(pixel_data_);
    DEL_PTR(texture_ptr_);
  }

  bool Image::Create(const glm::ivec2 &size,
    const Pixel::Type type,
    const void *pixels)
  {
    int old_size = GetDataSize();
    int info_updated = UpdateInfo(size[0],
      size[1],
      type);
    if (info_updated < 0) {
      return false;
    }
    // Deletes old pixel & palette data.
    if (GetDataSize() != old_size) {
      DEL_ARRAY(pixel_data_);
      pixel_data_ = new int8_t[GetDataSize()];
    }
    if (pixels) {
      memcpy(pixel_data_, pixels, GetDataSize());
    }
    else {
      memset(pixel_data_, 0, GetDataSize());
    }
    return true;
  }

  bool Image::CreateTexture()
  {
    DEL_PTR(texture_ptr_);
    int32_t texFormat, dataFormat, dataType;
    if (!ConvertPixelType(GetPixelType(), texFormat, dataFormat, dataType)) {
      assert(0);
      return false;
    }
    texture_ptr_ = new GLTexture((GLubyte*)(GetPixels()), glm::ivec3(GetSize(), 1),
      texFormat, dataFormat, dataType);
    return true;
  }

  int Image::UpdateInfo(int width, int height,
    Pixel::Type pixel_type)
  {
    if (width <= 0 || height <= 0 ) {
      assert(0);
      return -1;  // Returns negative if input parameters are invalid.
    }
    if (size_[0] == width && size_[1] == height
      && pixel_type_ == pixel_type)  return 0;  // Returns 0 if image information are unchanged.
    // Otherwise, updates image information accordingly.
    size_.x = width;
    size_.y = height;
    step_.x = Pixel::GetChannelCount(pixel_type);
    step_.y = size_[0] * step_[0];
    pixel_type_ = pixel_type;
    data_size_ = size_[0] * size_[1] * Pixel::GetByteCount(pixel_type);
    if (pixel_type == Pixel::kPalette8u
      || pixel_type == Pixel::kPalette16u) {
      palette_.resize(3 * (1 << Pixel::GetBitCount(pixel_type)));
    }
    else {
      palette_.clear();
    }
    return 1;
  }

  bool Image::SetPalette(const uint8_t *palette, int palette_size)
  {
    if (pixel_type_ != Pixel::kPalette8u
      && pixel_type_ != Pixel::kPalette16u)  return false;
    if (palette_.empty())  return false;
    // Copies palette data if available.
    if (palette) {
      if (palette_size <= 0) {
        assert(0);
        return false;
      }
      memcpy(&(palette_[0]),
        palette,
        std::min(palette_.size(), (size_t)palette_size));
    }
    else {
      std::vector<uint8_t>::iterator color_iter = palette_.begin();
      for (; color_iter != palette_.end(); ++color_iter) {
        *color_iter = 0;
      }
    }
    return true;
  }

  const uint8_t* Image::GetPalette() const
  {
    return palette_.empty() ? NULL : &(palette_[0]);
  }

  int Image::GetPaletteSize() const
  {
    return static_cast<int>(palette_.size());
  }

  bool Image::ConvertPixelType(Pixel::Type pixel_type,
    int32_t &texFormat, int32_t& dataFormat, int32_t& dataType)
  {
    texFormat = GL_R8;//;GL_LUMINANCE
    dataFormat = GL_RED;//;GL_LUMINANCE
    dataType = GL_UNSIGNED_BYTE;
    switch (pixel_type) {
    case Pixel::kGrey8i:
      dataType = GL_BYTE;
      return true;
    case Pixel::kPalette8u:
    case Pixel::kGrey8u:
      return true;
    case Pixel::kPalette16u:
    case Pixel::kGrey16u:
      texFormat = GL_R16;
      dataType = GL_UNSIGNED_SHORT;
      return true;
    case Pixel::kGrey16i:
      texFormat = GL_R16;
      dataType = GL_SHORT;
      return true;
    case Pixel::kRGB8u:
      texFormat = GL_RGB8;
      dataFormat = GL_RGB;
      return true;
    case Pixel::kRGBA8u:
      texFormat = GL_RGBA8;
      dataFormat = GL_RGBA;
      return true;
    case Pixel::kRGB16u:
      texFormat = GL_RGB16;
      dataType = GL_UNSIGNED_SHORT;
      dataFormat = GL_RGB;
    default:
      assert(0);
      return false;
    }
  }

  GLTexture* Image::GetTexture() const
  {
    if (!texture_ptr_) {
      assert(0);
      return NULL;
    }
    return texture_ptr_;
  }
}