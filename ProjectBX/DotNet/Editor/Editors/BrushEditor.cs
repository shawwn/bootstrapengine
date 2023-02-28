using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using Syncfusion.Windows.Forms;
using Editor.Dialogs;

namespace Editor.Editors
{
    public partial class BrushEditor : UserControl
    {
        static TextureSelectionDialog _textureSelector;
        Bootstrap.Brush _brush;


        public BrushEditor()
        {
            InitializeComponent();

            if( _textureSelector == null )
            {
                _textureSelector = new TextureSelectionDialog( false );
                _textureSelector.cbPackage.DefaultSelectedPackage = "editor";
            }

            // disable each stage texture selector.
            foreach( Control control in pnControls.Controls ) 
            {
                if( control is Button )
                    control.Enabled = false;
            }
        }

        public event EventHandler BrushEdited;

        [Browsable(false)]
        public Bootstrap.Brush Brush
        {
            get { return _brush; }
            set { _brush = value; UpdateControls(); }
        }

        public bool IsTextureSelectorVisible
        {
            get { return _textureSelector.Visible; }
        }

        protected override void OnHandleCreated( EventArgs e )
        {
            base.OnHandleCreated( e );
            if( _initialized )
                return;
            _initialized = true;

            // for the brush editor, show the "editor" package.
            _textureSelector.cbPackage.AddPackage( "editor" );
        }
        bool _initialized;

        void UpdateControls()
        {
            // enable or disable the stage buttons.
            foreach( Control control in pnControls.Controls ) 
            {
                if( control is Button )
                    control.Enabled = (_brush != null);
            }

            if( _brush == null ) 
                return;

            lbBrushName.Text = _brush.Name;
            UpdateButtonText(bnDiffuse, _brush.GetImageName(Bootstrap.Brush.Stage.Diffuse));
            UpdateButtonText(bnSpecular, _brush.GetImageName(Bootstrap.Brush.Stage.Specular));
            UpdateButtonText(bnNormal, _brush.GetImageName(Bootstrap.Brush.Stage.Normal));
            UpdateButtonText(bnBump, _brush.GetImageName(Bootstrap.Brush.Stage.Bump));
            UpdateButtonText(bnEmissive, _brush.GetImageName(Bootstrap.Brush.Stage.Emissive));
            UpdateButtonText(bnAmbient, _brush.GetImageName(Bootstrap.Brush.Stage.Ambient));
            UpdateButtonText(bnUser, _brush.GetImageName(Bootstrap.Brush.Stage.User));
            UpdateButtonText(bnMask, _brush.GetMaskName());
        }

        void UpdateButtonText(ButtonAdv button, string imageName)
        {
            // update the button's text if possible.  Otherwise set it to {not set}.
            if (imageName != null && imageName != "")
                button.Text = imageName;
            else
                button.Text = "{not set}";
        }

        void bnTexture_Click(object sender, EventArgs e)
        {
            // try to cast the sending object to a Syncfusion button.
            ButtonAdv button = sender as ButtonAdv;
            if (button == null)
                return;

            // get the current stage.
            bool selectingMask = false;
            Bootstrap.Brush.Stage stage = Bootstrap.Brush.Stage.User;
            string stageName = "";
            if (button == bnDiffuse)
            {
                stage = Bootstrap.Brush.Stage.Diffuse;
                stageName = "Diffuse";
            }
            else if (button == bnSpecular)
            {
                stage = Bootstrap.Brush.Stage.Specular;
                stageName = "Specular";
            }
            else if (button == bnNormal)
            {
                stage = Bootstrap.Brush.Stage.Normal;
                stageName = "Normal";
            }
            else if (button == bnBump)
            {
                stage = Bootstrap.Brush.Stage.Bump;
                stageName = "Bump";
            }
            else if (button == bnEmissive)
            {
                stage = Bootstrap.Brush.Stage.Emissive;
                stageName = "Emissive";
            }
            else if (button == bnAmbient)
            {
                stage = Bootstrap.Brush.Stage.Ambient;
                stageName = "Ambient";
            }
            else if (sender == bnUser)
            {
                stage = Bootstrap.Brush.Stage.User;
                stageName = "User";
            }
            else if (sender == bnMask)
            {
                selectingMask = true;
                stageName = "Mask";
            }

            // now select an image using the texture selector.
            _textureSelector.Text = "Select " + stageName + " Texture";
            DialogResult result = _textureSelector.ShowDialog();
            if (result == DialogResult.Cancel )
                return;

            // determine what to do... grab the result from the dialog or
            // clear the result.
            string imageName = "";
            if( result != DialogResult.No )
            {
                // if no selection was made but "None" was not the result, 
                // don't do anything.
                if (_textureSelector.Selection.Count == 0)
                    return;
                imageName = _textureSelector.Selection[0];
            }

            // set filler text if nothing was selected.
            if (imageName == "")
                button.Text = "{not set}";
            else
                button.Text = imageName;

            // now apply the image to the brush.
            if( selectingMask )
                _brush.SetMask( imageName );
            else
                _brush.SetImage( stage, imageName );

            // then persist the change.
            Bootstrap.Brush.SaveBrushes();

            // notify any observers of the change.
            if( BrushEdited != null )
                BrushEdited( this, new EventArgs() );
        }
    }
}
