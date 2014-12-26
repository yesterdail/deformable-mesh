#ifndef HJ_IOUTILITIES_H_
#define HJ_IOUTILITIES_H_

#include <cassert>

namespace hj
{
  class IOUtilities  
  {
  public:
    /**
    * Splits the file name string to get the extension.
    * @param file_name: Reference to the file name string.
    * @return: file extension string extracted from the file name.
    */
    static std::string GetExtension(const std::string &file_name)
    {
      assert(!file_name.empty());
      return file_name.substr(file_name.rfind(std::string(".")) + 1);
    }
  };
}

#endif // hj_IOUTILITIES_H_
