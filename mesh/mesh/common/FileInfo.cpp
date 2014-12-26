#include "FileInfo.h"
#include <algorithm>
#include <direct.h>

namespace hj
{
  struct MatchPathSeparator
  {
    bool operator()(char ch) const
    {
      return ch == '/';
    }
  };

  std::string FileInfo::basename(std::string const& fullpath)
  {
    return std::string(
      std::find_if(fullpath.rbegin(), fullpath.rend(),
      MatchPathSeparator()).base(),
      fullpath.end());
  }

  std::string FileInfo::absolutpath(std::string const& fullpath)
  {
    return std::string(
      fullpath.begin(),
      std::find_if(fullpath.rbegin(), fullpath.rend(),
      MatchPathSeparator()).base());
  }

  void FileInfo::SetCurrent(std::string const& path)
  {
    _chdir(path.c_str());
  }

}