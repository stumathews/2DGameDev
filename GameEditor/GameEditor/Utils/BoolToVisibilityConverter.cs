using System;
using System.Globalization;
using System.Windows;
using System.Windows.Data;

namespace GameEditor.Utils
{
    [ValueConversion(typeof(Visibility), typeof(bool))]
    internal class BoolToVisibilityConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var option = (bool)value;
            return option ? Visibility.Visible : Visibility.Hidden;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}