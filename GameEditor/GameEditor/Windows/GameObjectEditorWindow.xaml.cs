using GameEditor.ViewModels;
using System.Windows;
using System.Windows.Input;

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
            ViewModel.Initialize();

            DataContext = ViewModel;

            CloseWindowCommand = new RelayCommand((o) =>
            {
                ViewModel.SaveGameObjectTypes();
                Close();
                
            }, (o)=> ViewModel.IsInitialized);
        }
    }
}
