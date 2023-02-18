using GameEditor.Utils;
using GameEditor.Windows;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Windows;
using System.Windows.Input;

namespace GameEditor.ViewModels
{
    public class MainWindowViewModel : ViewModelBase, INotifyPropertyChanged
    {
        private readonly Window window;      

        private GameObjectEditorWindow gameObjectEditorWindow;

        private NewLevelViewModel newLevelViewModel;

        private LevelManager levelManager;
        public ICommand ShowAboutCommand {get;set;}
        public ICommand CloseCommand {get;set;}
        public ICommand ShowContentManagerCommand { get;set;}
        public ICommand AddPickupCommand { get;set; }
        public ICommand RemovePickupCommand { get; set; }
        public NewLevelViewModel NewLevelViewModel { get => newLevelViewModel; }
        public LevelManager LevelManager { get => levelManager; }
        public RoomViewModel SelectedRoom
        {
            get => selectedRoom; set
            {
                selectedRoom = value;
                OnPropertyChanged(nameof(SelectedRoom));
            }
        }

        public bool AutoPopulateLevelPickups
        {
            get => autoPopulateLevelPickups; set
            {
                autoPopulateLevelPickups = value;
                OnPropertyChanged(nameof(AutoPopulateLevelPickups));
            }
        }

        private RoomViewModel selectedRoom;
        private bool autoPopulateLevelPickups;

        public bool IsRoomSelected() => SelectedRoom != null;

        public Level LoadLevelFile()
        {
            var level = levelManager.LoadLevelFile();

            newLevelViewModel.NumCols = level.NumCols;
            newLevelViewModel.NumRows = level.NumRows;
            AutoPopulateLevelPickups = level.AutoPopulatePickups;
            return level;
        }

        private void ShowAboutWindow() => new About(window).Show();

        public MainWindowViewModel(Window parent)
        {
            levelManager = new LevelManager();
            newLevelViewModel = new NewLevelViewModel();
            ShowAboutCommand = new RelayCommand((o) => ShowAboutWindow());
            CloseCommand = new RelayCommand((o) => parent.Close());
            ShowContentManagerCommand = new RelayCommand((o) => ShowContentManagerWindow());
            AddPickupCommand = new RelayCommand((o) => SelectGameObjectType());
            RemovePickupCommand = new RelayCommand((o) => 
            {                
                if (IsRoomSelected())
                {
                    SelectedRoom.ResidentGameObjectType = null;
                }
            });
            window = parent;
            AutoPopulateLevelPickups = true;
        }

        private void SelectGameObjectType()
        {
            gameObjectEditorWindow = new GameObjectEditorWindow(window);
            gameObjectEditorWindow.ShowDialog();
            if(!gameObjectEditorWindow.ViewModel.IsSelected())
            {
                MessageBox.Show("No Pickup Selected");
                return;
            }
            if(IsRoomSelected())
            { 
                SelectedRoom.ResidentGameObjectType = new Models.GameObjectType
                {
                  AssetPath = gameObjectEditorWindow.ViewModel.SelectedItem.AssetPath,
                  Name = gameObjectEditorWindow.ViewModel.SelectedItem.Name,
                  Properties = gameObjectEditorWindow.ViewModel.SelectedItem.Properties,
                  Type = gameObjectEditorWindow.ViewModel.SelectedItem.Type,
                  ResourceId = gameObjectEditorWindow.ViewModel.SelectedItem.ResourceId
                };
            }
        }

        private void ShowContentManagerWindow() => new ContentManagerWindow(window).Show();

        internal void SaveLevel(List<RoomViewModel> rooms)
        {
            
            levelManager.SaveLevelFile(new Level(newLevelViewModel.NumCols, newLevelViewModel.NumRows, rooms, AutoPopulateLevelPickups), GameObjectTypeManager.LoadGameObjectTypesActual("GameObjectTypes.xml"));
        }
    }
}
