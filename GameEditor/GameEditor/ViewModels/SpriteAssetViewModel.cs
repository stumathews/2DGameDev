using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using GameEditor.Models;

namespace GameEditor.ViewModels
{
    public class SpriteAssetViewModel : ViewModelBase, INotifyPropertyChanged
    {
        private SpriteAsset asset;

        public SpriteAsset Asset
        {
            get => asset;
            protected set
            {
                if (Equals(value, asset)) return;
                asset = value;
                OnPropertyChanged(nameof(Asset));
            }
        }
    }
}
