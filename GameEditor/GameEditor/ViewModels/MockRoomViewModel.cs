using GameEditor.Models;
using GameEditor.ViewModels;

namespace GameEditor.Views
{
    public class MockRoomViewModel : RoomViewModel
    {
        public MockRoomViewModel() 
        {
            TopWallVisibility = BottomWallVisibility = LeftWallVisibility = RightWallVisibility =  System.Windows.Visibility.Visible;
            ResidentGameObjectType = new GameObjectType 
            {
                Name = "dude",
                Type = "Player",
                Properties = new System.Collections.Generic.Dictionary<string, string>(),
                AssetPath = "Assets/Platformer/Base pack/Player/p1_front.png"
            };
            RoomNumber = 100;
        }
    }
}
