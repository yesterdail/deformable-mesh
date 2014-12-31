#include "ImageIO.h"
#include "IOUtilities.h"
#include "ImageBmp.h"

namespace hj
{
  struct ImageIOInitializer
  {
    ImageIOInitializer()
    {
      array_.push_back(new BmpImageIOImplement);
    }

    ~ImageIOInitializer()
    {
      for(size_t i = 0; i < array_.size(); ++i) {
        if(array_[i]) {
          delete array_[i];
          array_[i] = NULL;
        }
      }
      array_.clear();
    }

    std::vector<BaseImageIOImplement*> array_;
  };

  static ImageIOInitializer image_ios;

  BaseImageIOImplement* ImageIO::FindIO(const std::string &extension)
  {
    for (size_t i = 0; i < image_ios.array_.size(); ++i)
    {
      if(image_ios.array_[i]->IsSupported(extension))
        return image_ios.array_[i]->NewIO();
    }
    return NULL;
  }

  bool ImageIO::Read(const std::string &file_name, Image *dst_ptr)
  {
    std::string extension(IOUtilities::GetExtension(file_name));
    if (extension.empty()) {  // Can't determine file extension.
      assert(0);
      return false;
    }
    return Read(file_name, extension, dst_ptr);
  }//--------------------

  bool ImageIO::Read(const std::string &file_name,
    const std::string &extension,
    Image *dst_ptr)
  {
    BaseImageIOImplement* io = ImageIO::FindIO(extension);
    if(!io) {
      assert(0);
      return false;
    }
    return io->Read(file_name, dst_ptr);
  }//-----------------

  bool ImageIO::Write(const std::string &file_name, const Image &src)
  {
    std::string extension(IOUtilities::GetExtension(file_name));
    if (extension.empty()) {
      assert(0);
      return false;
    }
    return Write(file_name, src, extension, 100);
  }//--------------------------------------------

  bool ImageIO::Write(const std::string &file_name, const Image &src,
    const std::string &extension, int quality)
  {
    BaseImageIOImplement* io = ImageIO::FindIO(extension);
    if(!io) {
      assert(0);
      return false;
    }
    return io->Write(file_name, src, quality);
  }//-----------------




}
