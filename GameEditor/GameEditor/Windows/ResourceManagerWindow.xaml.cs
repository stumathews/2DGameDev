using GameEditor.Utils;
using GameEditor.Views;
using System.Collections.Generic;
using System.Windows;
using GameEditor.Models;
using GameEditor.ViewModels;

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
            InitializeComponent();
            DataContext = new ResourcesViewModel(basePath);
        }

    }
}
