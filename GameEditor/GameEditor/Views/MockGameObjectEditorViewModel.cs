using GameEditor.Models;

using System.Collections.ObjectModel;
using System.Windows.Controls;
using System.Windows.Input;

namespace GameEditor.Views
{
    public class MockGameObjectEditorViewModel : IGameObjectEditorViewModel
    {
        public ObservableCollection<GameObjectType> GameObjectTypes { get; set; }
        public ObservableCollection<AssetModel> Assets { get; set; }
        public ICommand AddGameObjectTypeCommand { get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }
        public ICommand RemoveGameObjectTypeCommand { get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }
        public string NewName { get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }
        public AssetModel NewAsset { get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }
        public string NewType { get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }
        public GameObjectType SelectedItem{ get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }
        public ICommand SelectCommand { get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }

        public MockGameObjectEditorViewModel()
        {
            var gameObjectType1 = new GameObjectType() { Name = "bluePickup", Type = "Pickup", AssetPath = "Assets/Platformer/Base pack/Player/p3_walk/p3_walk.png", Properties = { { "points", "10" } } };
            var gameObjectType2 = new GameObjectType() { Name = "redPickup", Type = "Pickup", AssetPath = "game/assets/coin_silver.png", Properties = { { "points", "20" } } };
            var asset1 = new AssetModel() { Uid = 2, Name = "p3.png", Type = "graphic", Path = "Assets/Platformer/Base pack/Player/p3_walk/p3_walk.png" };
            var asset2 = new AssetModel() { Uid = 3, Name = "arial.ttf", Type = "font", Path = "game/assets/coin_silver.png" };


            Assets = new ObservableCollection<AssetModel>()
            {
                asset1, asset2
            };

            GameObjectTypes = new ObservableCollection<GameObjectType>()
            {
                gameObjectType1, gameObjectType2
            };
        }

    }
}
