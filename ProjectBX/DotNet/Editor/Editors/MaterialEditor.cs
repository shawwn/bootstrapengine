using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using Syncfusion.Windows.Forms;
using Syncfusion.Windows.Forms.Tools;

namespace Editor.Editors
{
    public partial class MaterialEditor : UserControl
    {
        // currently selected material.
        Bootstrap.Material  _material;
        bool                _populating;

        // ctor & dtor.
        public MaterialEditor()
        {
            InitializeComponent();
        }

        ~MaterialEditor()
        {

        }

        public Bootstrap.Material Material
        {
            get { return _material; }
            set
            {
                // get the name of the material and select it in the
                // thumbnail selector.
                _material = value;
                UpdatePanel();
            }
        }

        private void UpdatePanel()
        {
            // protect against unintentional changes to settings.
            _populating = true;

            // make sure there are no group-bar items.
            this.gbPasses.GroupBarItems.Clear();

            // set all of the controls.
            if ( _material == null )
            {
                // simply clear all of the controls.
                lbName.Text = "No material selected...";

                // return.
                _populating = false;
                return;
            }

            // assign values to all of the controls.
            lbName.Text = _material.Name;

            // get the error image.
            Bitmap errorImage = Program.ThumbnailMgr.GetErrorThumbnail();

            // make sure the material can be presented.
            int passCount = _material.PassCount;
            if ( passCount == 0 )
            {
                this.pbPreview.Image = errorImage;
                _populating = false;
                return;
            }

            // if there isn't a diffuse texture, set the error texture.
            Bootstrap.MaterialPass pass = _material.GetPass( 0 );
            string diffuseTex = "";
            if ( pass.DiffuseStage != null )
                diffuseTex = pass.DiffuseStage.Texture;
            if ( diffuseTex == null || diffuseTex == "" )
            {
                this.pbPreview.Image = errorImage;
            }
            else
            {
                // try to create the thumbnail.
                Bootstrap.Image image = new Bootstrap.Image( diffuseTex );
                Bitmap preview = image.ToBitmap( this.pbPreview.Size.Width, this.pbPreview.Size.Height );
                if ( preview != null )
                    this.pbPreview.Image = preview;
                else
                    this.pbPreview.Image = errorImage;
            }

            // setup material states.
            this.cbStipple.Checked = _material.Stipple;

            // populate pass controls.
            for ( int i = 0; i < passCount; ++i )
            {
                Bootstrap.MaterialPass curPass = _material.GetPass( i );

                // create the material pass control group.
                Controls.MaterialPassControlGroup materialPassControlGroup = new Controls.MaterialPassControlGroup();
                materialPassControlGroup.SetPass( curPass );

                // create the pass item and add it to the group bar.
                GroupBarItem passItem = new GroupBarItem();
                gbPasses.GroupBarItems.Add( passItem );

                // configure the current group bar item.
                passItem.BackColor = System.Drawing.Color.FromArgb( ( (int)( ( (byte)( 73 ) ) ) ), ( (int)( ( (byte)( 73 ) ) ) ), ( (int)( ( (byte)( 73 ) ) ) ) );
                passItem.Client = materialPassControlGroup;
                passItem.Text = "Pass " + ( i + 1 ).ToString();
            }

            // resize the gbPasses
            this.pnGroupBarPanel.AutoScroll = true;
            this.gbPasses.Size = new Size( this.gbPasses.Size.Width, 1100 );

            _populating = false;
        }

        private void cbStipple_CheckStateChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;
            
            _material.Stipple = cbStipple.Checked;
        }
    }
}
