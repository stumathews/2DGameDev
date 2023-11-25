using System;
using GameEditor.Models;
using System.Collections.Generic;
using System.IO;
using System.Windows;
using System.Xml;

namespace GameEditor.Utils
{
    public class GameObjectTypeManager
    {
        // Parse GameObject Types xml file
        public static List<GameObjectType> GetGameObjectTypes(string filename)
        {
            var gameObjectTypes = new List<GameObjectType>();
            if (!File.Exists(filename))
            {
                return gameObjectTypes;
            }

            var settings = new XmlReaderSettings() { DtdProcessing = DtdProcessing.Ignore };
            var reader = XmlReader.Create(filename, settings);

            GameObjectType gameObjectType = null;

            while (reader.Read())
            {
                if (reader.NodeType == XmlNodeType.Element)
                {
                    if (reader.Name.Equals("GameObjectType"))
                    {
                        gameObjectType = new GameObjectType
                        {
                            Properties = new List<KeyValuePair<string, string>>(),
                            AssetPath = reader.GetAttribute("AssetPath"),
                            Name = reader.GetAttribute("Name"),
                            ResourceId = int.Parse(reader.GetAttribute("ResourceId")),
                            Type = reader.GetAttribute("Type")
                        };
                    }                

                    if (reader.Name.Equals("Property"))
                    {
                        for (var attIndex = 0; attIndex < reader.AttributeCount; attIndex++)
                        {
                            reader.MoveToAttribute(attIndex);
                            gameObjectType.Properties.Add(new KeyValuePair<string, string>(reader.Name, reader.Value));
                        }
                    }
                }

                if ((reader.NodeType == XmlNodeType.EndElement || reader.IsEmptyElement) && reader.Name.Equals("GameObjectType"))
                {
                    gameObjectTypes.Add(gameObjectType);
                }
            }


            return gameObjectTypes;
        }

        public static void SaveGameObjectTypes(List<GameObjectType> gameObjectTypes)
        {
            try
            {
                // Write all Game Types created to file.
                using (var writer = XmlWriter.Create("GameObjectTypes.xml", new XmlWriterSettings { Indent = true }))
                {
                    writer.WriteStartDocument();
                    writer.WriteStartElement("GameObjectTypes");

                    foreach (var gameObjectType in gameObjectTypes)
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
            catch (Exception e)
            {
                MessageBox.Show($"Error Saving the Game Object Types: {e.Message}");
            }
        }
    }
}
