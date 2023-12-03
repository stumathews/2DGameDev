using System.Collections.ObjectModel;
using GameEditor.Models;

namespace GameEditor.ViewModels
{
    public class MockResourcesViewModel : ResourcesViewModel
    { 
        public MockResourcesViewModel()
        {
            var item01 = new Asset() { Uid = 1, Name = "low.wav", Type="fx", Path= "Assets/low.wav" };
            var item02 = new Asset() { Uid = 2, Name = "p3.png", Type = "graphic", Path = "Assets/Platformer/Base pack/Player/p3_walk/p3_walk.png" };
            var item03 = new Asset() { Uid = 3, Name = "arial.ttf", Type = "font", Path = "Assets/fonts/arial.ttf" };
            var item04 = new Asset() { Uid = 4, Name = "LevelMusic1", Type = "music", Path = "Assets/Music/8BitMetal.wav" };
            
            Assets = new ObservableCollection<Asset>()
            {
                item01, item03, item04, item02
            };
        }
    }
}
