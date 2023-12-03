using System.Collections.Generic;
using GameEditor.Models;

namespace GameEditor.ViewModels
{
    public class MockSpriteAssetViewModel : SpriteAssetViewModel
    {
        private static readonly ColorKey TestColorKey = new ColorKey(255, 0, 0);
        private static Asset asset = new Asset() { Uid = 2, Name = "p3.png", Type = "graphic", Path = "Assets/Platformer/Base pack/Player/p3_walk/p3_walk.png" };

        public MockSpriteAssetViewModel()
        {
            Asset = new SpriteAsset(asset.Uid, asset.Name, asset.Path, 32,32,TestColorKey, KeyFrameCollection.Test);
        }
    }
}