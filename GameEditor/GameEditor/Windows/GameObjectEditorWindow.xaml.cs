using GameEditor.ViewModels;
using GameEditor.Views;
using Microsoft.Win32;
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
using System.Xml.Xsl;
using System.Xml;

namespace GameEditor.Windows
{
    /// <summary>
    /// Interaction logic for AddPickupWindow.xaml
    /// </summary>
    public partial class GameObjectEditorWindow : Window
    {
        public GameObjectEditorViewModel ViewModel { get; }
        public ICommand CloseWindowCommand { get;set;}
        public GameObjectEditorWindow(Window window)
        {
            Owner = window;
                       
            InitializeComponent();

            ViewModel = new GameObjectEditorViewModel(this);
            DataContext = ViewModel;

            CloseWindowCommand = new RelayCommand((o) =>
            {
                ViewModel.SaveGameObjectTypes();
                Close();
            });
        }
    }
}
