using GameEditor.Views;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Collections.ObjectModel;

namespace GameEditor.Views
{
    public class MockAssetView
    { 
        public ObservableCollection<AssetModel> Assets { get; set; }

        public MockAssetView()
        {
            var item01 = new AssetModel() { Uid = 1, Name = "low.wav", Type="fx", Path= "Assets/low.wav" };
            var item02 = new AssetModel() { Uid = 2, Name = "p3.png", Type = "graphic", Path = "Assets/Platformer/Base pack/Player/p3_walk/p3_walk.png" };
            var item03 = new AssetModel() { Uid = 3, Name = "arial.ttf", Type = "font", Path = "Assets/fonts/arial.ttf" };
            var item04 = new AssetModel() { Uid = 4, Name = "LevelMusic1", Type = "music", Path = "Assets/Music/8BitMetal.wav" };
            Assets = new ObservableCollection<AssetModel>()
            {
                item01, item03, item04, item02
            };
        }
    }
}
