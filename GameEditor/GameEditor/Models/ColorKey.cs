namespace GameEditor.Models
{
    public class ColorKey
    {
        public ColorKey(int red, int green, int blue)
        {
            this.Red = red;
            this.Green = green;
            this.Blue = blue;
        }

        public int Red { get; }
        public int Green { get; }
        public int Blue { get; }
    }
}