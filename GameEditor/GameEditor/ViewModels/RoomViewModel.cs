using GameEditor.Models;
using System.ComponentModel;
using System.Windows.Media;
using GameEditor.Utils;

namespace GameEditor.ViewModels
{
    public class RoomViewModel : ViewModelBase, INotifyPropertyChanged
    {
        public RoomViewModel(int roomNumber)
        {
            this.roomNumber = roomNumber;

            WallThickness = 2;
            TopWallColour = Colors.Black;
            RightWallColour =  Colors.Black;
            BottomWallColour =  Colors.Black;
            LeftWallColour =  Colors.Black;

            BackgroundColor = Colors.White;

            PropertyChanged += (sender, args) =>
            {
                if (args.PropertyName == nameof(IsSelected))
                {
                    BackgroundColor = (IsSelected) ? Colors.LightBlue : Colors.White;
                }
            };

        }

        public Color BackgroundColor
        {
            get => backgroundColor;
            set
            {
                if (value.Equals(backgroundColor)) return;
                backgroundColor = value;
                OnPropertyChanged(nameof(BackgroundColor));
            }
        }

        [Browsable(false)]
        public bool IsSelected
        {
            get => isSelected;
            set
            {
                if (value == isSelected) return;
                isSelected = value;
                OnPropertyChanged(nameof(IsSelected));
            }
        }

        public Color LeftWallColour
        {
            get => leftWallColour;
            set
            {
                if (value.Equals(leftWallColour)) return;
                leftWallColour = value;
                OnPropertyChanged(nameof(LeftWallColour));
            }
        }

        public Color BottomWallColour
        {
            get => bottomWallColour;
            set
            {
                if (value.Equals(bottomWallColour)) return;
                bottomWallColour = value;
                OnPropertyChanged(nameof(BottomWallColour));
            }
        }

        public Color RightWallColour
        {
            get => rightWallColour;
            set
            {
                if (value.Equals(rightWallColour)) return;
                rightWallColour = value;
                OnPropertyChanged(nameof(RightWallColour));
            }
        }

        public Color TopWallColour
        {
            get => topWallColour;
            set
            {
                if (value.Equals(topWallColour)) return;
                topWallColour = value;
                OnPropertyChanged(nameof(TopWallColour));
            }
        }

        public int WallThickness    
        {
            get => wallThickness;
            set
            {
                if (value == wallThickness) return;
                wallThickness = value;
                OnPropertyChanged(nameof(WallThickness));
            }
        }

        [Browsable(false)]
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

        /// <summary>
        /// Type of Game Object that occupies this room
        /// </summary>
        [Browsable(false)]
        public GameObjectType ResidentGameObjectType
        {
            get => residentGameObject; set
            {
                residentGameObject = value;
                OnPropertyChanged(nameof(ResidentGameObjectType));
            }
        }

        private int roomNumber;
        private System.Windows.Visibility topWallVisibility;
        private System.Windows.Visibility rightWallVisibility;
        private System.Windows.Visibility bottomWallVisibility;
        private System.Windows.Visibility leftWallVisibility;
        private GameObjectType residentGameObject;
        private int wallThickness;
        private Color leftWallColour;
        private Color bottomWallColour;
        private Color rightWallColour;
        private Color topWallColour;
        private bool isSelected;
        private Color backgroundColor;
    }
}
