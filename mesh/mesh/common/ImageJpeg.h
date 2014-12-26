#ifndef HJ_FMT_JPEG_H_
#define HJ_FMT_JPEG_H_

#include "ImageBase.h"
#include <libjpeg/jpeglib.h>

namespace hj
{
  class JpegImageIOImplement : public BaseImageIOImplement
  {
  public:

    /**
    * Constructor.
    */
    JpegImageIOImplement(void);

    /**
    * Destructor.
    */
    virtual ~JpegImageIOImplement(void);

    /**
    * Reads data from an image file.
    * @param file_name: Name of the image file to be read.
    * @param dst_ptr: Pointer to an object receiving image data.
    * @return: True if successful, false otherwise. 
    */
    virtual bool Read(const std::string &file_name, Image *dst_ptr);

    /**
    * Writes image data to a file.
    * @param file_name: Name of the file to be written.
    * @param src: Reference to the source object containing image data.
    * @param quality: Image quality percentage. It's used for lossy formats.
    * @return: True if successful, false otherwise.
    */
    virtual bool Write(const std::string &file_name, const Image &src, int quality);

    /**
    * return: new a IO object ptr.
    */
    virtual BaseImageIOImplement* NewIO() const;

  private:
    /** Jpeg compression information. */
    struct jpeg_compress_struct compression_info_;

    /** Jpeg compression information. */
    struct jpeg_decompress_struct decompression_info_;

    /** Error mamager. */
    struct jpeg_error_mgr err_mgr_;
  };
}

#endif // HJ_FMT_JPEG_H_
