using System;
using System.Windows;
using System.Windows.Media;
using System.Windows.Media.Imaging;

namespace mesheditor.Mesh.Helpers
{
  class ByteImageConverter
  {
    /// <summary>
    /// convert from a buffer to a ImageSource
    /// </summary>
    /// <returns></returns>
    public static ImageSource ConvertFrom(byte[] buffer, int width, int height)
    {
      PixelFormat pf = PixelFormats.Rgb24;
      int stride = (width * pf.BitsPerPixel + 7) / 8;
      BitmapSource bitmapSource = BitmapSource.Create(width, height, 96, 96, pf, null, buffer, stride);
      return bitmapSource;
    }
  }
}
