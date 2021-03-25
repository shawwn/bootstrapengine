using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Editor.Dialogs
{
    public partial class UberTextureCreationDialog : BaseDialog
    {
        public UberTextureCreationDialog()
        {
            InitializeComponent();
        }

        private void bnOK_Click(object sender, EventArgs e)
        {
            // verify that the name is valid.
            string name = tbName.Text;
            if (name.Length == 0)
                return;

            // verify the characters used in the name are valid.
            foreach (char c in name)
            {
                if (!Char.IsLetterOrDigit(c) && c != '_')
                    return;
            }

            // store the dialog result and close the form.
            this.DialogResult = DialogResult.OK;
            this.Close();
		}
    }
}