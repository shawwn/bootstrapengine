using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.IO;
using System.Windows.Forms;
using System.Drawing.Imaging;
using Syncfusion.Windows.Forms;
using Syncfusion.Windows.Forms.Tools;

namespace Editor.Controls
{
    public partial class MaterialPassControlGroup : UserControl
    {
        Bootstrap.MaterialPass _materialPass;
        bool _populating;

        public MaterialPassControlGroup()
        {
            InitializeComponent();

            // workaround for a designer bug.
            this.label1.Font = new System.Drawing.Font( "Microsoft Sans Serif", 8.25F );
            this.label2.Font = new System.Drawing.Font( "Microsoft Sans Serif", 8.25F );
            this.cbLightShader.Font = new System.Drawing.Font( "Microsoft Sans Serif", 8.25F );
            this.cbAmbientShader.Font = new System.Drawing.Font( "Microsoft Sans Serif", 8.25F );
            this.btnTranslateU.Font = new System.Drawing.Font( "Microsoft Sans Serif", 8.25F );
            this.btnTranslateV.Font = new System.Drawing.Font( "Microsoft Sans Serif", 8.25F );
            this.btnScaleU.Font = new System.Drawing.Font( "Microsoft Sans Serif", 8.25F );
            this.btnScaleV.Font = new System.Drawing.Font( "Microsoft Sans Serif", 8.25F );
            this.btnRotate.Font = new System.Drawing.Font( "Microsoft Sans Serif", 8.25F );
            this.btnRotateCenterS.Font = new System.Drawing.Font( "Microsoft Sans Serif", 8.25F );
            this.btnRotateCenterT.Font = new System.Drawing.Font( "Microsoft Sans Serif", 8.25F );
        }

        public void SetPass( Bootstrap.MaterialPass pass )
        {
            _materialPass = pass;
            gbStages.SetPass( pass );
            UpdateControls();
        }

        public Bootstrap.MaterialPass GetPass()
        {
            return _materialPass;
        }

        private void UpdateControls()
        {
            // protect against unintentional changes to settings.
            _populating = true;

            // update the shader drop-downs.
            UpdateShaderDropDowns();

            // now update other controls.
            cbLightShader.Text = _materialPass.Shader;
            cbAmbientShader.Text = _materialPass.AmbientShader;

            // update the color mod/add values.
            Util.Drawing.FillPictureBox( pbMinLightResponse, _materialPass.MinLightResponse );
            Util.Drawing.FillPictureBox( pbMaxLightResponse, _materialPass.MaxLightResponse );
            Util.Drawing.FillPictureBox( pbVertexColorMod, _materialPass.VertexColorMod );
            Util.Drawing.FillPictureBox( pbVertexColorAdd, _materialPass.VertexColorAdd );

            // update the shadow checkboxes.
            cbCastShadow.Checked = _materialPass.CastShadow;
            cbReceiveShadow.Checked = _materialPass.ReceiveShadow;

            // update the write mask checkboxes.
            Bootstrap.MaterialPass.WriteChannel writeMask = _materialPass.WriteMask;
            cbRedMask.Checked = ( writeMask & Bootstrap.MaterialPass.WriteChannel.Red ) != 0;
            cbGreenMask.Checked = ( writeMask & Bootstrap.MaterialPass.WriteChannel.Green ) != 0;
            cbBlueMask.Checked = ( writeMask & Bootstrap.MaterialPass.WriteChannel.Blue ) != 0;
            cbAlphaMask.Checked = ( writeMask & Bootstrap.MaterialPass.WriteChannel.Alpha ) != 0;
            cbDepthMask.Checked = ( writeMask & Bootstrap.MaterialPass.WriteChannel.Depth ) != 0;
            _materialPass.WriteMask = writeMask;

            if ( _materialPass.Textures.Rotate != null )
                tbRotate.Text = _materialPass.Textures.Rotate.Decompile();
            else
                tbRotate.Text = "0.0";

            UpdateScriptPreview( tbScaleU, _materialPass.Textures.ScaleS, "1.0" );
            UpdateScriptPreview( tbScaleV, _materialPass.Textures.ScaleT, "1.0" );
            UpdateScriptPreview( tbTranslateU, _materialPass.Textures.TranslateS, "0.0" );
            UpdateScriptPreview( tbTranslateV, _materialPass.Textures.TranslateT, "0.0" );
            UpdateScriptPreview( tbRotate, _materialPass.Textures.Rotate, "0.0" );
            UpdateScriptPreview( tbRotateCenterU, _materialPass.Textures.RotateCenterS, "0.0" );
            UpdateScriptPreview( tbRotateCenterV, _materialPass.Textures.RotateCenterT, "0.0" );

            _populating = false;
        }

