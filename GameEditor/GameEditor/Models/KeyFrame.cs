namespace GameEditor.Models
{
    public class KeyFrame
    {
        public KeyFrame(int x, int y, int width, int height, string group)
        {
            X = x;
            Y = y;
            Width = width;
            Height = height;
            Group = group;
        }

        public int X { get; }
        public int Y { get; }
        public int Width { get; }
        public int Height { get; }
        public string Group { get; }
    }
}