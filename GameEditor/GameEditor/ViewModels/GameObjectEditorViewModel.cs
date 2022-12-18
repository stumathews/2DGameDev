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
        private readonly ObservableCollection<AssetModel> assets = new ObservableCollection<AssetModel>();
        private string newName;
        private string newType;
        private AssetModel newAsset;
        private GameObjectType selectedItem;

        public GameObjectEditorViewModel(Window window)
        {
            Window = window;

            window.Closing += Window_Closing;

            foreach (var asset in ResourceManager.ParseResources())
            {
                assets.Add(asset);
            }

            LoadGameObjectTypes();

            AddGameObjectTypeCommand = new RelayCommand((o) => 
            {
                GameObjectTypes.Add(new GameObjectType { AssetPath = NewAsset.Path, Name = NewName, Type = NewType, ResourceId = NewAsset.Uid });
            });

            RemoveGameObjectTypeCommand = new RelayCommand((o) => 
            {
                GameObjectTypes.Remove(SelectedItem);
            });

            SelectCommand = new RelayCommand(o => 
            {                
                if (SelectCommand == null)
                {
                    MessageBox.Show("No Gametype selected");
                    return;
                }
                
                Window.Close();
            });
        }

        private void Window_Closing(object sender, CancelEventArgs e)
        {
            SaveGameObjectTypes();
        }

        public ICommand AddGameObjectTypeCommand { get; set; }
        public ICommand RemoveGameObjectTypeCommand { get; set; }

        public ICommand SelectCommand { get;set;}

        public bool IsSelected() => selectedItem != null;

        private void LoadGameObjectTypes()
        {
            foreach (var type in GameObjectTypeManager.LoadGameObjectTypesActual("GameObjectTypes.xml"))
            {
                GameObjectTypes.Add(type);
            }
        }

        internal void SaveGameObjectTypes()
        {
            GameObjectTypeManager.SaveGameObjectTypes(GameObjectTypes.ToList());
        }

        public string NewName { get => newName; set { newName = value; OnPropertyChanged(nameof(NewName)); } }
        public string NewType { get => newType; set { newType = value; OnPropertyChanged(nameof(NewType)); } }
        public AssetModel NewAsset { get => newAsset; set { newAsset = value; OnPropertyChanged(nameof(NewAsset)); } }
        public ObservableCollection<AssetModel> Assets => assets;

        public ObservableCollection<GameObjectType> GameObjectTypes { get; set; } = new ObservableCollection<GameObjectType>();
        public GameObjectType SelectedItem
        {
            get => selectedItem; set
            {
                selectedItem = value;
                OnPropertyChanged(nameof(SelectedItem));
            }
        }

        public Window Window { get; }
    }
}
