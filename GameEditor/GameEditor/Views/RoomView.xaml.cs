using GameEditor.ViewModels;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Shapes;

namespace GameEditor
{
    /// <summary>
    /// Interaction logic for RoomView.xaml
    /// </summary>
    public partial class RoomView : UserControl
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
            if (e.Source.GetType() == typeof(Rectangle))
            {
                var rectangle = (Rectangle)e.Source;

                if (rectangle.Name.Equals("topWall"))
                {
                    ToggleTopWallVisibility(e);
                }
                if (rectangle.Name.Equals("rightWall"))
                {
                    ToggleRightWallVisibility(e);
                }
                if (rectangle.Name.Equals("bottomWall"))
                {
                    ToggleBottomWallVisibility(e);
                }
                if (rectangle.Name.Equals("leftWall"))
                {
                    ToggleLeftWallVisibility(e);
                }

            }
        }

        
        private static Rectangle GetRectangle(MouseEventArgs e)
        {
            var rectangle = (Rectangle)e.Source;
            return rectangle;
        }

        private static void ToggleVisibility(Rectangle rectangle)
        {
            rectangle.Visibility = rectangle.Visibility == System.Windows.Visibility.Visible
                                ? System.Windows.Visibility.Hidden
                                : System.Windows.Visibility.Visible;
        }

        private void leftWall_MouseEnter(object sender, MouseEventArgs e)
        {
            ToggleLeftWallVisibility(e);
        }

        private void ToggleLeftWallVisibility(MouseEventArgs e)
        {
            Rectangle rectangle = GetRectangle(e);
            if (e.LeftButton != MouseButtonState.Pressed)
                return;
            ToggleVisibility(rectangle);
            ViewModel.LeftWallVisibility = rectangle.Visibility;
        }

        private void rightWall_MouseEnter(object sender, MouseEventArgs e)
        {
            ToggleRightWallVisibility(e);
        }

        private void ToggleRightWallVisibility(MouseEventArgs e)
        {
            Rectangle rectangle = GetRectangle(e);

            if (e.LeftButton != MouseButtonState.Pressed)
                return;

            ToggleVisibility(rectangle);
            ViewModel.RightWallVisibility = rectangle.Visibility;
        }

        private void topWall_MouseEnter(object sender, MouseEventArgs e)
        {
            ToggleTopWallVisibility(e);
        }

        private void ToggleTopWallVisibility(MouseEventArgs e)
        {
            Rectangle rectangle = GetRectangle(e);
            if (e.LeftButton != MouseButtonState.Pressed)
                return;
            ToggleVisibility(rectangle);
            ViewModel.TopWallVisibility = rectangle.Visibility;
        }

        private void bottomWall_MouseEnter(object sender, MouseEventArgs e)
        {
            ToggleBottomWallVisibility(e);
        }

        private void ToggleBottomWallVisibility(MouseEventArgs e)
        {
            Rectangle rectangle = GetRectangle(e);
            if (e.LeftButton != MouseButtonState.Pressed)
                return;
            ToggleVisibility(rectangle);
            ViewModel.BottomWallVisibility = rectangle.Visibility;
        }
    }
}
