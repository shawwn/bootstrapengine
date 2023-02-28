using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using Editor.Dialogs;
using Bootstrap;

namespace Editor.Editors
{
    public partial class LightEditor : UserControl
    {
        Light _light;
        HDRColorDialog _colorDlg;
        TextureSelectionDialog _textureSelector;
        bool _colorDlgVisible;
        bool _populating;

        //================================
        // ctor & ftor.
        //================================

        //--------------------------------
        public LightEditor()
        {
            InitializeComponent();
            this.SetStyle( ControlStyles.Selectable, true );

            _colorDlgVisible = false;
            _colorDlg = new HDRColorDialog();
            _colorDlg.ColorChanged += new EventHandler( colorDialog_ColorChanged );
            _colorDlg.FormClosing += new FormClosingEventHandler( colorDialog_VisibleChanged );
            _colorDlg.HideOnClose = true;
            _colorDlg.Hide();

            _textureSelector = new TextureSelectionDialog( false );
            tcTransform.ScaleEnable = false;
        }


        //================================
        // public interface.
        //================================

        //--------------------------------
        public Bootstrap.Light Light
        {
            get { return _light; }
            set
            {
                // simply ignore any "clearing" of the light.
                if ( value == null )
                    return;

                // don't do unnecessary work.
                if ( value == _light )
                    return;

                // set the light's value and update the controls.
                _light = value;
                UpdateControls();
            }
        }


        //================================
        // private methods.
        //================================

        //--------------------------------
        private void UpdateControls()
        {
            // protect against unintentional changes to settings.
            _populating = true;

            tcTransform.SetSceneEntity( _light );
            tbID.Text = _light.ID;
            tbName.Text = _light.Name;
            cbCastShadow.Checked = _light.CastShadow;
            cbDirectional.Checked = _light.Parallel;
            Util.Drawing.FillPictureBox( pbLightColor, _light.Color );
            lscIntensity.Value = _light.Intensity;
            lscRange.Value = _light.Range;
            lscSoftness.Value = _light.Softness;
            lscFOV.Value = _light.ProjSpreadAngle;
            lscProjWidth.Value = _light.ProjWidth;
            lscProjHeight.Value = _light.ProjHeight;
            lscConstAtten.Value = _light.AttenConst;
            lscLinearAtten.Value = _light.AttenLinear;
            lscQuadraticAtten.Value = _light.AttenQuadratic;

            _populating = false;
        }

        //--------------------------------
        private void btnColor_Click( object sender, EventArgs e )
        {
            _colorDlg.Color = _light.Color;
            _colorDlg.Show();
            _colorDlgVisible = true;
        }

        //--------------------------------
        void colorDialog_ColorChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.Color = ( sender as HDRColorDialog ).Color;
            Util.Drawing.FillPictureBox( pbLightColor, _light.Color );
        }

        //--------------------------------
        void colorDialog_VisibleChanged( object sender, FormClosingEventArgs e )
        {
            _colorDlgVisible = false;
        }

        //--------------------------------
        private void LightEditorDialog_VisibleChanged( object sender, EventArgs e )
        {
            // make sure the color dialog is in the correct state.
            if ( _colorDlgVisible )
            {
                if ( !this.Visible )
                    _colorDlg.Hide();
                else if ( this.Visible )
                    _colorDlg.Show();
            }
        }

        //--------------------------------
        private void LightEditorDialog_FormClosing( object sender, FormClosingEventArgs e )
        {
            this.Hide();
            e.Cancel = true;
        }

        //--------------------------------
        private void cbCastShadow_CheckedChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.CastShadow = cbCastShadow.Checked;
        }

        //--------------------------------
        private void cbDirectional_CheckedChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.Parallel = cbDirectional.Checked;
        }

        //--------------------------------
        private void lscIntensity_ValueChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.Intensity = lscIntensity.Value;
        }

        //--------------------------------
        private void lscRange_ValueChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.Range = lscRange.Value;
        }

        //--------------------------------
        private void btnMask_Click( object sender, EventArgs e )
        {
            // show the texture browser.
            DialogResult result = _textureSelector.ShowDialog();
            if ( result == DialogResult.Cancel )
                return;

            List<string> selection = _textureSelector.Selection;
            if ( result != DialogResult.No )
            {
                // if no selection was made but "None" was not the result, 
                // don't do anything.
                if ( selection.Count == 0 )
                    return;
                _light.Mask = selection[ 0 ];
            }
            else
            {
                _light.Mask = null;
            }
        }

        //--------------------------------
        private void lscFOV_ValueChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.ProjSpreadAngle = lscFOV.Value;
        }

        //--------------------------------
        private void tcTransform_TranslateChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.Position = tcTransform.Position;
        }

        //--------------------------------
        private void tcTransform_RotateChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.Rotation = tcTransform.Rotation;
        }

        //--------------------------------
        private void tcTransform_ScaleChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.Scale = tcTransform.ScaleXYZ;
        }

        //--------------------------------
        private void tbName_TextChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.Name = tbName.Text;
        }

        //--------------------------------
        private void lscSoftness_ValueChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.Softness = lscSoftness.Value;
        }

        //--------------------------------
        private void lscProjWidth_ValueChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.ProjWidth = lscProjWidth.Value;
        }

        //--------------------------------
        private void lscProjHeight_ValueChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.ProjHeight = lscProjHeight.Value;
        }

        //--------------------------------
        private void lscConstAtten_ValueChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.AttenConst = lscConstAtten.Value;
        }

        //--------------------------------
        private void lscLinearAtten_ValueChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.AttenLinear = lscLinearAtten.Value;
        }

        //--------------------------------
        private void lscQuadraticAtten_ValueChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.AttenQuadratic = lscQuadraticAtten.Value;
        }

        //--------------------------------
        private void cbGlobal_CheckStateChanged( object sender, EventArgs e )
        {
            if ( _populating )
                return;

            _light.Global = cbGlobal.Checked;
        }
    }
}
