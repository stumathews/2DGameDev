﻿using GameEditor.Models;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Windows.Input;

namespace GameEditor.Views
{
    public class MockGameObjectEditorViewModel : IGameObjectEditorViewModel
    {
        public ObservableCollection<GameObjectType> GameObjectTypes { get; set; }
        public ObservableCollection<Asset> Assets { get; set; }
        public ICommand AddGameObjectTypeCommand { get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }
        public ICommand RemoveGameObjectTypeCommand { get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }
        public string NewName { get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }
        public Asset NewAsset { get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }
        public string NewType { get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }
        public GameObjectType SelectedItem{ get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }
        public ICommand SelectCommand { get => throw new System.NotImplementedException(); set => throw new System.NotImplementedException(); }

        public MockGameObjectEditorViewModel()
        {
            var gameObjectType1 = new GameObjectType() { Name = "bluePickup", Type = "Pickup", AssetPath = "Assets/Platformer/Base pack/Player/p3_walk/p3_walk.png", Properties = new List<KeyValuePair<string, string>> { new KeyValuePair<string, string>("points", "10") } };
            var gameObjectType2 = new GameObjectType() { Name = "redPickup", Type = "Pickup", AssetPath = "game/assets/coin_silver.png", Properties = new List<KeyValuePair<string, string>> { new KeyValuePair<string, string>("points","20") } };
            var asset1 = new Asset() { Uid = 2, Name = "p3.png", Type = "graphic", Path = "Assets/Platformer/Base pack/Player/p3_walk/p3_walk.png" };
            var asset2 = new Asset() { Uid = 3, Name = "arial.ttf", Type = "font", Path = "game/assets/coin_silver.png" };


            Assets = new ObservableCollection<Asset>()
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
