using GameEditor.Windows;
using System.ComponentModel;
using System.Windows;
using System.Windows.Input;

namespace GameEditor.ViewModels
{
    public class MainWindowViewModel : ViewModelBase, INotifyPropertyChanged
    {
        private readonly Window window;      
        public ICommand ShowAboutCommand {get;set;}
        public ICommand CloseCommand {get;set;}
        public ICommand ShowContentManagerCommand { get;set;}
        public ICommand AddPickupCommand { get;set; }

        private void ShowAboutWindow() => new About(window).Show();

        public MainWindowViewModel(Window parent)
        {           
            ShowAboutCommand = new RelayCommand((o) => ShowAboutWindow());
            CloseCommand = new RelayCommand((o) => parent.Close());     
            ShowContentManagerCommand = new RelayCommand((o) => ShowContentManagerWindow() );
            AddPickupCommand = new RelayCommand((o) => new GameObjectEditorWindow(window).Show());
            window = parent;
        }

        private void ShowContentManagerWindow() => new ContentManagerWindow(window).Show();
    }
}
