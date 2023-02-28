using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Bootstrap;

namespace Editor.Dialogs
{
    public partial class DetailCastDialog : BaseDialog
    {
        [Flags]
        public enum CastOptions
        {
            CastBump,
            CastNormal,
            SmoothSource,
        };

        string _objFilePath;
        float _maxRayDist;
        float _bumpScale;
        CastOptions _options;

        //================================
        // ctor & ftor.
        //================================

        //--------------------------------
        public DetailCastDialog()
        {
            InitializeComponent();

            // set ray parameters to valid defaults.
            _maxRayDist = 10.0f;
            _bumpScale = 1.0f;

            // set the initial options/flags.
            _options = CastOptions.CastBump | CastOptions.CastNormal | CastOptions.SmoothSource;
        }


        //================================
        // public methods.
        //================================

        //--------------------------------
        public float MaxRayLength { get { return _maxRayDist; } }
        public float BumpScale { get { return _bumpScale; } }
        public CastOptions Options { get { return _options; } }
        public string ObjFilePath { get { return _objFilePath; } }

        //--------------------------------
        private void bnOK_Click(object sender, EventArgs e)
        {
            // has a valid file been selected?
            if (_objFilePath == "")
                return;
            
            // can we retreive the textbox values?
            try
            {
                _maxRayDist = Convert.ToSingle(tbRayLength.Text);
                _bumpScale = Convert.ToSingle(tbBumpScale.Text);
            }
            catch (SystemException)
            {
                return;
            }

            // success!
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void buttonAdv1_Click(object sender, EventArgs e)
        {
            // show the file dialog.
            OpenFileDialog open = new OpenFileDialog();
            open.Filter = "OBJ file (*.obj)|*.obj|All files (*.*)|*.*";
            open.RestoreDirectory = true;
            open.CheckFileExists = true;
            if (open.ShowDialog() == DialogResult.OK)
            {
                // update the button and store the values.
                bnSelectFile.Text = "(" + open.FileName + ")";
                _objFilePath = open.FileName;
            }
        }

        private void rbCastBump_CheckedChanged(object sender, EventArgs e)
        {
            if (rbCastBump.Checked)
            {
                _options |= CastOptions.CastBump;
                _options &= ~CastOptions.CastNormal;
            }
        }

        private void rbCastNormal_CheckedChanged(object sender, EventArgs e)
        {
            if (rbCastNormal.Checked)
            {
                _options &= ~CastOptions.CastBump;
                _options |= CastOptions.CastNormal;
            }
        }

        private void rbCastBoth_CheckedChanged(object sender, EventArgs e)
        {
            if (rbCastBoth.Checked)
            {
                _options |= CastOptions.CastBump;
                _options |= CastOptions.CastNormal;
            }
        }
    }
}