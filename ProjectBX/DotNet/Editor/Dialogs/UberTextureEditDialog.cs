using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Syncfusion.Windows.Forms;

namespace Editor.Dialogs
{
    public partial class UberTextureEditDialog : Office2007Form
    {
        Bootstrap.UberTexture _uberTexture;

        //================================
        // ctor & ftor.
        //================================

        //--------------------------------
        public UberTextureEditDialog()
        {
            InitializeComponent();
        }

        //================================
        // public interface.
        //================================

        //--------------------------------
        public Bootstrap.UberTexture UberTexture
        {
            get { return _uberTexture; }
            set { _uberTexture = value; }
        }

        //--------------------------------
        public void Render()
        {
            if (pnViewport.Visible)
            {
//              Program.GameEngine.DrawUberTex.Render(_uberTexture as Bootstrap.UberTexture, 0.0f, 0.0f, 1.0f, 1.0f);
            }
        }

        //--------------------------------
        protected override bool ProcessDialogKey(Keys keyData)
        {
            // handle the escape key.
            if (keyData == Keys.Escape)
            {
                this.DialogResult = DialogResult.OK;
                this.Close();
                return true;
            }

            // filter the message through to the default handler.
            return base.ProcessDialogKey(keyData);
        }

        //--------------------------------
        private void pnViewport_VisibleChanged(object sender, EventArgs e)
        {
//          if (pnViewport.Visible)
//              Program.GameEngine.DrawUberTex.Target = pnViewport;
//          else
//              Program.GameEngine.DrawUberTex.Target = null;
        }

        //--------------------------------
        private void UberTextureEditDialog_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.Hide();
            e.Cancel = true;
        }
    }
}