        private void UpdateScriptPreview( TextBoxExt textBox, Bootstrap.Expr expr, string defValue )
        {
            if ( expr != null && expr.Valid )
                textBox.Text = expr.Decompile();
            else
                textBox.Text = defValue;
        }

        private void UpdateShaderDropDowns()
        {
            // initialize the shaders dropdown.
            List<string> ambientShaders = Bootstrap.Shader.GetAmbientShaderList();
            List<string> lightShaders = Bootstrap.Shader.GetLightShaderList();

            // assign the shaders to their dropdowns.
            cbAmbientShader.Items.Clear();
            foreach ( string shader in ambientShaders )
                cbAmbientShader.Items.Add( shader );

            cbLightShader.Items.Clear();
            foreach ( string shader in lightShaders )
                cbLightShader.Items.Add( shader );
        }

        private void bnMinLightResponse_Click( object sender, EventArgs e )
        {
            // open the color dialog.
            Dialogs.HDRColorDialog colorDialog = new Dialogs.HDRColorDialog();
            colorDialog.Color = _materialPass.MinLightResponse;
            colorDialog.ShowDialog();
            _materialPass.MinLightResponse = colorDialog.Color;
            Util.Drawing.FillPictureBox( pbMinLightResponse, colorDialog.Color );
        }

        private void bnMaxLightResponse_Click( object sender, EventArgs e )
        {
            // open the color dialog.
            Dialogs.HDRColorDialog colorDialog = new Dialogs.HDRColorDialog();
            colorDialog.Color = _materialPass.MaxLightResponse;
            colorDialog.ShowDialog();
            _materialPass.MaxLightResponse = colorDialog.Color;
            Util.Drawing.FillPictureBox( pbMaxLightResponse, colorDialog.Color );
        }

        private void bnVertexColorMod_Click( object sender, EventArgs e )
        {
            // open the color dialog.
            Dialogs.HDRColorDialog colorDialog = new Dialogs.HDRColorDialog();
            colorDialog.Color = _materialPass.VertexColorMod;
            colorDialog.ShowDialog();
            _materialPass.VertexColorMod = colorDialog.Color;
            Util.Drawing.FillPictureBox( pbVertexColorMod, colorDialog.Color );
        }

        private void bnVertexColorAdd_Click( object sender, EventArgs e )
        {
            // open the color dialog.
            Dialogs.HDRColorDialog colorDialog = new Dialogs.HDRColorDialog();
            colorDialog.Color = _materialPass.VertexColorAdd;
            colorDialog.ShowDialog();
            _materialPass.VertexColorAdd = colorDialog.Color;
            Util.Drawing.FillPictureBox( pbVertexColorAdd, colorDialog.Color );
        }

        private void cbCastShadow_CheckStateChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _materialPass.CastShadow = cbCastShadow.Checked;
        }

