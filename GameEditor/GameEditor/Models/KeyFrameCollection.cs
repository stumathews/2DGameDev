using System.Collections.Generic;

namespace GameEditor.Models
{
    /*
      <keyframes duration="150"> <!-- Wait 0 ms before switching to next frame -->
                <keyframe x="0" y="0" w="32" h="32" group="up"/>
                <keyframe x="32" y="0" w="32" h="32" group="up"/>
                <keyframe x="64" y="0" w="32" h="32" group="right"/>
                <keyframe x="96" y="0" w="32" h="32" group="right"/>
                <keyframe x="128" y="0" w="32" h="32" group="down"/>
                <keyframe x="160" y="0" w="32" h="32" group="down"/>
                <keyframe x="192" y="0" w="32" h="32" group="left"/>
                <keyframe x="224" y="0" w="32" h="32" group="left"/>
            </keyframes>
     */

    public class KeyFrameCollection
    {
        public KeyFrameCollection(int durationMs, List<KeyFrame> keyFrames = null)
        {
            DurationMs = durationMs;
            KeyFrames = keyFrames ?? new List<KeyFrame>();
        }

        public int DurationMs { get; }
        public List<KeyFrame> KeyFrames { get; }

        public static KeyFrameCollection Test = new KeyFrameCollection(100, new List<KeyFrame>
        {
            new KeyFrame(0,0,32,32, "up"),
            new KeyFrame(32,0,32,32, "up"),
            new KeyFrame(64,0,32,32, "right"),
            new KeyFrame(96,0,32,32, "right"),
            new KeyFrame(128,0,32,32, "down"),
            new KeyFrame(160,0,32,32, "down"),
            new KeyFrame(192,0,32,32, "left"),
            new KeyFrame(224,0,32,32, "left"),
        });
    }
}