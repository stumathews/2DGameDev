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
        private NewLevelViewModel newLevelViewModel;
        private LevelManager levelManager;
        private UniformGrid maze;
        public MainWindow()
        {
            levelManager = new LevelManager();
            newLevelViewModel = new NewLevelViewModel();
            viewModel = new MainWindowViewModel(this);
            DataContext = viewModel;
            InitializeComponent();
            InitializeEmptyMaze();
        }

        private void InitializeEmptyMaze()
        {
            List<RoomViewModel> rooms = new List<RoomViewModel>();
            for(var i = 0; i < newLevelViewModel.NumRows * newLevelViewModel.NumRows;i++)
            {
                var room = new RoomViewModel();
                room.RoomNumber = i;
                rooms.Add(room);
            }
            SetMaze(rooms);
        }

        private void newMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var window = new CreateNewLevelWindow(newLevelViewModel);
            window.ShowDialog();
            InitializeEmptyMaze();

        }

        private void SetMaze(List<RoomViewModel> rooms)
        {
            // We can create the main content now, which is a uniform grid representing our maze of rooms

            var numRows = newLevelViewModel.NumRows;
            var numCols = newLevelViewModel.NumCols;

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
                
                maze.Children.Add(roomView);
            }

        }

        private void grid_MouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if (e.Source.GetType() == typeof(RoomView))
            {
                var roomView = (RoomView)e.Source;
                propertyGrid.SelectedObject = roomView.ViewModel;
                propertyGrid.SelectedObjectName = $"Editing Room {roomView.ViewModel.RoomNumber}";
                propertyGrid.SelectedObjectTypeName = "Room";
            }
        }

        private void openMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var level = levelManager.LoadLevelFile();
            newLevelViewModel.NumCols = level.NumCols;
            newLevelViewModel.NumRows = level.NumRows;
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
            levelManager.SaveLevelFile(new Level(newLevelViewModel.NumCols, newLevelViewModel.NumRows, rooms));
        }
    }
}
