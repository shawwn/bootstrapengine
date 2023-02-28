using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Syncfusion.Windows.Forms;
using Syncfusion.Windows.Forms.Tools;

namespace Editor.Dialogs
{
    public partial class MaterialSelectionDialog : Office2007Form
    {
        public static bool NeedPopulate;

        Bootstrap.Material _selection;

        public MaterialSelectionDialog()
        {
            InitializeComponent();

            // set the selection panel to null by default.
            _selection = null;

            // focus the thumbnail selector.
            thumbnailSelector.Focus();
        }

        ~MaterialSelectionDialog()
        {
            if (_selection != null)
                _selection.Dispose();
        }

        public Bootstrap.Material Selection
        {
            get { return _selection; }
        }

        void Populate()
        {
            NeedPopulate = false;

            // clear the current thumbnails.
            thumbnailSelector.Clear();

            // populate the thumbnail control.
            string[] materialNames = Bootstrap.Material.GetLoadedMaterialNames();

            // iterate through each material and create an icon.
            foreach( string materialName in materialNames )
            {
                // create a new material for the item.
                Bootstrap.Material material = new Bootstrap.Material( materialName );

                // try to get a decent thumbnail for the material.
                Bitmap bitmap = null;
                if( material.PassCount > 0 )
                {
                    Bootstrap.MaterialPass pass = material.GetPass( 0 );
                    string textureName = pass.DiffuseStage.Texture;
                    if( textureName != null && textureName != "" )
                    {
                        bitmap = Program.ThumbnailMgr.GetThumbnail( textureName );
                    }
                    else
                    {
                        string uberTextureName = pass.UberTexture;
                        if( uberTextureName != null && uberTextureName != "" )
                        {
                            using( Bootstrap.UberTexture uberTexture = new Bootstrap.UberTexture( uberTextureName ) )
                            {
                                bitmap = uberTexture.MakeThumbnail();
                            }
                        }
                    }
                }

                // set the bitmap to the error bitmap if no usable preview could
                // be found.
                if( bitmap == null )
                    bitmap = Program.ThumbnailMgr.GetErrorThumbnail();

                // add a new thumbnail and dispose the material.
                thumbnailSelector.Add( material.ShortName, bitmap, materialName );
                material.Dispose();
            }
        }

        private void thumbnailSelector_SelectionChanged(object sender, EventArgs e)
        {
            // clear the selection.
            if (_selection != null)
            {
                _selection.Dispose();
                _selection = null;
            }

            // get the current selection (the last thing that was clicked on).
            List< Controls.Thumbnail > selection = thumbnailSelector.Selection;
            int last = selection.Count-1;
            if (selection.Count > 0)
                _selection = new Bootstrap.Material(selection[last].UserData);
        }

        private void MaterialSelectionDialog_VisibleChanged(object sender, EventArgs e)
        {
            // If not, try to populate the dialog box.
            if (this.Visible)
            {
                if (thumbnailSelector.ThumbnailCount == 0 || NeedPopulate)
                    Populate();
            }
        }

        private void bnReload_Click( object sender, EventArgs e )
        {
            Populate();
        }

        private void thumbnailSelector_SelectionMade(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            this.Close();
        }
    }
}