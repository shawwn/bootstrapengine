using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.IO;
using System.Windows.Forms;
using Syncfusion.Windows.Forms;

namespace Editor.Dialogs
{
    public partial class BrushCreationDialog : BaseDialog
    {
        public BrushCreationDialog()
        {
            InitializeComponent();

			// populate the brush categories.
            DirectoryInfo brushDir = new DirectoryInfo( Bootstrap.Settings.Get().GetBrushFolder() );
			foreach( DirectoryInfo directory in brushDir.GetDirectories() )
			{
				// each folder within the brush folder is a category.
				cbCategory.Items.Add( directory.Name );
			}

			if( !cbCategory.Items.Contains( "default" ) )
			{
				// add the default category.
				cbCategory.Items.Add( "default" );
			}

			// select the default category.
			cbCategory.SelectedItem = "default";

			// highlight and focus on the brush name text box.
			tbName.SelectAll();
        }

        protected override bool ProcessDialogKey(Keys keyData)
        {
            if (keyData == Keys.Enter)
            {
                if (FinishBrush())
                    return base.ProcessDialogKey(keyData);
                else
                    return true;
            }

            return base.ProcessDialogKey(keyData);
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            if (FinishBrush())
            {
                // set the dialog result on the form.
                this.DialogResult = DialogResult.OK;
                this.Close();
            }
        }

        private bool FinishBrush()
        {
            // make sure the category specified is valid.
            string category = cbCategory.Text;
            if (!IsValidName(category))
            {
                MessageBox.Show("The category specified contains invalid characters.  Only letters, numbers, and '_' are supported.",
                    "Invalid category...", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return false;
            }

            // make sure the name specified is valid.
            string name = tbName.Text;
            if (!IsValidName(name))
            {
                MessageBox.Show("The name specified contains invalid characters.  Only letters, numbers, and '_' are supported.",
                    "Invalid name...", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return false;
            }

            // ensure that a folder exists where for the category.  If it
            // doesn't, create it now.
            string path = Bootstrap.Settings.Get().GetBrushFolder() + "\\" + category;
            DirectoryInfo dir = new DirectoryInfo(path);

            // try to create the directory if it doesn't exist.
            try
            {
                if (!dir.Exists)
                    dir.Create();
            }
            catch (SystemException)
            {
                MessageBox.Show("An error occured while trying to create the folder for the category specified.  This could be due" +
                    " to an invalid folder name or a hardware problem.  Please try a different name and if this fails again, please" +
                    " contact your IT supervisor.", "Error creating folder...", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                return false;
            }

            // return true to indicate success.
            return true;
        }

        private bool IsValidName(string name)
        {
            // verify that the string is valid.
            if (name.Length == 0)
                return false;

            // the first character must be a letter.
            if (!Char.IsLetter(name[0]))
                return false;

            // check all characters to make sure they're a letter or
            // an underscore.
            foreach (char c in name)
                if (!Char.IsLetter(c) && !Char.IsNumber(c) && c != '_')
                    return false;

            // return true to indicate success.
            return true;
        }
    }
}