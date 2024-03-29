﻿using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Linq;
using System.Windows;
using System.Windows.Input;
using System.Xml;
using System.Xml.Linq;
using GameEditor.Models;
using Microsoft.Win32;

namespace GameEditor.ViewModels
{
    public class ResourcesViewModel : ViewModelBase, INotifyPropertyChanged
    {
        private readonly string basePath;
        private string resourcesXml;
        public ObservableCollection<Asset> Assets { get; set; }
        
        public ICommand AddResourceCommand { get; }
        public ICommand RemoveResourceCommand { get; }
        public ICommand SaveResourcesAsCommand { get; }

        public string ResourcesXml
        {
            get => resourcesXml;
            set
            {
                if (value == resourcesXml) return;
                resourcesXml = value;
                OnPropertyChanged(nameof(ResourcesXml));
            }
        }

        public ResourcesViewModel(string basePath = "C:\\repos\\2DGameDev\\")
        {
            this.basePath = basePath;
            Assets = new ObservableCollection<Asset>();
            AddResourceCommand = new RelayCommand((o) => { MessageBox.Show("Add Resource"); });
            RemoveResourceCommand = new RelayCommand((o) => { MessageBox.Show("Remove Resource"); });
            SaveResourcesAsCommand = new RelayCommand(SaveResourcesToFile);
            ParseResources();
            ResourcesXml = GetResourcesXml();
        }

        private void SaveResourcesToFile(object o)
        {
            var saveFileDialog = GetSaveFileDialog();

            if (saveFileDialog.ShowDialog() is false)
            {
                return;
            }

            // Create the xml nodes
            var assetsNode = CreateResourcesXml();
            
            // Write the xml nodes to file
            using (var writer = XmlWriter.Create(saveFileDialog.FileName, GetXmlWriterSettings()))
            {
                assetsNode.WriteTo(writer);
            }
        }

        private string GetResourcesXml()
        {
            return CreateResourcesXml().ToString();
        }

        private XElement CreateResourcesXml()
        {
            var assetsNode = new XElement("Assets",
                from asset in Assets
                let anAsset =
                    new XElement("Asset",
                        new XAttribute("uid", asset.Uid),
                        new XAttribute("name", asset.Name),
                        new XAttribute("type", asset.Type),
                        new XAttribute("filename", asset.Path),
                        GetInnerXmlAsElements(asset))
                select anAsset);
            return assetsNode;
        }

        private static IEnumerable<XElement> GetInnerXmlAsElements(Asset asset)
        {
            if (string.IsNullOrEmpty(asset.InnerXml)) return null;

            var doc = new XmlDocument();
            var myRootedXml = "<root>" + asset.InnerXml + "</root>"; // need to use a fake root for our inner xml in order to parse it
                
            doc.LoadXml(myRootedXml);

            return XElement.Parse(doc.InnerXml).Elements();
        }

        private static SaveFileDialog GetSaveFileDialog()
        {
            return new SaveFileDialog
            {
                Filter = "XML Files (*.xml)|*.xml;"
            };
        }

        private static XmlWriterSettings GetXmlWriterSettings()
        {
            return new XmlWriterSettings
            {
                Indent = true,
            };
        }

        private void ParseResources()
        {
            // The resource manager will deserialize the resources for us in the resources.xml files
            var assets = ResourceManager.ParseResources(basePath);
            foreach (var asset in assets)
            {
                Assets.Add(asset);
            }
        }
    }
}

