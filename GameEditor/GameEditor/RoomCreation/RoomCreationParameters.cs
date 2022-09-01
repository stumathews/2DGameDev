using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GameEditor.Data
{
    public class RoomCreationParameters : INotifyPropertyChanged
    {
        private void NotifyChanged(string name)
        {
            if(PropertyChanged != null)
            {
                PropertyChanged(name, new PropertyChangedEventArgs(name));
            }
        }

        private int wallHeight;
        private int wallWidth;
        private int numRows;
        private int numCols;

        public int WallHeight { get => wallHeight; set { wallHeight = value; NotifyChanged(nameof(WallHeight)); } }
        public int WallWidth { get => wallWidth; set { wallWidth = value; NotifyChanged(nameof(WallWidth)); } }
        public int NumRows { get => numRows; set { numRows = value;  NotifyChanged(nameof(NumRows)); } }
        public int NumCols { get => numCols; set { numCols = value; NotifyChanged(nameof(NumCols)); } }

        public event PropertyChangedEventHandler PropertyChanged;
    }
}
