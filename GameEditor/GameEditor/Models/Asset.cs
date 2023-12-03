namespace GameEditor.Models
{
    /// <summary>
    /// Generic view of any asset
    /// </summary>
    public class Asset
    {
        /// <summary>
        /// Asset Name
        /// </summary>
        public string Name { get; set; }

        /// <summary>
        /// Asset path
        /// </summary>
        public string Path { get; set; }

        /// <summary>
        /// Asset Type
        /// </summary>
        public string Type { get; set; }

        /// <summary>
        /// Asset UID
        /// </summary>
        public int Uid { get; set; }

        /// <summary>
        /// Specific Asset XML
        /// <remarks>Each specific asset type can manage\load its own internal XML</remarks>
        /// </summary>
        public string InnerXml { get; set; }
    }
}