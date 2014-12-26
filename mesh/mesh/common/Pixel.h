#ifndef HJ_Pixel_h__
#define HJ_Pixel_h__

#include <string>

namespace hj
{
  /** Lists all supported pixel type*/
  class Pixel
  {
  public:
    enum Type
    {
      /** Pixel type: Grey with signed 8 bit */
      kGrey8i,
      /** Pixel type: Grey with unsigned 8 bit */
      kGrey8u,
      /** Pixel type: Grey with signed 16 bit */
      kGrey16i,
      /** Pixel type: Grey with unsigned 16 bit */
      kGrey16u,
      /** Pixel type: Grey with signed 32 bit */
      kGrey32i,
      /** Pixel type: Grey with unsigned 32 bit */
      kGrey32u,
      /** Pixel type: Grey with float 32 bit */
      kGrey32f,
      /** Pixel type: Grey with double float 64 bit */
      kGrey64f,
      /** Pixel type: RGB(Red, Green and Blue) with every component unsigned 8 bit */
      kRGB8u,
      /** Pixel type: RGB(Red, Green and Blue) with every component unsigned 16 bit */
      kRGB16u,
      /** Pixel type: RGBA(Red, Green, Blue and Alpha) with every component unsigned 8 bit */
      kRGBA8u,
      /** Pixel type: RGBA(Red, Green, Blue and Alpha) with every component unsigned 16 bit */
      kRGBA16u,
      /** Pixel type: Palette with unsigned 8 bit*/
      kPalette8u,
      /** Pixel type: Palette with unsigned 16 bit*/
      kPalette16u,
      /** Pixel type: Unknown*/
      kUnknown
    };

    /**
    * Retrieves names of the enumerated pixel types.
    * @param type: An enumurated pixel type.
    * @return: A name string of the type.
    */
    static bool GetTypeName(Type type, std::string &name);

    /**
    * Retrieves number of channels of a pixel type.
    * @param type: An enumurated pixel type.
    * @return: Number of channels of this type.
    */
    static int GetChannelCount(Type type);

    /**
    * Retrieves the number of bytes of a pixel type.
    * @param type: An enumerated pixel type.
    * @return: Number of bytes of this type.
    */
    static int GetByteCount(Type type);

    /**
    * Retrieves the number of bits of a pixel type.
    * @param type: An enumerated pixel type.
    * @return: Number of bits of this type.
    */
    static int GetBitCount(Type type);
  };
}
#endif // HJ_Pixel_h__