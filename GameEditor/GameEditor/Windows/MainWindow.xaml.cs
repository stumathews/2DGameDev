using GameEditor.ViewModels;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Media;

namespace GameEditor.Windows
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly MainWindowViewModel viewModel;        
        private UniformGrid maze;
        public MainWindow()
        {
            viewModel = new MainWindowViewModel(this);
            DataContext = viewModel; // Hook up UI to view model
            InitializeComponent();
            InitializeEmptyMaze();
        }

        private void InitializeEmptyMaze()
        {
            var rooms = new List<RoomViewModel>();
            for(var i = 0; i < viewModel.NewLevelViewModel.NumRows * viewModel.NewLevelViewModel.NumRows;i++)
            {
                // Add new room
                rooms.Add(new RoomViewModel
                {
                    RoomNumber = i
                });
            }
            SetMaze(rooms);
        }

        private void newMenuItem_Click(object sender, RoutedEventArgs e)
        {
            // Show create new level window
            new CreateNewLevelWindow(viewModel.NewLevelViewModel).ShowDialog();
            InitializeEmptyMaze();
        }

        private void SetMaze(List<RoomViewModel> rooms)
        {
            // We can create the main content now, which is a uniform grid representing our maze of rooms
            if(Grid.Children.Contains(maze))
            {
                maze.Children.Clear();
                Grid.Children.Remove(maze);
            }

            maze = new UniformGrid
            {
                Rows = viewModel.NewLevelViewModel.NumRows,
                Columns = viewModel.NewLevelViewModel.NumCols,
                Background = Brushes.WhiteSmoke
            };

            maze.SetValue(Grid.RowProperty, 2);
            maze.SetValue(Grid.ColumnProperty, 0);
            Grid.Children.Add(maze);
                        
            foreach(var room in rooms)
            {
                var roomView = new RoomView(room.RoomNumber)
                {
                    ViewModel =
                    {
                        RightWallVisibility = room.RightWallVisibility,
                        LeftWallVisibility = room.LeftWallVisibility,
                        TopWallVisibility = room.TopWallVisibility,
                        BottomWallVisibility = room.BottomWallVisibility,
                        ResidentGameObjectType = room.ResidentGameObjectType
                    }
                };
                maze.Children.Add(roomView);
            }
        }

        private void grid_MouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (e.Source.GetType() != typeof(RoomView)) return;

            var room = (RoomView)e.Source;

            // The main window keeps track of the select room
            viewModel.SelectedRoom = room.ViewModel;

            // Let the property grid show the selected room
            PropertyGrid.SelectedObject = viewModel.SelectedRoom;
            PropertyGrid.SelectedObjectName = $"Editing Room {room.ViewModel.RoomNumber}";
            PropertyGrid.SelectedObjectTypeName = "Room";
        }

        private void openMenuItem_Click(object sender, RoutedEventArgs e)
        {
            // The view model knows how to ask for a new load window ro be shown
            var level = viewModel.LoadLevelFile();

            // With the newly loaded level, use it as the maze
            SetMaze(level.Rooms);
        }

        private void saveAsMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var rooms = new List<RoomViewModel>();

            // fetch all the rooms in the maze, i.e the roomviews that are in the grid
            foreach(var child in maze.Children)
            {
                var roomView = child as RoomView;
                rooms.Add(roomView?.ViewModel);
            }

            // Serialize the rooms
            viewModel.SaveLevel(rooms);
        }
    }
}
