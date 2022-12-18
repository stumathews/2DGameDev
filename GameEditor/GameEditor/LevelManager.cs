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
                XmlReaderSettings settings = new XmlReaderSettings
                {
                    DtdProcessing = DtdProcessing.Ignore
                };
                XmlReader reader = XmlReader.Create(openFileDialog.FileName, settings);
                RoomViewModel roomViewModel = null;
                while (reader.Read())
                {                    
                    if (reader.NodeType == XmlNodeType.Element)
                    {
                        if(reader.Name.Equals("level"))
                        {
                            level.NumCols = int.Parse(reader.GetAttribute("cols"));
                            level.NumRows = int.Parse(reader.GetAttribute("rows"));
                            level.AutoPopulatePickups = bool.Parse(reader.GetAttribute("autoPopulatePickups"));
                        }

                        if(reader.Name.Equals("room"))
                        {
                            roomViewModel = new RoomViewModel
                            {
                                RoomNumber = int.Parse(reader.GetAttribute("number")),
                                TopWallVisibility = bool.Parse(reader.GetAttribute("top")) ? Visibility.Visible : Visibility.Hidden,
                                RightWallVisibility = bool.Parse(reader.GetAttribute("right")) ? Visibility.Visible : Visibility.Hidden,
                                BottomWallVisibility = bool.Parse(reader.GetAttribute("bottom")) ? Visibility.Visible : Visibility.Hidden,
                                LeftWallVisibility = bool.Parse(reader.GetAttribute("left")) ? Visibility.Visible : Visibility.Hidden
                            };
                        }
                        if (reader.Name.Equals("object"))
                        {
                            roomViewModel.ResidentGameObjectType = new GameObjectType
                            {
                                AssetPath = reader.GetAttribute("assetPath"),
                                Name = reader.GetAttribute("name"),
                                ResourceId = int.Parse(reader.GetAttribute("resourceId")),
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
            OnLevelLoaded?.Invoke(this, new EventArgs());
            return level;            
        }
    }
}
