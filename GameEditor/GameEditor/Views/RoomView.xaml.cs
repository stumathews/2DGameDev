using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Shapes;
using GameEditor.ViewModels;

namespace GameEditor.Views
{
    public partial class RoomView
    {
        public RoomViewModel ViewModel;

        public RoomView(int roomNumber)
        {
            ViewModel= new RoomViewModel(roomNumber);
            DataContext = ViewModel;
            InitializeComponent();
            
        }

        private void DockPanel_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (!(e.Source is Rectangle rectangle)) return;

            switch (rectangle.Name)
            {
                case "topWall": ToggleTopWallVisibility(e); break;
                case "rightWall": ToggleRightWallVisibility(e); break;
                case "bottomWall": ToggleBottomWallVisibility(e); break;
                case "leftWall": ToggleLeftWallVisibility(e); break;
            }
        }

        private static Rectangle GetRectangle(MouseEventArgs e) => (Rectangle)e.Source;

        private static void ToggleVisibility(Rectangle rectangle)
        {
            rectangle.Visibility = rectangle.Visibility == System.Windows.Visibility.Visible
                                ? System.Windows.Visibility.Hidden
                                : System.Windows.Visibility.Visible;
        }

        private void leftWall_MouseEnter(object sender, MouseEventArgs e) => ToggleLeftWallVisibility(e);

        private void ToggleLeftWallVisibility(MouseEventArgs e)
        {
            var rectangle = GetRectangle(e);
            if (e.LeftButton != MouseButtonState.Pressed) return;

            // Update the UI
            ToggleVisibility(rectangle);

            // Also update the view model
            ViewModel.LeftWallVisibility = rectangle.Visibility;
        }

        private void rightWall_MouseEnter(object sender, MouseEventArgs e) => ToggleRightWallVisibility(e);

        private void ToggleRightWallVisibility(MouseEventArgs e)
        {
            var rectangle = GetRectangle(e);

            if (e.LeftButton != MouseButtonState.Pressed)
                return;

            // Update the UI
            ToggleVisibility(rectangle);
            
            // Also update the view model
            ViewModel.RightWallVisibility = rectangle.Visibility;
        }

        private void topWall_MouseEnter(object sender, MouseEventArgs e) => ToggleTopWallVisibility(e);

        private void ToggleTopWallVisibility(MouseEventArgs e)
        {
            var rectangle = GetRectangle(e);
            if (e.LeftButton != MouseButtonState.Pressed)
                return;

            // Update the UI
            ToggleVisibility(rectangle);

            // Also update the view model
            ViewModel.TopWallVisibility = rectangle.Visibility;
        }

        private void bottomWall_MouseEnter(object sender, MouseEventArgs e) => ToggleBottomWallVisibility(e);

        private void ToggleBottomWallVisibility(MouseEventArgs e)
        {
            var rectangle = GetRectangle(e);
            if (e.LeftButton != MouseButtonState.Pressed)
                return;

            // Update the UI
            ToggleVisibility(rectangle);

            // Also update the view model
            ViewModel.BottomWallVisibility = rectangle.Visibility;
        }
    }
}
