using System;
using System.Collections.Generic;
using System.Xml;
using GameEditor.Views;

namespace GameEditor
{
    public static class ResourceManager
    {
        public static List<AssetModel> ParseResources(string basePath = "C:\\repos\\2DGameDev\\")
        { 
            var assets = new List<AssetModel>();
            var doc = new XmlDocument();

            doc.Load(basePath + "game\\Resources.xml");

            if (doc.DocumentElement == null) throw new Exception("Could not load resources");

            var xmlNodeList = doc.DocumentElement.SelectSingleNode("/Assets")?.ChildNodes;
            
            if (xmlNodeList == null) throw new Exception("Could not load resources");

            // ReSharper disable once LoopCanBeConvertedToQuery
            foreach (XmlNode assetNode in xmlNodeList)
            {
                assets.Add(new AssetModel
                {
                    Name = assetNode.Attributes?["name"].Value,
                    Uid = int.Parse(assetNode.Attributes?["uid"].Value ?? throw new NullReferenceException("uid")),
                    Type = assetNode.Attributes["type"].Value,
                    Path = assetNode.Attributes["filename"].Value,
                    InnerXml = assetNode.InnerXml
                });
            }

            return assets;
        }
    }
}
