namespace GameEditor.Models
{
    public class SpriteAsset : GraphicAsset
    {
        private readonly KeyFrameCollection keyFrames;
        public ColorKey ColorKey { get; }


        /*
         <Asset uid="18" scene="0" name="snap_player" type="graphic" filename="game/assets/character2.png" width="32" height="32">
         <colorkey red="255" blue="255" green="255"/>
         <sprite>
          <animation>
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
          </animation> 
        </sprite>  
      </Asset>
         */

        public SpriteAsset(int uid, string name, string filename, int width, int height, ColorKey colorKey, KeyFrameCollection keyFrames) : base(uid, name, filename, width, height)
        {
            this.keyFrames = keyFrames;
            ColorKey = colorKey;
        }
        
    }
}