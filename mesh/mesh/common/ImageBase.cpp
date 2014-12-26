#include "ImageBase.h"
#include <assert.h>

namespace hj
{
  BaseImageIOImplement::BaseImageIOImplement(void)
  {
  }


  BaseImageIOImplement::~BaseImageIOImplement(void)
  {
  }

  BaseImageIOImplement* BaseImageIOImplement::NewIO() const
  {
    return NULL;
  }

  bool BaseImageIOImplement::IsSupported(const std::string &extension)
  {
    std::vector<std::string>::size_type i = 0;
    bool supported = false;
    for (; i < supported_extensions_.size(); i++) {
      supported = (supported_extensions_[i].compare(extension) == 0);
      if (supported)  break;
    }
    return supported;
  }

  bool BaseImageIOImplement::CheckImageForWriting(const Image &src)
  {
    glm::ivec2 size(src.GetSize());
    const void *pixels = src.GetPixels();
    return size[0] > 0 && size[1] > 0 && pixels;
  }

  bool BaseImageIOImplement::AddSupportedExtension(const std::string &extension)
  {
    if (extension.empty()) {
      assert(0);
      return false;
    }
    supported_extensions_.push_back(extension);
    return true;
  }


}
