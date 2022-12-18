using GameEditor.ViewModels;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Data;
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
            DataContext = viewModel;
            InitializeComponent();
            InitializeEmptyMaze();

            // Bind property grids selected item with viewMmodels selectedItem
            //propertyGrid.SelectedObject
            //viewModel.SelectedRoom

        }

        private void InitializeEmptyMaze()
        {
            var rooms = new List<RoomViewModel>();
            for(var i = 0; i < viewModel.NewLevelViewModel.NumRows * viewModel.NewLevelViewModel.NumRows;i++)
            {
                var room = new RoomViewModel
                {
                    RoomNumber = i
                };
                rooms.Add(room);
            }
            SetMaze(rooms);
        }

        private void newMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var window = new CreateNewLevelWindow(viewModel.NewLevelViewModel);
            window.ShowDialog();
            InitializeEmptyMaze();
        }

        private void SetMaze(List<RoomViewModel> rooms)
        {
            // We can create the main content now, which is a uniform grid representing our maze of rooms

            var numRows = viewModel.NewLevelViewModel.NumRows;
            var numCols = viewModel.NewLevelViewModel.NumCols;

            if(grid.Children.Contains(maze))
            {
                maze.Children.Clear();
                grid.Children.Remove(maze);
            }

            maze = new UniformGrid
            {
                Rows = numRows,
                Columns = numCols,
                Background = Brushes.WhiteSmoke
            };

            maze.SetValue(Grid.RowProperty, 2);
            maze.SetValue(Grid.ColumnProperty, 0);
            grid.Children.Add(maze);
                        
            foreach(var room in rooms)
            {
                var roomView = new RoomView(room.RoomNumber);
                roomView.ViewModel.RightWallVisibility = room.RightWallVisibility;
                roomView.ViewModel.LeftWallVisibility = room.LeftWallVisibility;
                roomView.ViewModel.TopWallVisibility = room.TopWallVisibility;
                roomView.ViewModel.BottomWallVisibility = room.BottomWallVisibility;
                roomView.ViewModel.ResidentGameObjectType = room.ResidentGameObjectType;
                maze.Children.Add(roomView);
            }
        }

        private void grid_MouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (e.Source.GetType() == typeof(RoomView))
            {
                var roomView = (RoomView)e.Source;
                viewModel.SelectedRoom = roomView.ViewModel;

                propertyGrid.SelectedObject = viewModel.SelectedRoom;
                propertyGrid.SelectedObjectName = $"Editing Room {roomView.ViewModel.RoomNumber}";
                propertyGrid.SelectedObjectTypeName = "Room";
                
            }
        }

        private void openMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var level = viewModel.LoadLevelFile();
            SetMaze(level.Rooms);
        }

        private void saveAsMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var rooms = new List<RoomViewModel>();
            foreach(var child in maze.Children)
            {
                var roomView = child as RoomView;
                rooms.Add(roomView.ViewModel);
            }

            // Save these rooms;
            viewModel.SaveLevel(rooms);
        }
    }
}
