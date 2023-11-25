using GameEditor.ViewModels;
using System.Windows;

namespace GameEditor.Windows
{
    /// <summary>
    /// Interaction logic for NewRoomWindow.xaml
    /// </summary>
    public partial class CreateNewLevelWindow : Window
    {
        public CreateNewLevelWindow(NewLevelViewModel newLevelViewModel)
        {            
            InitializeComponent();
            DataContext = newLevelViewModel;    
            
            newLevelViewModel.Parent = this;
        }
    }
}
