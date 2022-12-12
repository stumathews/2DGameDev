using GameEditor.Views;
using System.Collections.Generic;
using System.Xml;

namespace GameEditor.Utils
{
    public static class ResourceManager
    {
        public static List<AssetModel> ParseResources(string basePath = "C:\\repos\\2DGameDev\\")
        { 
            var assets = new List<AssetModel>();
            var doc = new XmlDocument();

            doc.Load(basePath + "game\\Resources.xml");

            foreach (XmlNode assetNode in doc.DocumentElement.SelectSingleNode("/Assets").ChildNodes)
            {
                assets.Add(new AssetModel
                {
                    Name = assetNode.Attributes["name"].Value,
                    Uid = int.Parse(assetNode.Attributes["uid"].Value),
                    Type = assetNode.Attributes["type"].Value,
                    Path = assetNode.Attributes["filename"].Value,
                    InnerXml = assetNode.InnerXml
                });
            }
            return assets;
        }
    }
}
