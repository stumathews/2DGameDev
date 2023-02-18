using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;
using System.Xml;
using System.Xml.Linq;
using GameEditor.Models;
using GameEditor.Utils;
using Microsoft.Win32;

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

            var saveFileDialog = new SaveFileDialog
            {
                Filter = "XML Files (*.xml)|*.xml;"
            };

            try
            {
                if (!(saveFileDialog.ShowDialog() is true)) return;

                OnAboutToSaveFile?.Invoke(this, $"Saving File '{saveFileDialog.FileName}'...");
                
                var levelNode = new XElement("level",
                    new XAttribute("cols", level.NumCols),
                    new XAttribute("rows", level.NumRows),
                    new XAttribute("autoPopulatePickups", level.AutoPopulatePickups.ToString()),
                    from room in level.Rooms
                        
                    let gameObject = room.ResidentGameObjectType
                    let roomEl = new XElement("room",
                        new XAttribute("number", room.RoomNumber),
                        new XAttribute("top", room.TopWallVisibility.ToBoolString()),
                        new XAttribute("right", room.RightWallVisibility.ToBoolString()),
                        new XAttribute("bottom", room.BottomWallVisibility.ToBoolString()),
                        new XAttribute("left", room.LeftWallVisibility.ToBoolString()),
                        // <object>
                        gameObject != null 
                            ? new XElement("object",
                            new XAttribute("name", gameObject.Name),
                            new XAttribute("type", gameObject.Type ?? ""),
                            new XAttribute("resourceId", gameObject.ResourceId),
                            new XAttribute("assetPath", gameObject.AssetPath), 
                            // <property>
                            from property in gameObject.Properties
                            let key = new XAttribute("name", property.Key)
                            let value = new XAttribute("value", property.Value)
                            select new XElement("property", key, value)) 
                            : null) 
                    select roomEl);

                using (var writer = XmlWriter.Create(saveFileDialog.FileName, xmlSettings))
                {
                    levelNode.WriteTo(writer);
                }

                OnFileSaved?.Invoke(this,  $"Saved File '{saveFileDialog.FileName}'.");
            }
            catch(Exception ex)
            {
                throw new Exception ($"Error saving level file '{saveFileDialog.FileName}': {ex.Message}");
            }   
        }

        public Level LoadLevelFile()
        {
            var openFileDialog = new OpenFileDialog();

            Level level = null;
            if(openFileDialog.ShowDialog() is true)
            {
                level = (from levels in XElement.Load(openFileDialog.FileName).AncestorsAndSelf()
                        select new Level
                        {
                            Rooms = levels.Descendants("room").Select(r => new RoomViewModel
                            {
                                RoomNumber = GetAsNumber(r, "number"),
                                TopWallVisibility = GetAsVisibilityFromTruthString(r,"top"),
                                LeftWallVisibility = GetAsVisibilityFromTruthString(r,"left"),
                                RightWallVisibility = GetAsVisibilityFromTruthString(r,"right"),
                                BottomWallVisibility = GetAsVisibilityFromTruthString(r, "bottom"),
                                ResidentGameObjectType = r.Descendants("object").Select(o => new GameObjectType()
                                {
                                    Name = GetAsString(o, "name"),
                                    AssetPath = GetAsString(o, "assetPath"),
                                    Properties = o.Descendants("property").Select(p => new KeyValuePair<string, string>(key: GetAsString(p, "name"), value: GetAsString(p, "value"))).ToList()
                                }).SingleOrDefault()
                            }).ToList(),
                        }).SingleOrDefault();

            }
            OnLevelLoaded?.Invoke(this, EventArgs.Empty);
            return level;            
        }

        private static string GetAsString(XElement o, string attributeName) 
            => (o.Attribute(attributeName) ?? throw new NullReferenceException(attributeName)).Value;

        private static Visibility GetAsVisibility(XElement r, string attributeName) 
            => GetAsString(r, attributeName).ToVisibility();

        private static Visibility GetAsVisibilityFromTruthString(XElement r, string attributeName) 
            => GetAsString(r, attributeName).VisibilityFromTruthString();

        private static int GetAsNumber(XElement r, string attributeName) 
            => GetAsString(r, attributeName).ToNumber();
    }
}
