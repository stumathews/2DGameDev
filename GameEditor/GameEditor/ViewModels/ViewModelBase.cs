using System;
using System.ComponentModel;

namespace GameEditor.ViewModels
{
    public class ViewModelBase
    {
        public event PropertyChangedEventHandler PropertyChanged;
        protected virtual void OnPropertyChanged(string propertyName)
        {
            VerifyPropertyName(propertyName);

            var handler = PropertyChanged;
            if (handler == null) return;

            var e = new PropertyChangedEventArgs(propertyName);
            handler(this, new PropertyChangedEventArgs(propertyName));
        }

        public void VerifyPropertyName(string propertyName)
        {
            // Verify that the property name matches a real, public instance property of this object
            if (TypeDescriptor.GetProperties(this)[propertyName] != null) return;

            var msg = "Invalid property name: " + propertyName;

            throw new Exception(msg);
        }
    }
}
