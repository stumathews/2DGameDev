using System;
using System.Globalization;
using System.Windows.Data;
using System.Windows.Media;

namespace GameEditor.Utils
{
    [ValueConversion(typeof(ImageSource), typeof(string))]
    internal class FilenameConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var filename = (string)value;

            return filename != null && filename.EndsWith(".png") ? new ImageSourceConverter().ConvertFromString("C:\\repos\\2DGameDev\\" + filename) : null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotSupportedException();
        }
    }
}
