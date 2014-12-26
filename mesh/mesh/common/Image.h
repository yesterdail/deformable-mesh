#ifndef HJ_Image_h__
#define HJ_Image_h__

#include "Pixel.h"
#include "glmext.h"
#include "macro.h"
#include <vector>

namespace hj
{
  class GLTexture;

  class Image
  {
  public:
    /**
    * Default constructor.
    */
    Image();

    /**
    * Destructor.
    */
    virtual ~Image();

    /**
    * Creates a layout and stores image information for this image.
    * No memory is allocated yet.
    * @param size: Reference to image size.
    * @param type: Image pixel type.
    * @remarks: For multichannel images, valid_bit_count refers to number of bits in individual channels.
    * @param pixels: Pointer to input pixel data.
    * @return: true if is created successfully, false otherwise.
    */
    bool Create(const glm::ivec2 &size,
      const Pixel::Type type,
      const void *pixels);

    /**
    * Create a texture asociate with current image.
    * @return: true if is created successfully, false otherwise.
    */
    bool CreateTexture();

    /**
    * Returns constant pointer to pixel data.
    * @remarks: User needs to cast it to proper data type while in use.
    * @return: Constant pointer to pixel data.
    */
    virtual const void* GetPixels() const
    {
      return pixel_data_;
    }

    /**
    * Returns pointer to pixel data.
    * @remarks: User needs to cast it to proper data type while using.
    * @return: Pointer to pixel data.
    */
    virtual void* GetPixels()
    {
      return pixel_data_;
    }

    /**
    * Returns the width and height of this image.
    * @return: Size of the image.
    */
    glm::ivec2 GetSize() const
    {
      return size_;
    }

    /**
    * Returns steps in both directions of this image.
    * @return: Steps in both directions of this image.
    */
    glm::ivec2 GetStep() const
    {
      return step_;
    }

    /**
    * Returns pixel type.
    * @return: Pixel type of the image.
    */
    Pixel::Type GetPixelType() const
    {
      return pixel_type_;
    }

    /**
    * Retrieves size in bytes of pixel data stored in this object.
    * @return: Size of pixel data stored in this object.
    */
    int GetDataSize() const
    {
      return data_size_;
    }

    /**
    * Allocates buffer for palette if necessary and copies data for an external buffer if available.
    * @param palette: Pointer to palette data which must be 8-bit unsigned integers.
    * @param palette_size: Size in bytes of the external palette data.
    * return: True if palette is created or copied successfully, false otherwise.
    */
    bool SetPalette(const uint8_t *palette, int palette_size);

    /**
    * Returns constant pointer to the palette data.
    * @return: constant pointer to the palette data. NULL is returned if pixel type doesn't support palette.
    */
    const uint8_t* GetPalette() const;

    /**
    * Returns size in bytes of the palette.
    * @return: Size in bytes of the palette.
    */
    int GetPaletteSize() const;

    GLTexture* GetTexture() const;

  protected:
    /**
    * Updates image information.
    * @param width: Width of the image.
    * @param height: Height of the image.
    * @param pixel_type: Pixel type of the image.
    * @return: Negative if parameters are invalid , 0 if image information are unchanged, positive otherwise.
    */
    int UpdateInfo(int width,
      int height,
      Pixel::Type pixel_type);

    /**
    * Converts a IVL pixel type to corresponding OpenGL formats & types.
    * @param pixel_type: IVL pixel type in this object.
    * @return: True if the specified type is supported by OpenGL, false otherwise.
    */
    bool ConvertPixelType(Pixel::Type pixel_type,
      int32_t& texFormat,
      int32_t& dataFormat,
      int32_t& dataType);

  private:
    /** Width and height of image. */
    glm::ivec2 size_;

    /** Number of data items between 2 adjacent rows or columns. */
    glm::ivec2 step_;

    /** Type of image pixel data. */
    Pixel::Type pixel_type_;

    /** Size of pixel data, in bytes. */
    int data_size_;

    /** Pointer to pixel data. */
    void *pixel_data_;

    /** Palette data. */
    std::vector<uint8_t> palette_;

    GLTexture* texture_ptr_;
  };
}
#endif // HJ_Image_h__