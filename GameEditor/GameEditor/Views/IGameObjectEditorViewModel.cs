using GameEditor.Models;
using System.Collections.ObjectModel;
using System.Windows.Input;

namespace GameEditor.Views
{
    public interface IGameObjectEditorViewModel
    {
        ICommand AddGameObjectTypeCommand { get;set;}
        ICommand RemoveGameObjectTypeCommand { get;set;}
        ObservableCollection<AssetModel> Assets { get; }
        ObservableCollection<GameObjectType> GameObjectTypes { get; set; }
        string NewName { get; set; }
        AssetModel NewAsset { get; set; }
        string NewType { get; set; }
        GameObjectType SelectedItem { get; set; }
        ICommand SelectCommand { get; set; }
    }
}