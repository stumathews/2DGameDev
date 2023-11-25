using GameEditor.Utils;
using GameEditor.Views;
using System.Collections.Generic;
using System.Windows;

namespace GameEditor.Windows
{
    /// <summary>
    /// Interaction logic for ContentManager.xaml
    /// </summary>
    public partial class ContentManagerWindow : Window
    {
        // Base path for looking for resources.xml
        private readonly string basePath;

        public ContentManagerWindow(Window parent, string basePath = "C:\\repos\\2DGameDev\\")
        {
            this.basePath = basePath;

            Owner = parent;

            ParseResources();
            InitializeComponent();
            DataContext = this;
                      
        }

        // List of assets that this screen will load up from loading the resources file
        public List<AssetModel> Assets { get; set; } = new List<AssetModel>();

        private void ParseResources()
        {
            // The resource manager will deserialize the resources for us in the resources.xml files
            Assets = ResourceManager.ParseResources(basePath);
        }
    }
}
