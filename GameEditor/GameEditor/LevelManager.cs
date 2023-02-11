using GameEditor.Models;
using Microsoft.Win32;
using System;
using System.Collections.Generic;
using System.Windows;
using System.Xml;

namespace GameEditor.ViewModels
{
    public class LevelManager 
    {
        public event EventHandler<string> OnFileSaved;
        public event EventHandler<string> OnAboutToSaveFile;
        public event EventHandler OnLevelLoaded;

        public void SaveLevelFile(Level level)
        {
            // Collect all the rooms and serialize to XML
            var xmlSettings = new XmlWriterSettings()
            {
                Indent = true,
            };

            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "XML Files (*.xml)|*.xml;";
            try
            {
                if(saveFileDialog.ShowDialog() is true)
                {
                    OnAboutToSaveFile?.Invoke(this, $"Saving File '{saveFileDialog.FileName}'...");
                    using (XmlWriter writer = XmlWriter.Create(saveFileDialog.FileName, xmlSettings))
                    {
                        writer.WriteStartDocument();
                        writer.WriteStartElement("level"); // <level ...
                        writer.WriteAttributeString("cols", level.NumCols.ToString());
                        writer.WriteAttributeString("rows", level.NumRows.ToString());
                        writer.WriteAttributeString("autoPopulatePickups", level.AutoPopulatePickups.ToString());
                        foreach(var roomViewModel in level.Rooms)
                        {
                            var topVisible = roomViewModel.TopWallVisibility == Visibility.Visible;
                            var rightVisible = roomViewModel.RightWallVisibility == Visibility.Visible;
                            var bottomVisible = roomViewModel.BottomWallVisibility == Visibility.Visible;
                            var leftVisible = roomViewModel.LeftWallVisibility == Visibility.Visible;

                            writer.WriteStartElement("room"); // <room ...
                            writer.WriteAttributeString("number", roomViewModel.RoomNumber.ToString());
                            writer.WriteAttributeString("top", topVisible.ToString() );
                            writer.WriteAttributeString("right", rightVisible.ToString() );
                            writer.WriteAttributeString("bottom", bottomVisible.ToString() );
                            writer.WriteAttributeString("left", leftVisible.ToString() );

                            if(roomViewModel.ResidentGameObjectType != null)
                            {
                                var gameObjectType = roomViewModel.ResidentGameObjectType;
                                writer.WriteStartElement("object"); //<object ...
                                writer.WriteAttributeString("name", gameObjectType.Name);
                                writer.WriteAttributeString("type", gameObjectType.Type);
                                writer.WriteAttributeString("resourceId", gameObjectType.ResourceId.ToString());
                                writer.WriteAttributeString("assetPath", gameObjectType.AssetPath);
                                foreach(var property in gameObjectType.Properties)
                                {
                                    writer.WriteStartElement("property"); //<property ..
                                    writer.WriteAttributeString("name", property.Key);
                                    writer.WriteAttributeString("value", property.Value);
                                    writer.WriteEndElement();
                                }
                                writer.WriteEndElement();
                            }

                            writer.WriteEndElement();
                        }
                        writer.WriteEndElement();
                        writer.WriteEndDocument();
                    }

                    OnFileSaved?.Invoke(this,  $"Saved File '{saveFileDialog.FileName}'.");
                } 
            }
            catch(Exception ex)
            {
                throw new Exception ($"Error saving level file '{saveFileDialog.FileName}': {ex.Message}");
            }   
        }

        public Level LoadLevelFile()
        {
            var openFileDialog = new OpenFileDialog();
            var level = new Level();

            if(openFileDialog.ShowDialog() is true)
            {
                var settings = new XmlReaderSettings
                {
                    DtdProcessing = DtdProcessing.Ignore
                };
                var reader = XmlReader.Create(openFileDialog.FileName, settings);
                RoomViewModel roomViewModel = null;
                
                while (reader.Read())
                {                    
                    if (reader.NodeType == XmlNodeType.Element)
                    {
                        if(reader.Name.Equals("level"))
                        {
                            level.NumCols = int.Parse(reader.GetAttribute("cols") ?? throw new Exception(
                                "NumCols Not found"));
                            level.NumRows = int.Parse(reader.GetAttribute("rows") ?? throw new Exception(
                                "NumRows Not found"));
                            level.AutoPopulatePickups = bool.Parse(reader.GetAttribute("autoPopulatePickups") ??
                                                                   throw new Exception(
                                                                       "AutoPopulatePickups not found"));
                        }

                        roomViewModel = new RoomViewModel();

                        if(reader.Name.Equals("room"))
                        {
                            roomViewModel.RoomNumber = int.Parse(reader.GetAttribute("number") ?? "0");
                            roomViewModel.TopWallVisibility = bool.Parse(reader.GetAttribute("top") ??
                                                                         throw new Exception(
                                                                             "Top wall visibility Not found"))
                                ? Visibility.Visible
                                : Visibility.Hidden;
                            roomViewModel.RightWallVisibility = bool.Parse(reader.GetAttribute("right") ??
                                                                           throw new Exception(
                                                                               "Right wall visibility Not found"))
                                    ? Visibility.Visible
                                    : Visibility.Hidden;
                            roomViewModel.BottomWallVisibility = bool.Parse(reader.GetAttribute("bottom") ??
                                                                            throw new Exception(
                                                                                "Bottom wall visibility Not found"))
                                    ? Visibility.Visible
                                    : Visibility.Hidden;
                            roomViewModel.LeftWallVisibility =
                                bool.Parse(reader.GetAttribute("left") ??
                                           throw new Exception("Left wall visibility Not found"))
                                    ? Visibility.Visible
                                    : Visibility.Hidden;
                        }
                        if (reader.Name.Equals("object"))
                        {
                            roomViewModel.ResidentGameObjectType = new GameObjectType
                            {
                                AssetPath = reader.GetAttribute("assetPath"),
                                Name = reader.GetAttribute("name"),
                                ResourceId = int.Parse(reader.GetAttribute("resourceId") ?? throw new Exception("Resource Id Not found")),
                                Type = reader.GetAttribute("type"),
                                Properties = new List<KeyValuePair<string, string>>()
                            };
                        }

                        if (reader.Name.Equals("property"))
                        {
                            for (var i = 0; i < reader.AttributeCount; i++)
                            {
                                reader.MoveToAttribute(i);
                                roomViewModel.ResidentGameObjectType.Properties.Add(new KeyValuePair<string, string>(reader.Name, reader.Value));
                            }
                        }
                    }

                    if ((reader.NodeType == XmlNodeType.EndElement || reader.IsEmptyElement) && reader.Name.Equals("room"))
                    {
                        level.Rooms.Add(roomViewModel);
                    }
                }
            }
            OnLevelLoaded?.Invoke(this, EventArgs.Empty);
            return level;            
        }
    }
}
