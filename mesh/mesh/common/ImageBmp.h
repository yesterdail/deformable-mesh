#ifndef HJ_FMT_BMP_H_
#define HJ_FMT_BMP_H_

#include "ImageBase.h"
#include <windows.h>

namespace hj
{
  class BmpImageIOImplement : public BaseImageIOImplement
  {
  public:
    /**
    * Default constructor.
    */
    BmpImageIOImplement(void);

    /**
    * Destructor.
    */
    virtual ~BmpImageIOImplement(void);

    /**
    * Reads data from a image file.
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
    virtual bool Write(const std::string &file_name, const Image &src,
      int quality);

    /**
    * return: new a IO object ptr.
    */
    virtual BaseImageIOImplement* NewIO() const;

  private:
    /**
    * Parses bitmap file header, validates some fields.
    * @param file_name: Reference to a string containing the path of the image file.
    * @param ptr_file_header: Pointer to the structure to store the file header.
    * return: File handle to the image file. -1 if it isn't a valid BMP file. 
    */
    int ParseFileHeader(const std::string &file_name,
      BITMAPFILEHEADER *ptr_file_header);

    /**
    * Parses bitmap information header, retrieves some important information about the image.
    * @param file_handle: Opened handle to the file image.
    * @param info_header_size: Size of the information header structure. It's used to check version of the header.
    * @param img_size: Reference to the object receiving width and height of the image.
    * @param img_unit: Reference to the object receiving horizontal & vertical units of the image.
    * @return: Number of bits of each pixel.
    */
    int ParseInfoHeader(int file_handle, int info_header_size,
      glm::ivec2 &img_size, glm::dvec2 &img_unit);

    /**
    * Returns appropriate pixel type according to bit count and color table of BMP file information.
    * @param bit_count: Number of bits of each pixel in the image.
    * @param color_table: Color table read from the BMP file.
    * @return: IVL pixel type.
    */
    Pixel::Type CheckPixelTypeForReading(int bit_count,
      const RGBQUAD color_table[256]);

    /**
    * Returns bytes per line of a BMP file, taking 4-byte padding into account.
    * @param bit_count: Number of bits of each pixel in the image.
    * @param width: Width of the image.
    * @return: Bytes-per-line in the BMP file.
    */
    int BmpBytesPerLine(int bit_count, int width)
    {return ((width*bit_count + 31) >> 5) << 2;}

    /**
    * Reads palette data from BMP file and sets them to the Image object.
    * @param color_table: Memory block storing palette data.
    * @param bit_count: Bits-per-pixel of the BMP image data.
    * @param dst_img: Pointer to the object to which palette data is copied.
    * @return: true if successful, false otherwise.
    */ 
    bool SetPalette(const RGBQUAD color_table[256], int bit_count,
      Image *dst_ptr);

    /**
    * Reads normal pixel data, i.e. each pixel occupies 1 or 3 or 4 bytes.
    * @param bmp_pixels: Pointer to the memory block storing pixel data.
    * @param src_bytes_per_line: Bytes-per-line of the BMP image data.
    * @param top_down: Flag indicating whether it's top-down image or a bottom-up one.
    * @param dst_img: Pointer to the object to which pixel data is copied.
    * @return: true if successful, false otherwise.
    */
    bool ReadNormalPixels(const uint8_t *bmp_pixels, 
      int src_bytes_per_line, bool top_down, Image *dst_ptr);

    /**
    * Reads pixel data of which 4 bits represent 1 pixel.
    * @remarks: Each byte will be decomposed and rearranged to 2 bytes.
    * @param bmp_pixels: Pointer to the memory block storing pixel data.
    * @param src_bytes_per_line: Bytes-per-line of the BMP image data.
    * @param top_down: Flag indicating whether it's top-down image or a bottom-up one.
    * @param dst_ptr: Pointer to the object to which pixel data is copied.
    * @return: true if successful, false otherwise.
    */
    bool Read4BitPixels(const unsigned char *bmp_pixels,
      int src_bytes_per_line, bool top_down, Image *dst_ptr);

    /**
    * Reads pixel data of which 1 bit represents 1 pixel.
    * @remarks: Each byte will be decomposed and rearranged to 8 bytes.
    * @param bmp_pixels: Pointer to the memory block storing pixel data.
    * @param src_bytes_per_line: Bytes-per-line of the BMP image data.
    * @param top_down: Flag indicating whether it's top-down image or a bottom-up one.
    * @param dst_ptr: Pointer to the object to which pixel data is copied.
    * @return: True if successful, false otherwise.
    */
    bool Read1BitPixels(const uint8_t *bmp_pixels,
      int src_bytes_per_line, bool top_down, Image *dst_ptr);

    /**
    * Retrieves palette data from an Image object.
    * @param image: Reference to the image containing the palette.
    * @param color_table: Memory block to receive palette data.
    * @return: Size of palette copied from the image. 0 if this image doesn't contain a palette, -1 if palette data is corrupted.
    */
    int GetPalette(const Image &image, RGBQUAD color_table[256]);

    /**
    * Checks if an object containing image data is valid for writing to a file.
    * @return: True if it's valid, false otherwise.
    */
    virtual bool CheckImageForWriting(const Image &src);

    /**
    * Writes pixel data of an Image object to a file.
    * @param src_img: Reference to the image containing pixel data.
    * @param file_handle: Handle to the file to which pixel data are written to.
    * @return: True if successful, false otherwise.
    */
    bool WritePixels(const Image &src_img, int file_handle);

  };
}

#endif // HJ_FMT_BMP_H_
