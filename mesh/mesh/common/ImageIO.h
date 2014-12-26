#ifndef HJ_IMAGEIO_H_
#define HJ_IMAGEIO_H_

#include <vector>
#include "Config.h"

namespace hj
{
  class Plugin;
  class Image;
  class BaseImageIOImplement;
  using std::vector;

  class ImageIO
  {
  public:
    /**
    * Reads data from an image file.
    * @param file_name: Name of the image file to be read.
    * @param dst_ptr: Pointer to an object receiving image data.
    * @return: True if successful, false otherwise.
    * @remarks: Image data format is deduced from the file name extension. If the specified file name contains no extension, reading fails.
    */
    static bool Read(const std::string &file_name, Image *dst_ptr);

    /**
    * Reads data from a image file.
    * @param file_name: Name of the image file to be read.
    * @param extension:	Extension of the file to be read which indicates image format.
    * @param dst_ptr: Pointer to an object receiving image data.
    * @return: True if successful, false otherwise.
    */
    static bool Read(const std::string &file_name,
      const std::string &extension,
      Image *dst_ptr);

    /**
    * Writes image data to a file.
    * @param file_name: Name of the file to be written.
    * @param src: Reference to the object containing image data.
    * @return: True if successful, false otherwise.
    * @remarks: Image data format is deduced from the file name extension. If the specified file name contains no extension, writing fails.
    * @remarks: Image quality percentage will be defaulted to 100 for lossy formats.
    */
    static bool Write(const std::string &file_name, const Image &src);

    /**
    * Writes image data to a file.
    * @param file_name: Name of the file to be written.
    * @param src: Reference to the object containing image data.
    * @param extension: Extension of the file to be written which indicates image format.
    * @param quality: Image quality percentage. It's used for lossy formats.
    * @return: True if successful, false otherwise.
    */
    static bool Write(const std::string &file_name,
      const Image &src,
      const std::string &extension,
      int quality);

  private:
    static BaseImageIOImplement* FindIO(const std::string &extension);
  };
}

#endif // HJ_IMAGEIO_H_
