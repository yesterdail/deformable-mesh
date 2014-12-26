#include "Pixel.h"
#include <cassert>

namespace hj
{
  bool Pixel::GetTypeName(Type type, std::string &name)
  {
    switch (type)	{
    case kGrey8i:
      name = "kGrey8i";
      break;

    case kGrey8u:
      name = "kGrey8u";
      break;

    case kGrey16i:
      name = "kGrey16i";
      break;

    case kGrey16u:
      name = "kGrey16u";
      break;

    case kGrey32i:
      name = "kGrey32i";
      break;

    case kGrey32u:
      name = "kGrey32u";
      break;

    case kGrey32f:
      name = "kGrey32f";
      break;

    case kGrey64f:
      name = "kGrey64f";
      break;
    case kRGB8u:
      name = "kRGB8u";
      break;

    case kRGB16u:
      name = "kRGB16u";
      break;

    case kRGBA8u:
      name = "kRGBA8u";
      break;

    case kRGBA16u:
      name = "kRGBA16u";
      break;

    case kPalette8u:
      name = "kPalette8u";
      break;

    case kPalette16u:
      name = "kPalette16u";
      break;

    default:
      name = "Unknown";
      break;
    }
    return true;
  }

  int Pixel::GetChannelCount(Type type)
  {
    switch (type) {
    case kRGB8u:
    case kRGB16u:
      return 3;

    case kRGBA8u:
    case kRGBA16u:
      return 4;

    default:
      return 1;
    }
  }

  int Pixel::GetByteCount(Type type)
  {
    switch (type)	{
    case kGrey16i:
    case kGrey16u:
    case kPalette16u:
      return 2;

    case kGrey32i:
    case kGrey32u:
    case kGrey32f:
    case kRGBA8u:
      return 4;

    case kGrey64f:
      return 8;

    case kRGB8u:
      return 3;

    case kRGB16u:
      return 6;

    case kRGBA16u:
      return 8;

    default:
      return 1;
    }
  }

  int Pixel::GetBitCount(Type type)
  {
    switch (type)	{
    case kGrey16i:
    case kGrey16u:
    case kPalette16u:
      return 16;

    case kRGB8u:
      return 24;

    case kGrey32i:
    case kGrey32u:
    case kGrey32f:
    case kRGBA8u:
      return 32;

    case kGrey64f:
    case kRGBA16u:
      return 64;

    case kRGB16u:
      return 48;

    default:
      return 8;
    }
  }
}