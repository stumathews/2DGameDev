namespace GameEditor.Models
{
    public class GraphicAsset : Asset
    {

        public GraphicAsset(int uid, string name, string filename, int width, int height)
        {
            Width = width;
            Height = height;
            Uid = uid;
            Name = name;
            Path = filename;
            Type = "graphic";
        }

        public int Width { get; }
        public int Height { get; }
    }
}