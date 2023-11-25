using GameEditor.Models;
using GameEditor.Utils;
using GameEditor.Views;
using System;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Windows;
using System.Windows.Input;

namespace GameEditor.ViewModels
{
    public class GameObjectEditorViewModel : ViewModelBase, IGameObjectEditorViewModel, INotifyPropertyChanged
    {
        public ICommand AddGameObjectTypeCommand { get; set; }
        public ICommand RemoveGameObjectTypeCommand { get; set; }
        public ICommand SelectCommand { get;set;}
        public bool IsInitialized { get; private set; }
        public string NewName { get => newName; set { newName = value; OnPropertyChanged(nameof(NewName)); } }
        public string NewType { get => newType; set { newType = value; OnPropertyChanged(nameof(NewType)); } }
        public GameObjectType SelectedItem { get => selectedItem; set { selectedItem = value; OnPropertyChanged(nameof(SelectedItem)); } }
        public AssetModel NewAsset { get => newAsset; set { newAsset = value; OnPropertyChanged(nameof(NewAsset)); } }
        public ObservableCollection<GameObjectType> GameObjectTypes { get; set; } = new ObservableCollection<GameObjectType>();
        public ObservableCollection<AssetModel> Assets { get; } = new ObservableCollection<AssetModel>();
        public bool IsSelected() => selectedItem != null;
        public void SaveGameObjectTypes() => GameObjectTypeManager.SaveGameObjectTypes(GameObjectTypes.ToList());

        public GameObjectEditorViewModel(Window window)
        {
            Window = window;

            window.Closing += (sender, args) => SaveGameObjectTypes();
            
            AddGameObjectTypeCommand = new RelayCommand((o) => { AddGameObjectType(); }, (o) => CanExecuteAddGameObject());
            RemoveGameObjectTypeCommand = new RelayCommand((o) => { GameObjectTypes.Remove(SelectedItem); }, (o)=> IsSelected());
            SelectCommand = new RelayCommand(o => { OnSubmit(); }, (o) => IsSelected());
            NewName = NewType = null;
            NewAsset = null;
        }

        public bool Initialize()
        {
            try
            {
                // Load resources
                foreach (var asset in ResourceManager.ParseResources())
                {
                    Assets.Add(asset);
                }

                // Load game object types
                foreach (var type in GameObjectTypeManager.GetGameObjectTypes("GameObjectTypes.xml"))
                {
                    GameObjectTypes.Add(type);
                }
                
                SelectedItem = GameObjectTypes.First();
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Unable to crete Game Object Editor View Model: {ex.Message}");
                IsInitialized = false;
            }


            IsInitialized = true;
            return IsInitialized;
        }


        private bool CanExecuteAddGameObject()
        {
            return !string.IsNullOrEmpty(NewType) && !string.IsNullOrEmpty(NewName) && newAsset != null;
        }

        private void OnSubmit()
        {
            if (SelectCommand == null)
            {
                MessageBox.Show("No Game type selected");
                return;
            }

            Window.Close();
        }

        private void AddGameObjectType()
        {
            if (!CanExecuteAddGameObject()) return;

            GameObjectTypes.Add(new GameObjectType
            {
                // Protected by CanExecute command
                AssetPath = NewAsset.Path,
                Name = NewName,
                Type = NewType,
                ResourceId = NewAsset.Uid
            });
        }

        
        private string newName;
        private string newType;
        private AssetModel newAsset;
        private GameObjectType selectedItem;
        private Window Window { get; }
    }
}
