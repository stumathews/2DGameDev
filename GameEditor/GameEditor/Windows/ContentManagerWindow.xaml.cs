using GameEditor.Utils;
using GameEditor.Views;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.Xml;

namespace GameEditor.Windows
{
    /// <summary>
    /// Interaction logic for ContentManager.xaml
    /// </summary>
    public partial class ContentManagerWindow : Window
    {
        private readonly string _basePath;
        List<AssetModel> assets = new List<AssetModel>();
        public ContentManagerWindow(Window parent, string basePath = "C:\\repos\\2DGameDev\\")
        {
            _basePath = basePath;
            Owner = parent;

            ParseResources();
            InitializeComponent();
            DataContext = this;
                      
        }

        public List<AssetModel> Assets { get => assets; set => assets = value; }

        private void ParseResources()
        {
            Assets = ResourceManager.ParseResources(_basePath);
        }
    }
}
