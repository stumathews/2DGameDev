using GameEditor.Models;
using GameEditor.Utils;
using GameEditor.Views;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;

namespace GameEditor.ViewModels
{
    internal class GameObjectEditorViewModel : ViewModelBase, IGameObjectEditorViewModel, INotifyPropertyChanged
    {
        private readonly ObservableCollection<AssetModel> assets = new ObservableCollection<AssetModel>();
        private string newName;
        private string newType;
        private AssetModel newAsset;
        private GameObjectType selectedItem;

        public GameObjectEditorViewModel()
        {
            foreach (var asset in ResourceManager.ParseResources())
            {
                assets.Add(asset);
            }

            AddGameObjectTypeCommand = new RelayCommand((o) => 
            {
                GameObjectTypes.Add(new GameObjectType { AssetPath = NewAsset.Path, Name = NewName, Type = NewType });
            });

            RemoveGameObjectTypeCommand = new RelayCommand((o) => 
            {
                GameObjectTypes.Remove(SelectedItem);
            });
        }

        public ICommand AddGameObjectTypeCommand { get; set; }
        public ICommand RemoveGameObjectTypeCommand { get; set; }

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
    }
}
