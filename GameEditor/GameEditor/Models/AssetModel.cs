using GameEditor.ViewModels;
using System.ComponentModel;

namespace GameEditor.Views
{
    public class AssetModel
    {
        private string _name;
        private string _path;
        private string _type;
        private int uid;
        private string _innerXml;

        public string Name { get => _name; set => _name = value; }
        public string Path { get => _path; set => _path = value; }
        public string Type { get => _type; set => _type = value; }
        public int Uid { get => uid; set => uid = value; }
        public string InnerXml { get => _innerXml; set => _innerXml = value; }
    }
}