using GameEditor.Utils;
using GameEditor.Windows;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Input;

namespace GameEditor.ViewModels
{
    public class MainWindowViewModel : ViewModelBase, INotifyPropertyChanged
    {
        public ICommand ShowAboutCommand {get;set;}
        public ICommand CloseCommand {get;set;}
        public ICommand ShowContentManagerCommand { get;set;}
        public ICommand AddPickupCommand { get;set; }
        public ICommand RemovePickupCommand { get; set; }
        public ICommand LoadLevelFileCommand { get; set; }
        public ICommand SaveLevelCommand { get; set;}
        public ICommand CreateNewLevelCommand { get; set; }

        public NewLevelViewModel NewLevelViewModel { get; }
        public LevelManager LevelManager { get; }

        public bool AutoPopulatePickups
        {
            get => autoPopulatePickups;
            set
            {
                if (value == autoPopulatePickups) return;
                autoPopulatePickups = value;
                OnPropertyChanged(nameof(AutoPopulatePickups));
            }
        }

        public Level Level
        {
            get => level;
            set
            {
                level = value;
                OnPropertyChanged(nameof(Level));
            }
        }

        public RoomViewModel SelectedRoom
        {
            get => selectedRoom; set
            {
                selectedRoom = value;
                OnPropertyChanged(nameof(SelectedRoom));
            }
        }

        public void CreateEmptyLevel()
        {
            const int rows = 10;
            const int cols = 10;

            // Create room view models
            var rooms = CreateEmptyRooms(rows, cols);

            // Create level from rooms
            Level = new Level(rows, cols, rooms, AutoPopulatePickups);
        }

        public MainWindowViewModel(Window parent)
        {
            // We keep track of the parent window
            window = parent;

            // Our level manager
            LevelManager = new LevelManager();

            // New level view model
            NewLevelViewModel = new NewLevelViewModel();

            // The ViewModel's behaviors that the View can bind to
            ShowAboutCommand = new RelayCommand((o) => new About(window).Show());
            CloseCommand = new RelayCommand((o) => parent.Close());
            ShowContentManagerCommand = new RelayCommand((o) => new ContentManagerWindow(window).Show());
            AddPickupCommand = new RelayCommand((o) => AssociateSelectedGameObjectWithRoom());
            RemovePickupCommand = new RelayCommand((o) => RemovePickupFromSelectedRoom(), canExecute: o => IsRoomSelected());
            LoadLevelFileCommand = new RelayCommand(LoadLevelFile);
            SaveLevelCommand = new RelayCommand((rooms => SaveLevel(rooms as List<RoomViewModel>)));
            CreateNewLevelCommand = new RelayCommand(o => CreateNewLevel());
        }

        private void RemovePickupFromSelectedRoom()
        {
            SelectedRoom.ResidentGameObjectType = null;
        }

        private void LoadLevelFile(object o)
        {
            Level = LevelManager.LoadLevelFile();
            if (Level == null) return;

            // Ensure we extract the value so we can bind it in the UI
            AutoPopulatePickups = Level.AutoPopulatePickups;
        }

        private void CreateNewLevel()
        {
            // Show create new level window
            new CreateNewLevelWindow(NewLevelViewModel).ShowDialog();

            var rows = NewLevelViewModel.NumRows;
            var cols = NewLevelViewModel.NumCols;

            var rooms = CreateEmptyRooms(rows, cols);

            // Create room
            Level = new Level(rows, cols, rooms, AutoPopulatePickups);
        }

        private static List<RoomViewModel> CreateEmptyRooms(int numRow, int numCols)
        {
            // Create a list of Rooms represented by a RoomViewModel object
            var rooms = new List<RoomViewModel>();

            // Lets make as many as the view model says we can depending on the number of Rows x Cols
            for(var i = 0; i < numRow * numCols; i++)
            {
                // Add new room
                rooms.Add(new RoomViewModel(i));
            }

            return rooms;
        }
        
        // Used when you want to add a Pickup to the room
        private void AssociateSelectedGameObjectWithRoom()
        {
            // Select a Game Object Type eg Pickup, Enemy, Player etc..
            gameObjectEditorWindow = new GameObjectEditorWindow(window);
            gameObjectEditorWindow.ShowDialog();

            // What did the user select...
            if(!gameObjectEditorWindow.ViewModel.IsSelected())
            {
                MessageBox.Show("No game object selected");
                return;
            }

            if(IsRoomSelected())
            { 
                // Associate the GameObject Type with the room its been selected for
                SelectedRoom.ResidentGameObjectType = new Models.GameObjectType
                { 
                    // Where the path of this asset is
                    AssetPath = gameObjectEditorWindow.ViewModel.SelectedItem.AssetPath,
                    
                    // Name of the asset for this game object
                    Name = gameObjectEditorWindow.ViewModel.SelectedItem.Name,
                    
                    // Properties for this game object
                    Properties = gameObjectEditorWindow.ViewModel.SelectedItem.Properties,
                    
                    // The type of game object, eg. Enemy, Player, Pickup etc..
                    Type = gameObjectEditorWindow.ViewModel.SelectedItem.Type,

                    // The resource Id from the resources.xml
                    ResourceId = gameObjectEditorWindow.ViewModel.SelectedItem.ResourceId
                };
            }
        }

        private bool IsRoomSelected() => SelectedRoom != null;

        private void SaveLevel(List<RoomViewModel> rooms)
        {
            // Out level is defined as rooms in a Row x Col configuration.
            // It also can be set to have its pickups loaded randomly by the game or not.
            var levelToSave = new Level(NewLevelViewModel.NumCols, NewLevelViewModel.NumRows, rooms, AutoPopulatePickups);

            // Game Object Types eg. Pickup, Player, Enemy
            var knownGameObjectTypes = GameObjectTypeManager.GetGameObjectTypes("GameObjectTypes.xml");

            // Save the level to XML
            LevelManager.SaveLevelFile(levelToSave, knownGameObjectTypes);
        }

        private RoomViewModel selectedRoom;
        private Level level;
        private readonly Window window;
        private GameObjectEditorWindow gameObjectEditorWindow;
        private bool autoPopulatePickups;
    }
}