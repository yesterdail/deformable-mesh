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
      Int32Rect rect = new Int32Rect(0, 0, width, height);
      WriteableBitmap imagesrc = new WriteableBitmap(width, height, 96, 96, PixelFormats.Rgb24, null);
      int stride = imagesrc.PixelWidth * 3;
      imagesrc.WritePixels(rect, buffer, stride, 0);
      return imagesrc as ImageSource;
    }
  }
}
