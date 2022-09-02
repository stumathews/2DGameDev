using GameEditor.Windows;
using System.Collections.Generic;
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

        private void ShowAboutWindow()
        {
            About aboutMenu = new About
            {
                Owner = window // Allows to set startup position according to owner
            };

            aboutMenu.Show();
        }

        public MainWindowViewModel(Window parent)
        {           
            ShowAboutCommand = new RelayCommand((o) => ShowAboutWindow());
            CloseCommand = new RelayCommand((o) => parent.Close());             
            window = parent;
        }
    }
}
