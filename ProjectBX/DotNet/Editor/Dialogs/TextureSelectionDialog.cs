using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using Syncfusion.Windows.Forms;

namespace Editor.Dialogs
{
    public partial class TextureSelectionDialog : Office2007Form
    {
        // flag to indicate display of ubertextures instead of
        // regular textures.
        bool _uberTextures;

        //=======================
        // constructor & finalizer
        //=======================

        //-----------------------
        public TextureSelectionDialog(bool uberTextures)
        {
            InitializeComponent();
            this.VisibleChanged += new EventHandler(OnShow);
            _uberTextures = uberTextures;

            // the create button should only be visible if we're the
            // ubertexture dialog.
            bnCreate.Visible = _uberTextures;

            // focus the thumbnail selector.
            thumbnailSelector.Focus();
        }


        //=======================
        // public methods
        //=======================

        //-----------------------
        public bool ShowClearSelection
        {
            get { return bnNone.Visible; }
            set { bnNone.Visible = value; }
        }

        //-----------------------
        public bool ShowCancel
        {
            get { return bnCancel.Visible; }
            set { bnCancel.Visible = value; }
        }

        //-----------------------
        public List<string> Selection
        {
            get
            {
                List<string> selection = new List<string>();
                if (_uberTextures)
                {
                    string path = "data/" + this.cbPackage.Text + "/uber/";
                    foreach (Controls.Thumbnail thumbnail in thumbnailSelector.Selection)
                        selection.Add(path + thumbnail.Text);
                }
                else
                {
                    string path = "data/" + this.cbPackage.Text + "/textures/";
                    foreach (Controls.Thumbnail thumbnail in thumbnailSelector.Selection)
                        selection.Add(path + thumbnail.Text);
                }
                return selection;
            }
        }

        //-----------------------
        public void Add(string name, Bitmap image)
        {
            thumbnailSelector.Add(name, image);
        }

        //-----------------------
        public void Remove(string name)
        {
            thumbnailSelector.Remove(name);
        }

        //-----------------------
        public void Clear()
        {
            thumbnailSelector.Clear();
        }

        //-----------------------
        protected override bool ProcessDialogKey(Keys keyData)
        {
            switch( keyData )
            {
                case Keys.Enter:
                    if( !tbFilter.Focused )
                    {
                        this.DialogResult = DialogResult.OK;
                        this.Close();
                        return true;
                    }
                    break;

                case Keys.Escape:
                    {
                        this.DialogResult = DialogResult.Cancel;
                        this.Close();
                    }
                    return true;
            }
            return base.ProcessDialogKey( keyData );
        }

        //-----------------------
        private void cbPackage_SelectedIndexChanged(object sender, EventArgs e)
        {
            // determine what package was selected.
            UpdateThumbnails();
        }

        //-----------------------
        private void UpdateThumbnails()
        {
            // clear thumbnails.
            thumbnailSelector.Clear();

            // iterate through files in the folder.
            if( !_uberTextures )
            {
                string packageFolder = Bootstrap.Settings.Get().GetWorkingFolder() + "\\data\\" + cbPackage.SelectedItem;
                string packageTexFolder = packageFolder + "\\textures";
                if (Directory.Exists(packageTexFolder))
                {
                    DirectoryInfo texFolder = new DirectoryInfo(packageTexFolder);
                    FileInfo[] imageFiles = texFolder.GetFiles(tbFilter.Text);
                    foreach (FileInfo file in imageFiles)
                    {
                        // ignore files that are not .tga or .dds files.
                        if( file.Extension != ".tga" && file.Extension != ".dds" )
                            continue;

                        // get the thumbnail from the thumbnail manager.
                        Bitmap image = Program.ThumbnailMgr.GetThumbnail(file.FullName);
                        if (image != null)
                            thumbnailSelector.Add(file.Name, image);
                    }
                }
            }
            else
            {
                // get the list of ubertextures available to the current scene.
                string relativePath = "\\data\\" + cbPackage.SelectedItem + "\\uber\\";
                List<string> textureNames = Bootstrap.UberTexture.GetUberTextureList(relativePath);
                foreach (string textureName in textureNames)
                {
                    // get the ubertexture and generate a thumbnail.
                    string texturePath = relativePath + textureName;
                    using (Bootstrap.UberTexture uberTexture = new Bootstrap.UberTexture(texturePath))
                    {
                        // generate a thumbnail and add it to the thumbnail selector.
                        Bitmap thumbnail = uberTexture.MakeThumbnail();
                        if (thumbnail != null)
                            thumbnailSelector.Add(textureName, thumbnail);
                    }
                }
            }
        }

        //-----------------------
        private void tbFilter_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
                UpdateThumbnails();
        }

        //-----------------------
        private void OnShow(object sender, EventArgs e)
        {
            // don't do anything if we're not being made visible.
            if (!this.Visible)
                return;

            // update all of the thumbnails.
            UpdateThumbnails();
        }

        //-----------------------
        private void thumbnailSelector_SelectionMade(object sender, EventArgs e)
        {
            // set the dialog result and close the form.
            this.DialogResult = DialogResult.OK;
            this.Close();
        }

        private void bnCreate_Click(object sender, EventArgs e)
        {
            // show the ubertexture creation dialog.
            UberTextureCreationDialog dialog = new UberTextureCreationDialog();
            DialogResult result = dialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                // ubertexture creation.
                string relativePath = "\\data\\" + cbPackage.SelectedItem + "\\uber\\" + dialog.tbName.Text + ".utx";
                int width = Convert.ToInt32(dialog.cbWidth.Text);
                int height = Convert.ToInt32(dialog.cbHeight.Text);
                using (Bootstrap.UberTexture texture = new Bootstrap.UberTexture(relativePath, width, height))
                {
                    UpdateThumbnails();
                }
            }
        }
    }
}