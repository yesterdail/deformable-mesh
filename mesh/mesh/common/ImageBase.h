#ifndef HJ_FMT_BASE_H_
#define HJ_FMT_BASE_H_

#include "Image.h"
#include <vector>
#include <string>

namespace hj
{
  class BaseImageIOImplement
  {
  public:

    /**
    * Constructor.
    * @param service: Reference to name string of supported image format.
    */
    BaseImageIOImplement(void);

    /**
    * Destructor.
    */
    virtual ~BaseImageIOImplement(void);

    /**
    * Reads data from an image file.
    * @param file_name: Name of the image file to be read.
    * @param dst_ptr: Pointer to an object receiving image data.
    * @return: True if successful, false otherwise. 
    */
    virtual bool Read(const std::string &file_name, Image *dst_ptr) = 0;

    /**
    * Writes image data to a file.
    * @param file_name: Name of the file to be written.
    * @param src: Reference to the source object containing image data.
    * @param quality: Image quality percentage. It's used for lossy formats.
    * @return: True if successful, false otherwise.
    */
    virtual bool Write(const std::string &file_name, const Image &src,
      int quality) = 0;

    /**
    * return: new a IO object ptr.
    */
    virtual BaseImageIOImplement* NewIO() const;

    /**
    * Returns extensions of supported file formats.
    * @return: Reference to the vector storing extension strings that are supported by this plugin.
    */
    const std::vector<std::string>& GetSupportedExtensions()
    {return supported_extensions_;}

    /**
    * Checks if an image format is supported.
    * @return: True if it's supported, false otherwise.
    */
    bool IsSupported(const std::string &extension);

  protected:
    /**
    * Checks if an object containing image data is valid for writing to a file.
    * @return: True if it's valid, false otherwise.
    */
    virtual bool CheckImageForWriting(const Image &src);

    /**
    * Adds an extension string to the supported extension array.
    * @param extension: Reference to the extension string.
    * @return: True if the extension string is valid, false otherwise.
    */
    bool AddSupportedExtension(const std::string &extension);

  protected:
    /** Extension of file formats that can be read. */
    std::vector<std::string> supported_extensions_;
  };
}

#endif // HJ_FMT_BASE_H_
