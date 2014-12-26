#ifndef HJ_FileInfo_h__
#define HJ_FileInfo_h__

#include <string>

namespace hj
{
  class FileInfo
  {
  public:

    static std::string basename(std::string const& fullpath);

    static std::string absolutpath(std::string const& fullpath);

    static void SetCurrent(std::string const& path);
  };
}
#endif // HJ_FileInfo_h__