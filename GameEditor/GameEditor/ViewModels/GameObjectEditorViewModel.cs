using GameEditor.Models;
using GameEditor.Utils;
using GameEditor.Views;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Reflection.Emit;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Input;
using System.Xml;

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
            var filename = "GameObjectTypes.xml";
            if (!File.Exists(filename))
            {
                return;
            }
            XmlReaderSettings settings = new XmlReaderSettings();
            settings.DtdProcessing = DtdProcessing.Ignore;
            XmlReader reader = XmlReader.Create(filename, settings);
            GameObjectType gameObjectType = null;
            List<GameObjectType> gameObjectTypes = new List<GameObjectType>();
            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    if (reader.Name.Equals("GameObjectType"))
                    {
                        gameObjectType = new GameObjectType();
                        gameObjectType.Properties = new Dictionary<string, string>();
                        gameObjectType.AssetPath = reader.GetAttribute("AssetPath");
                        gameObjectType.Name = reader.GetAttribute("Name");
                        gameObjectType.ResourceId = int.Parse(reader.GetAttribute("ResourceId"));
                        gameObjectType.Type = reader.GetAttribute("Type");                       
                    }                    
                }
                if (reader.Name.Equals("Property"))
                {
                    for (var attIndex = 0; attIndex < reader.AttributeCount; attIndex++)
                    {
                        reader.MoveToAttribute(attIndex);
                        gameObjectType.Properties.Add(reader.Name, reader.Value);
                    }
                }
                if(reader.NodeType == XmlNodeType.EndElement && reader.Name.Equals("GameObjectType"))
                {
                    gameObjectTypes.Add(gameObjectType);
                    gameObjectType = null;
                }
            }
            foreach(var type in gameObjectTypes)
            {
                GameObjectTypes.Add(type);
            }
        }

        internal void SaveGameObjectTypes()
        {
            // Write all Game Types created to file.
            using (XmlWriter writer = XmlWriter.Create("GameObjectTypes.xml", new XmlWriterSettings { Indent = true }))
            {
                writer.WriteStartDocument();
                writer.WriteStartElement("GameObjectTypes");

                foreach (var gameObjectType in GameObjectTypes)
                {
                    writer.WriteStartElement("GameObjectType");
                    writer.WriteAttributeString("Name", gameObjectType.Name);
                    writer.WriteAttributeString("Type", gameObjectType.Type);
                    writer.WriteAttributeString("ResourceId", gameObjectType.ResourceId.ToString());
                    writer.WriteAttributeString("AssetPath", gameObjectType.AssetPath);
                    foreach (var property in gameObjectType.Properties)
                    {
                        writer.WriteStartElement("Property");
                        writer.WriteAttributeString(property.Key, property.Value);
                        writer.WriteEndElement();
                    }
                    writer.WriteEndElement();
                }
                writer.WriteEndElement();

                writer.WriteEndDocument();

            }
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
