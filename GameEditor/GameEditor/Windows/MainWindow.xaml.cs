using GameEditor.ViewModels;
using System.Linq;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
using System.Windows.Media;
using GameEditor.Views;

namespace GameEditor.Windows
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        private readonly MainWindowViewModel mainWindowViewModel;        
        
        public UniformGrid MazeGrid;
        
        public MainWindow()
        {
            // Construct the Objects defined in the XAML
            InitializeComponent();
            
            // View model to drive behavior and store data for view
            mainWindowViewModel = new MainWindowViewModel(this);

            // Set the data context for XAML elements to the view model
            DataContext = mainWindowViewModel; // Hook up UI to view model
            
            // Let us know when the Level is changed. i.e new level or loaded an existing level etc.
            mainWindowViewModel.PropertyChanged += (sender, args) =>
            {
                if (args.PropertyName.Equals(nameof(mainWindowViewModel.Level)))
                {
                    // Use the changes in the level to update the UI
                    UpdateMazeGrid(mainWindowViewModel);
                }
            };
            
            // Start with empty level
            mainWindowViewModel.CreateEmptyLevel();
        }
        
        public void UpdateMazeGrid(MainWindowViewModel windowViewModel)
        {
            if (windowViewModel.Level == null) return;

            // We can create the main content now, which is a uniform grid representing our maze of rooms...
           
            // Clear out the previous level
            if(Grid.Children.Contains(MazeGrid))
            {
                MazeGrid.Children.Clear();
                Grid.Children.Remove(MazeGrid);
            }

            // Make an empty new level
            MazeGrid = new UniformGrid
            {
                Rows = 10,
                Columns = 10,
                Background = Brushes.WhiteSmoke
            };
            
            // Set attached properties that we'd normally set in XAML
            MazeGrid.SetValue(Grid.RowProperty, 2);
            MazeGrid.SetValue(Grid.ColumnProperty, 0);

            // Add maze to grid manually
            Grid.Children.Add(MazeGrid);
            
            // Construct the level UI elements from the provided rooms view models

            var roomViews = windowViewModel.Level.Rooms.Select((room, i) => new RoomView(i)
            {
                ViewModel =
                {
                    RightWallVisibility = room.RightWallVisibility,
                    LeftWallVisibility = room.LeftWallVisibility,
                    TopWallVisibility = room.TopWallVisibility,
                    BottomWallVisibility = room.BottomWallVisibility,
                    ResidentGameObjectType = room.ResidentGameObjectType
                }
            });

            // Populate/add to UI
            foreach (var roomView in roomViews)
            {
                if (roomView.ViewModel.RoomNumber == 0)
                {
                    SetSelectedRoom(roomView.ViewModel);
                }

                MazeGrid.Children.Add(roomView);
            }
            
            ViewSelectedRoomProperties();
        }

        private void grid_MouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (e.Source.GetType() != typeof(RoomView)) return;

            // You clicked on a Room...
            var room = (RoomView)e.Source;

            SetSelectedRoom(room.ViewModel);


            // Let the property grid show the selected room
            ViewSelectedRoomProperties();
        }

        private void SetSelectedRoom(RoomViewModel room)
        {
            if (mainWindowViewModel.SelectedRoom != null)
            {
                mainWindowViewModel.SelectedRoom.IsSelected = false;
            }

            // The main window keeps track of the select room
            mainWindowViewModel.SelectedRoom = room;
            mainWindowViewModel.SelectedRoom.IsSelected = true;
        }

        private void ViewSelectedRoomProperties()
        {
            PropertyGrid.SelectedObject = mainWindowViewModel.SelectedRoom;
            PropertyGrid.SelectedObjectName = $"Editing Room {mainWindowViewModel.SelectedRoom.RoomNumber}";
            PropertyGrid.SelectedObjectTypeName = "Room";
        }

        //  Save current level configuration to file
        private void saveAsMenuItem_Click(object sender, RoutedEventArgs e)
        {
            // Collect view models from the UI and save them as a new level
            var rooms = MazeGrid.Children.OfType<RoomView>().Select(x => x.ViewModel).ToList();
            mainWindowViewModel.SaveLevelCommand.Execute(rooms);
        }
    }
}
