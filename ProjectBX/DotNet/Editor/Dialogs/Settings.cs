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
    public partial class Settings : Office2007Form
    {
        public Settings( string title )
        {
            InitializeComponent();

            // set the caption.
            this.Text = title;

            // set the text of the buttons.
            Bootstrap.Settings settings = Bootstrap.Settings.Get();
            bnWorkingFolder.Text = settings.GetWorkingFolder();
            bnBrushLibrary.Text = settings.GetBrushFolder();

			// attach some events.
			bnBrushLibrary.Click += new EventHandler( bn_Click );
			bnWorkingFolder.Click += new EventHandler( bn_Click );
			this.FormClosing += new FormClosingEventHandler( Settings_FormClosing );
        }

		public string WorkingFolder
		{
			get { return bnWorkingFolder.Text; }
		}

		void bn_Click( object sender, EventArgs e )
		{
			// display the folder selector.
            FolderBrowserDialog browser = new FolderBrowserDialog();
			browser.RootFolder = Environment.SpecialFolder.MyComputer;
			if( sender == bnWorkingFolder ) { browser.SelectedPath = bnWorkingFolder.Text; }
			else if( sender == bnBrushLibrary ) { browser.SelectedPath = bnBrushLibrary.Text; }
            DialogResult result = browser.ShowDialog();
            if (result != DialogResult.OK) { return; }

			// set the button text.
			if( sender == bnWorkingFolder ) { bnWorkingFolder.Text = browser.SelectedPath; }
			else if( sender == bnBrushLibrary ) { bnBrushLibrary.Text = browser.SelectedPath; }

			// make the settings dialog a little easier to work with...
			if( bnBrushLibrary.Text == "" ) { bnBrushLibrary.Text = bnWorkingFolder.Text + "\\data\\editor\\brushes"; }
		}

		void Settings_FormClosing( object sender, FormClosingEventArgs e )
		{
			// if the user didn't click OK, do nothing.
			if( this.DialogResult != DialogResult.OK ) { return; }

			// ensure that the brush folder has been created.
			if( bnBrushLibrary.Text != "" )
			{
				if( !System.IO.Directory.Exists( bnBrushLibrary.Text ) )
				{
					System.IO.Directory.CreateDirectory( bnBrushLibrary.Text );
				}
			}

			// save each path.
            Bootstrap.Settings settings = Bootstrap.Settings.Get();
			settings.SetWorkingFolder( bnWorkingFolder.Text );
			settings.SetBrushFolder( bnBrushLibrary.Text );
			settings.Save();

			// ensure that "editor/textures" has been created.
			EnsureEditorTexturesFolderCreated();

		}

		public static void EnsureEditorTexturesFolderCreated()
		{
			string edTexturesFolder = Bootstrap.Settings.Get().GetWorkingFolder() + "\\data\\editor\\textures";
			if( !System.IO.Directory.Exists( edTexturesFolder ) )
				System.IO.Directory.CreateDirectory( edTexturesFolder );
		}
    }
}