using System.Collections.Generic;
namespace GameEditor.Models
{
    public class GameObjectType
    {
        public string Name { get; set; }
        public string AssetPath { get; set; }
        public string Type { get;set; }
        public int ResourceId { get;set; }

        public List<KeyValuePair<string, string>> Properties { get;set;} = new List<KeyValuePair<string, string>>();
    }
}