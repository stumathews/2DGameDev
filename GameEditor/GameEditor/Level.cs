using System.Collections.Generic;
using GameEditor.ViewModels;

namespace GameEditor
{
    public class Level
    {
        public int NumCols { get; set; }
        public int NumRows { get; set; }
        public List<RoomViewModel> Rooms { get; set; }
        public bool AutoPopulatePickups { get; set; }

        public Level()
        {
            Rooms = new List<RoomViewModel>();
        }

        public Level(int numCols, int numRows , List<RoomViewModel> rooms, bool autoPopulatePickups)
        {
            NumCols = numCols;
            NumRows = numRows;
            Rooms = rooms;
            AutoPopulatePickups = autoPopulatePickups;
        }
    }
}
