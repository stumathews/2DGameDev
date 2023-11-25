using GameEditor.Models;
using System.ComponentModel;

namespace GameEditor.ViewModels
{
    public class RoomViewModel : ViewModelBase, INotifyPropertyChanged
    {
        public RoomViewModel(int roomNumber)
        {
            this.roomNumber = roomNumber;
        }
        private int roomNumber;
        private System.Windows.Visibility topWallVisibility;
        private System.Windows.Visibility rightWallVisibility;
        private System.Windows.Visibility bottomWallVisibility;
        private System.Windows.Visibility leftWallVisibility;
        private GameObjectType residentGameObject;

        public int RoomNumber
        {
            get => roomNumber; set
            {
                roomNumber = value;
                OnPropertyChanged(nameof(RoomNumber));
            }
        }
        public System.Windows.Visibility TopWallVisibility
        {
            get => topWallVisibility; set
            {
                topWallVisibility = value;
                OnPropertyChanged(nameof(TopWallVisibility));
            }
        }
        public System.Windows.Visibility RightWallVisibility
        {
            get => rightWallVisibility; set
            {
                rightWallVisibility = value;
                OnPropertyChanged(nameof(RightWallVisibility));
            }
        }
        public System.Windows.Visibility BottomWallVisibility
        {
            get => bottomWallVisibility; set
            {
                bottomWallVisibility = value;
                OnPropertyChanged(nameof(BottomWallVisibility));
            }
        }
        public System.Windows.Visibility LeftWallVisibility
        {
            get => leftWallVisibility; set
            {
                leftWallVisibility = value;
                OnPropertyChanged(nameof(LeftWallVisibility));
            }
        }

        public GameObjectType ResidentGameObjectType
        {
            get => residentGameObject; set
            {
                residentGameObject = value;
                OnPropertyChanged(nameof(ResidentGameObjectType));
            }
        }
    }
}
