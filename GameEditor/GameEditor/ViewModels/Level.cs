using System.Collections.Generic;

namespace GameEditor.ViewModels
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
        public Level(int NumCols, int NumRows , List<RoomViewModel> Rooms, bool autoPopulatePickups)
        {
            this.NumCols = NumCols;
            this.NumRows = NumRows;
            this.Rooms = Rooms;
            AutoPopulatePickups = autoPopulatePickups;
        }
    }
}
