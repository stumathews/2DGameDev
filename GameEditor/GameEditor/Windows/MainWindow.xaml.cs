using GameEditor.ViewModels;
using Microsoft.Win32;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Controls.Primitives;
using System.Windows.Media;
using System.Windows.Shapes;
using System.Xml;

namespace GameEditor.Windows
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private readonly NewLevelViewModel newLevelViewModel;
        private UniformGrid maze;
        public MainWindow()
        {
            InitializeComponent();
            newLevelViewModel = new NewLevelViewModel();
            AddMazeToWindow();
        }

        private void aboutMenuItem_Click(object sender, RoutedEventArgs e)
        {
            About aboutMenu = new About
            {
                Owner = this // Allows to set startup position according to owner
            };

            aboutMenu.Show();
            status.Text = "Showing about";
        }

        private void closeMenuItem_Click(object sender, RoutedEventArgs e)
        {
            Close();
        }

        private void newMenuItem_Click(object sender, RoutedEventArgs e)
        {
            Windows.CreateNewLevelWindow createNewLevelWindow = new Windows.CreateNewLevelWindow(newLevelViewModel)
            {
                Owner = this
            };

            createNewLevelWindow.ShowDialog();
            AddMazeToWindow();

        }

        private void AddMazeToWindow()
        {
            // We can create the main content now, which is a uniform grid representing our rooms
            maze = new UniformGrid
            {
                Rows = newLevelViewModel.NumRows,
                Columns = newLevelViewModel.NumCols,
                Background = Brushes.WhiteSmoke
            };

            // Add it to the correct place in the main window's grid
            maze.SetValue(Grid.RowProperty, 2);
            maze.SetValue(Grid.ColumnProperty, 0);
            grid.Children.Add(maze);

            // Walls to the cells
            for (var i = 0; i < newLevelViewModel.NumRows * newLevelViewModel.NumCols; i++)
            {
                //var panel = new DockPanel();
                //panel.LastChildFill = true;

                //var top = new Rectangle();
                //top.SetValue(DockPanel.DockProperty, Dock.Top);
                //var right = new Rectangle();
                //right.SetValue(DockPanel.DockProperty, Dock.Right);
                //var bottom = new Rectangle();
                //bottom.SetValue(DockPanel.DockProperty, Dock.Bottom);
                //var left = new Rectangle();
                //left.SetValue(DockPanel.DockProperty, Dock.Left);

                //top.Height = bottom.Height = 2;
                //right.Width = left.Width = 2;
                //top.Fill = right.Fill = bottom.Fill = left.Fill = Brushes.Black;

                //panel.Children.Add(left);
                //panel.Children.Add(right);
                //panel.Children.Add(top);
                //panel.Children.Add(bottom);
                //TextBlock label = new TextBlock();
                //label.Text = $"{i}";
                //panel.Children.Add(label);
                maze.Children.Add(new RoomView(i));
            }
        }

        private void grid_MouseUp(object sender, System.Windows.Input.MouseButtonEventArgs e)
        {
            if(e.Source.GetType() == typeof(RoomView))
            {
                var roomView = (RoomView)e.Source;
                propertyGrid.SelectedObject = roomView.ViewModel;
                propertyGrid.SelectedObjectName = $"Editing Room {roomView.ViewModel.RoomNumber}";
                propertyGrid.SelectedObjectTypeName = "Room";
            }
        }

        private void saveAsMenuItem_Click(object sender, RoutedEventArgs e)
        {
            // Collect all the rooms and serialize to XML
            var sts = new XmlWriterSettings()
            {
                Indent = true,
            };

            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "XML Files (*.xml)|*.xml;";
            try
            {
                if(saveFileDialog.ShowDialog() is true)
                {
                    status.Text = $"Saving File '{saveFileDialog.FileName}'...";
                    using (XmlWriter writer = XmlWriter.Create(saveFileDialog.FileName, sts))
                    {
                        writer.WriteStartDocument();
                        writer.WriteStartElement("level");
                        writer.WriteAttributeString("cols", newLevelViewModel.NumCols.ToString());
                        writer.WriteAttributeString("rows", newLevelViewModel.NumRows.ToString());
                        foreach(var child in maze.Children)
                        {
                            var roomView = (RoomView)child;
                            var roomViewModel = roomView.ViewModel;

                            var topVisible = roomViewModel.TopWallVisibility == Visibility.Visible;
                            var rightVisible = roomViewModel.RightWallVisibility == Visibility.Visible;
                            var bottomVisible = roomViewModel.BottomWallVisibility == Visibility.Visible;
                            var leftVisible = roomViewModel.LeftWallVisibility == Visibility.Visible;

                            writer.WriteStartElement("room");
                            writer.WriteAttributeString("number", roomViewModel.RoomNumber.ToString());
                            writer.WriteAttributeString("top", topVisible.ToString() );
                            writer.WriteAttributeString("right", rightVisible.ToString() );
                            writer.WriteAttributeString("bottom", bottomVisible.ToString() );
                            writer.WriteAttributeString("left", leftVisible.ToString() );
                            writer.WriteEndElement();
                        }
                        writer.WriteEndElement();
                        writer.WriteEndDocument();
                    }
                    status.Text = $"Saved File '{saveFileDialog.FileName}'.";
                } 
            }
            catch(Exception ex)
            {
                status.Text = $"Error saving level file '{saveFileDialog.FileName}': {ex.Message}";
            }            
            
        }
    }
}
