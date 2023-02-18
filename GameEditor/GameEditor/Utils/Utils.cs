using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace GameEditor.Utils
{
    public static class Utils
    {
        public static int ToNumber(this string str) => int.Parse(str);
        public static string ToBoolString(this Visibility visibility) => visibility == Visibility.Visible ? Boolean.TrueString : Boolean.FalseString;
        public static Visibility ToVisibility(this string str) => (Visibility)Enum.Parse(typeof(Visibility), str);
        public static bool ToBool(this string str) => bool.Parse(str);
        public static Visibility VisibilityFromTruthString(this string truth) => bool.Parse(truth) ? Visibility.Visible : Visibility.Hidden;
    }
}
