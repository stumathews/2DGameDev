
using System.ComponentModel;
using System.Windows;
using System.Windows.Input;

namespace GameEditor.ViewModels
{
    public class NewLevelViewModel : ViewModelBase, INotifyPropertyChanged
    {
        private int numRows;
        public int NumRows
        {
            get => numRows;
            set { numRows = value; OnPropertyChanged(nameof(NumRows)); }
        }

        private int numCols;

        public int NumCols
        {
            get => numCols;
            set { numCols = value; OnPropertyChanged(nameof(NumCols)); }
        }

        private int wallHeight;

        public int WallHeight
        {
            get => wallHeight;
            set { wallHeight = value; OnPropertyChanged(nameof(WallHeight)); }
        }

        private int wallWidth;
        public Window Parent { get; set; }

        public int WallWidth
        {
            get => wallWidth;
            set { wallWidth = value; OnPropertyChanged(nameof(WallWidth));}
        }

        private void Submit()
        {
            Parent.Close();
        }

        bool CanSubmit()
        {            
            return NumRows == NumCols && WallHeight > 0 && WallWidth > 0;
        }

        public ICommand SubmitCommand
        {
            get; private set;
        }

        public ICommand CancelCommand
        {
            get; private set;
        }

        public NewLevelViewModel()
        {
            NumRows = NumCols = 10;
            WallHeight = WallWidth = 10;
            SubmitCommand = new RelayCommand((o) => Submit(), (o) => CanSubmit());
            CancelCommand = new RelayCommand((o) => Cancel());
        }

        private void Cancel()
        {
            MessageBox.Show("Roger that, Cancelling.");
            Parent?.Close();
        }
    }
}
