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
    public partial class DetailCastProgressDialog : BaseDialog
    {
        DetailCastDialog _castOptions;
        bool _started;

        //================================
        // ctor & ftor.
        //================================

        //--------------------------------
        public DetailCastProgressDialog(DetailCastDialog castOptions)
        {
            InitializeComponent();

            // make sure the user can't click the OK button.
            bnOK.Enabled = false;
            lblStatus.Text = "Initializing...";

            // setup our internal state.
            _castOptions = castOptions;
            _started = false;
        }


        //================================
        // public methods.
        //================================

        //--------------------------------
        public void Cast()
        {
            // if we've already started casting, simply return.
            if (_started)
                return;

            // mark the operation as started and begin running.
            _started = true;

            // initialize the progress callback delegate.
            UberTextureRasterizer.CastProgressCallback progressCallback = new
                UberTextureRasterizer.CastProgressCallback(CastCallback);

            bool castNormal = (_castOptions.Options & DetailCastDialog.CastOptions.CastNormal) != 0;
            bool castBump = (_castOptions.Options & DetailCastDialog.CastOptions.CastBump) != 0;
            bool smoothSource = (_castOptions.Options & DetailCastDialog.CastOptions.SmoothSource) != 0;

            // start the casting operation.
            try
            {
                UberTextureRasterizer rasterizer = new UberTextureRasterizer();
                rasterizer.CastGeometry(progressCallback, _castOptions.ObjFilePath, _castOptions.MaxRayLength,
                      castNormal, castBump, _castOptions.BumpScale, smoothSource);
            }
            catch (System.ApplicationException)
            {
                // done.
                this.Close();
            }

            // update the dialog here to show that we're finished.
            lblStatus.Text = "Done!\n";
            bnOK.Enabled = true;

			GC.KeepAlive( progressCallback );
        }

        
        //================================
        // private methods.
        //================================

        //--------------------------------
        private void CastCallback(int totalTriCount, int trisHandled, int raysCast)
        {
            // modify the status.
            if (totalTriCount == trisHandled)
                lblStatus.Text = "Finalizing...\n";
            else
                lblStatus.Text = "Ray casting...\n";

            // update the UI.
            lblTrisTotal.Text = totalTriCount.ToString();
            lblTrisRemaining.Text = (totalTriCount - trisHandled).ToString();
            lblRaysCast.Text = raysCast.ToString();
            if (totalTriCount > 0)
                pbProgress.Value = (int)(((double)trisHandled / (double)totalTriCount) * 1000.0);
            else
                pbProgress.Value = 1000;

            // make sure we update the windows message pump.
            System.Windows.Forms.Application.DoEvents();
        }

        //--------------------------------
        private void bnOK_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.OK;
            bnOK.Enabled = false;
            this.Close();
        }
    }
}