/*
<?xml version="1.0" encoding="utf-8"?>

<!-- List of all asset in the game -->
<Assets>
  <Asset uid="1" scene="4" name="LevelMusic4" type="music" filename="Assets/Music/MainTheme.wav"></Asset>
  <Asset uid="2" scene="0" name="scratch.wav" type="fx" filename="Assets/scratch.wav"></Asset>
  <Asset uid="3" scene="0" name="high.wav" type="fx" filename="Assets/high.wav"></Asset>
  <Asset uid="4" scene="0" name="medium.wav" type="fx" filename="Assets/medium.wav"></Asset>
  <Asset uid="5" scene="0" name="low.wav" type="fx" filename="Assets/low.wav"></Asset>  
  <Asset uid="6" scene="1" name="p1.png" type="graphic" filename="Assets/Platformer/Base pack/Player/p1_front.png" width="66" height="92">
		
	</Asset>
  <Asset uid="7" scene="2" name="p2.png" type="graphic" filename="Assets/Platformer/Base pack/Player/p2_front.png" width="66" height="92">
		
	</Asset>
  <Asset uid="8" scene="2" name="p3.png" type="graphic" filename="Assets/Platformer/Base pack/Player/p3_front.png" width="66" height="92">
		
	</Asset>
  <Asset uid="9" scene="3" name="p1.png" type="graphic" filename="Assets/Platformer/Base pack/Player/p1_walk/p1_walk.png" width="256" height="512">
    <sprite>
      <animation>
        <keyframes>
          <keyframe x="0" y="0" w="67" h="92"/>
          <keyframe x="66" y="0" w="66" h="93"/>
          <keyframe x="132" y="0" w="67" h="92"/>
          <keyframe x="0" y="93" w="67" h="93"/>
          <keyframe x="67" y="93" w="66" h="93"/>
          <keyframe x="133" y="93" w="71" h="92"/>
          <keyframe x="0" y="186" w="71" h="93"/>
          <keyframe x="71" y="186" w="71" h="93"/>
          <keyframe x="142" y="186" w="70" h="93"/>
          <keyframe x="0" y="279" w="71" h="93"/>
          <keyframe x="71" y="279" w="67" h="92"/>
        </keyframes>
      </animation> 
    </sprite>  
  </Asset>
  <Asset uid="10" scene="3" name="p2.png" type="graphic" filename="Assets/Platformer/Base pack/Player/p2_walk/p2_walk.png" width="256" height="512">
     <sprite>
      <animation>
        <keyframes>
          <keyframe x="0" y="0" w="67" h="92"/>
          <keyframe x="67" y="0" w="66" h="93"/>
          <keyframe x="133" y="0" w="66" h="91"/>
          <keyframe x="0" y="93" w="67" h="91"/>
          <keyframe x="67" y="93" w="66" h="91"/>
          <keyframe x="133" y="93" w="69" h="90"/>
          <keyframe x="0" y="184" w="69" h="91"/>
          <keyframe x="69" y="184" w="69" h="92"/>
          <keyframe x="138" y="184" w="68" h="92"/>
          <keyframe x="0" y="276" w="69" h="93"/>
          <keyframe x="69" y="276" w="67" h="92"/>
        </keyframes>
      </animation> 
    </sprite>  
  </Asset>
  <Asset uid="11" scene="3" name="p3.png" type="graphic" filename="Assets/Platformer/Base pack/Player/p3_walk/p3_walk.png" width="256" height="512">
     <sprite>
      <animation>
        <keyframes>
          <keyframe x="0" y="0" w="66" h="92"/>
          <keyframe x="66" y="0" w="66" h="93"/>
          <keyframe x="132" y="0" w="66" h="93"/>
          <keyframe x="0" y="93" w="66" h="93"/>
          <keyframe x="66" y="93" w="66" h="93"/>
          <keyframe x="132" y="93" w="71" h="92"/>
          <keyframe x="0" y="186" w="71" h="93"/>
          <keyframe x="71" y="186" w="71" h="93"/>
          <keyframe x="142" y="186" w="70" h="92"/>
          <keyframe x="0" y="279" w="71" h="93"/>
          <keyframe x="71" y="279" w="66" h="92"/>
        </keyframes>
      </animation> 
    </sprite>  
  </Asset>
  <Asset uid="12" scene="0" name="arial.ttf" type="font" filename="Assets/fonts/arial.ttf"></Asset>
  <Asset uid="13" scene="0" name="beep.wav" type="fx" filename="Assets/beep.wav"></Asset>
  <Asset uid="14" scene="0" name="boop.wav" type="fx" filename="Assets/boop.wav"></Asset>
  <Asset uid="15" scene="1" name="LevelMusic1" type="music" filename="Assets/Music/8BitMetal.wav"></Asset>
  <Asset uid="16" scene="2" name="LevelMusic2" type="music" filename="Assets/Music/FranticLevel.wav"></Asset>
  <Asset uid="17" scene="3" name="LevelMusic3" type="music" filename="Assets/Music/HappyLevel.wav"></Asset>
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
	<Asset uid="188" scene="0" name="edge_player" type="graphic" filename="game/assets/character1.png" width="32" height="32">
     <colorkey red="255" blue="255" green="255"/>
     <sprite>
      <animation>
        <keyframes duration="150"> <!-- Wait 150 ms before switching to next frame -->
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
	<Asset uid="19" scene="0" name="goldcoin" type="graphic" filename="game/assets/coin_gold.png" width="32" height="32">
     <colorkey red="255" blue="255" green="255"/>
     <sprite>
      <animation>
        <keyframes duration="100"> <!-- Wait 0 ms before switching to next frame -->
          <keyframe x="0" y="0" w="32" h="32" />
          <keyframe x="32" y="0" w="32" h="32" />
          <keyframe x="64" y="0" w="32" h="32" />
          <keyframe x="96" y="0" w="32" h="32" />
          <keyframe x="128" y="0" w="32" h="32" />
          <keyframe x="160" y="0" w="32" h="32" />
          <keyframe x="192" y="0" w="32" h="32" />
          <keyframe x="224" y="0" w="32" h="32" />
        </keyframes>
      </animation> 
    </sprite>  
  </Asset>
	<Asset uid="20" scene="0" name="silvercoin" type="graphic" filename="game/assets/coin_silver.png" width="32" height="32">
     <colorkey red="255" blue="255" green="255"/>
     <sprite>
      <animation>
        <keyframes duration="100"> <!-- Wait 0 ms before switching to next frame -->
          <keyframe x="0" y="0" w="32" h="32" />
          <keyframe x="32" y="0" w="32" h="32" />
          <keyframe x="64" y="0" w="32" h="32" />
          <keyframe x="96" y="0" w="32" h="32" />
          <keyframe x="128" y="0" w="32" h="32" />
          <keyframe x="160" y="0" w="32" h="32" />
          <keyframe x="192" y="0" w="32" h="32" />
          <keyframe x="224" y="0" w="32" h="32" />
        </keyframes>
      </animation> 
    </sprite>  
  </Asset>
	<Asset uid="21" scene="0" name="coppercoin" type="graphic" filename="game/assets/coin_copper.png" width="32" height="32">
    <colorkey red="255" blue="255" green="255"/>
    <sprite>
    <animation>
      <keyframes duration="100"> <!-- Wait 100 ms before switching to next frame -->
        <keyframe x="0" y="0" w="32" h="32" />
        <keyframe x="32" y="0" w="32" h="32" />
        <keyframe x="64" y="0" w="32" h="32" />
        <keyframe x="96" y="0" w="32" h="32" />
        <keyframe x="128" y="0" w="32" h="32" />
        <keyframe x="160" y="0" w="32" h="32" />
        <keyframe x="192" y="0" w="32" h="32" />
        <keyframe x="224" y="0" w="32" h="32" />
      </keyframes>
    </animation>
		</sprite>
	</Asset>
	<Asset uid="22" scene="0" name="hudspritesheet" type="graphic" filename="game/assets/hud_spritesheet.png" width="269" height="32">
		<colorkey red="255" blue="255" green="255"/>
		<sprite>
			<animation>
				<keyframes>
					<!-- Wait 100 ms before switching to next frame -->
					<keyframe name="hud_0.png" x="230" y="0" w="30" h="38"/>
					<keyframe name="hud_1.png" x="196" y="41" w="26" h="37"/>
					<keyframe name="hud_2.png" x="55" y="98" w="32" h="38"/>
					<keyframe name="hud_3.png" x="239" y="80" w="28" h="38"/>
					<keyframe name="hud_4.png" x="238" y="122" w="29" h="38"/>
					<keyframe name="hud_5.png" x="238" y="162" w="28" h="38"/>
					<keyframe name="hud_6.png" x="230" y="40" w="30" h="38"/>
					<keyframe name="hud_7.png" x="226" y="206" w="32" h="39"/>
					<keyframe name="hud_8.png" x="192" y="206" w="32" h="40"/>
					<keyframe name="hud_9.png" x="196" y="0" w="32" h="39"/>
					<keyframe name="hud_coins.png" x="55" y="0" w="47" h="47"/>
					<keyframe name="hud_gem_blue.png" x="104" y="0" w="46" h="36"/>
					<keyframe name="hud_gem_green.png" x="98" y="185" w="46" h="36"/>
					<keyframe name="hud_gem_red.png" x="98" y="147" w="46" h="36"/>
					<keyframe name="hud_gem_yellow.png" x="98" y="223" w="46" h="36"/>
					<keyframe name="hud_heartEmpty.png" x="0" y="47" w="53" h="45"/>
					<keyframe name="hud_heartFull.png" x="0" y="94" w="53" h="45"/>
					<keyframe name="hud_heartHalf.png" x="0" y="0" w="53" h="45"/>
					<keyframe name="hud_keyBlue.png" x="146" y="147" w="44" h="40"/>
					<keyframe name="hud_keyBlue_disabled.png" x="150" y="38" w="44" h="40"/>
					<keyframe name="hud_keyGreem_disabled.png" x="104" y="38" w="44" h="40"/>
					<keyframe name="hud_keyGreen.png" x="192" y="122" w="44" h="40"/>
					<keyframe name="hud_keyRed.png" x="193" y="80" w="44" h="40"/>
					<keyframe name="hud_keyRed_disabled.png" x="192" y="164" w="44" h="40"/>
					<keyframe name="hud_keyYellow.png" x="146" y="189" w="44" h="40"/>
					<keyframe name="hud_keyYellow_disabled.png" x="147" y="80" w="44" h="40"/>
					<keyframe name="hud_p1.png" x="55" y="49" w="47" h="47"/>
					<keyframe name="hud_p1Alt.png" x="0" y="141" w="47" h="47"/>
					<keyframe name="hud_p2.png" x="49" y="141" w="47" h="47"/>
					<keyframe name="hud_p2Alt.png" x="0" y="190" w="47" h="47"/>
					<keyframe name="hud_p3.png" x="49" y="190" w="47" h="47"/>
					<keyframe name="hud_p3Alt.png" x="98" y="98" w="47" h="47"/>
					<keyframe name="hud_x.png" x="0" y="239" w="30" h="28"/>
				</keyframes>
			</animation>
		</sprite>
	</Asset>
	<Asset uid="23" scene="0" name="win.wav" type="fx" filename="Assets/win.wav"></Asset>
	<Asset uid="24" scene="0" name="AutoLevelMusic" type="music" filename="Assets/Music/IntroLoop.wav"></Asset>
	<Asset uid="25" scene="4" name="LevelMusic5" type="music" filename="Assets/Music/SubduedTheme.wav"></Asset>
	<Asset uid="26" scene="0" name="kenvector_future2.ttf" type="font" filename="Assets/fonts/kenvector_future2.ttf"></Asset>
	<Asset uid="27" scene="0" name="amg2.png" type="graphic" filename="game/assets/amg2.png" width="32" height="32">
		<colorkey red="255" blue="255" green="255"/>
		<sprite>
			<animation>
				<keyframes duration="150"> <!-- Wait 150 ms before switching to next frame -->
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
	<Asset uid="28" scene="0" name="bmg1.png" type="graphic" filename="game/assets/bmg1.png" width="32" height="32">
		<colorkey red="255" blue="255" green="255"/>
		<sprite>
			<animation>
				<keyframes duration="150"> <!-- Wait 150 ms before switching to next frame -->
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
	<Asset uid="29" scene="0" name="chr1.png" type="graphic" filename="game/assets/chr1.png" width="32" height="32">
		<colorkey red="255" blue="255" green="255"/>
		<sprite>
			<animation>
				<keyframes duration="150"> <!-- Wait 150 ms before switching to next frame -->
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
	<Asset uid="30" scene="0" name="dvl1.png" type="graphic" filename="game/assets/dvl1.png" width="32" height="32">
		<colorkey red="255" blue="255" green="255"/>
		<sprite>
			<animation>
				<keyframes duration="150"> <!-- Wait 150 ms before switching to next frame -->
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
</Assets>

 */