using System;
using System.Globalization;
using System.Windows.Data;

namespace mesheditor.Mesh
{
  /// <summary>
  /// Convert ToolType to bool.
  /// Can be used to check active tool button/menu item
  /// in client application.
  /// ConverterParameter should be string representation 
  /// of the button tool type ("Pointer", "Line" etc.)
  /// </summary>
  [ValueConversion(typeof(ToolType), typeof(bool))]
  public class ToolTypeConverter : IValueConverter
  {
    public object Convert(object value, Type targetType,
                  object parameter, CultureInfo culture)
    {
      string name = Enum.GetName(typeof(ToolType), value);

      return (name == (string)parameter);
    }

    public object ConvertBack(object value, Type targetType,
                              object parameter, CultureInfo culture)
    {
      return new NotSupportedException(this.GetType().Name + "ConvertBackNotSupported");
    }
  }
}