        private void cbReceiveShadow_CheckStateChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _materialPass.ReceiveShadow = cbReceiveShadow.Checked;
        }

        private void cbWriteMask_CheckStateChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            // update the write mask.
            Bootstrap.MaterialPass.WriteChannel writeMask = 0;
            if ( cbRedMask.Checked )
                writeMask |= Bootstrap.MaterialPass.WriteChannel.Red;
            if ( cbGreenMask.Checked )
                writeMask |= Bootstrap.MaterialPass.WriteChannel.Green;
            if ( cbBlueMask.Checked )
                writeMask |= Bootstrap.MaterialPass.WriteChannel.Blue;
            if ( cbAlphaMask.Checked )
                writeMask |= Bootstrap.MaterialPass.WriteChannel.Alpha;
            if ( cbDepthMask.Checked )
                writeMask |= Bootstrap.MaterialPass.WriteChannel.Depth;

            // set the write mask.
            _materialPass.WriteMask = writeMask;
        }

        private void btnTranslateU_Click( object sender, EventArgs e )
        {
            using ( Dialogs.ScriptEditor editor = new Dialogs.ScriptEditor( _materialPass.Textures.TranslateS ) )
            {
                editor.ShowDialog();
                _materialPass.Textures.TranslateS = editor.Expression;
            }
            UpdateScriptPreview( tbTranslateU, _materialPass.Textures.TranslateS, "0.0" );
        }

        private void btnTranslateV_Click( object sender, EventArgs e )
        {
            using ( Dialogs.ScriptEditor editor = new Dialogs.ScriptEditor( _materialPass.Textures.TranslateT ) )
            {
                editor.ShowDialog();
                _materialPass.Textures.TranslateT = editor.Expression;
            }
            UpdateScriptPreview( tbTranslateV, _materialPass.Textures.TranslateT, "0.0" );
        }

        private void btnScaleU_Click( object sender, EventArgs e )
        {
            using ( Dialogs.ScriptEditor editor = new Dialogs.ScriptEditor( _materialPass.Textures.ScaleS ) )
            {
                editor.ShowDialog();
                _materialPass.Textures.ScaleS = editor.Expression;
            }
            UpdateScriptPreview( tbScaleU, _materialPass.Textures.ScaleS, "1.0" );
        }

        private void btnScaleV_Click( object sender, EventArgs e )
        {
            using ( Dialogs.ScriptEditor editor = new Dialogs.ScriptEditor( _materialPass.Textures.ScaleT ) )
            {
                editor.ShowDialog();
                _materialPass.Textures.ScaleT = editor.Expression;
            }
            UpdateScriptPreview( tbScaleV, _materialPass.Textures.ScaleT, "1.0" );
        }

        private void btnRotate_Click( object sender, EventArgs e )
        {
            using ( Dialogs.ScriptEditor editor = new Dialogs.ScriptEditor( _materialPass.Textures.Rotate ) )
            {
                editor.ShowDialog();
                _materialPass.Textures.Rotate = editor.Expression;
            }
            UpdateScriptPreview( tbRotate, _materialPass.Textures.Rotate, "0.0" );
        }

        private void btnRotateCenterS_Click( object sender, EventArgs e )
        {
            using ( Dialogs.ScriptEditor editor = new Dialogs.ScriptEditor( _materialPass.Textures.RotateCenterS ) )
            {
                editor.ShowDialog();
                _materialPass.Textures.RotateCenterS = editor.Expression;
            }
            UpdateScriptPreview( tbRotateCenterU, _materialPass.Textures.RotateCenterS, "0.0" );
        }

        private void btnRotateCenterT_Click( object sender, EventArgs e )
        {
            using ( Dialogs.ScriptEditor editor = new Dialogs.ScriptEditor( _materialPass.Textures.RotateCenterT ) )
            {
                editor.ShowDialog();
                _materialPass.Textures.RotateCenterT = editor.Expression;
            }
            UpdateScriptPreview( tbRotateCenterV, _materialPass.Textures.RotateCenterT, "0.0" );
        }

        private void cbAmbientShader_SelectedIndexChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _materialPass.Shader = cbAmbientShader.Text;
        }

        private void cbLightShader_SelectedIndexChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _materialPass.Shader = cbLightShader.Text;
        }
    }
}
