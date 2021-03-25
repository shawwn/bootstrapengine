using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace Editor.Dialogs
{
	public partial class BrushPaletteSelectionDialog : BaseDialog
	{
		public BrushPaletteSelectionDialog()
		{
			InitializeComponent();

			// hook some events.
			thumbnailSelector.SelectionChanged += new EventHandler( thumbnailSelector_SelectionChanged );
			thumbnailSelector.SelectionMade += new EventHandler( thumbnailSelector_SelectionMade );

			// focus the thumbnail selector.
			thumbnailSelector.Focus();
		}


        //=======================
        // public attributes
        //=======================

        public Bootstrap.BrushPalette Selection
        {
            get { return _selection; }
        }
		Bootstrap.BrushPalette _selection;

		public Bootstrap.BrushPalette NewPalette()
		{
			// reload all palettes.
			ReloadScripts();

			// ask the user for the name of a palette.
			string name = "";
			using( TextEntryDialog grabName = new TextEntryDialog() )
			{
				grabName.Title = "Create Palette";
				grabName.Caption = "Name";
				DialogResult result = grabName.ShowDialog();
				if( result != DialogResult.OK || grabName.TextResult == "" )
					return null;

				name = grabName.TextResult;
			}

			// create the palette.
			Bootstrap.BrushPalette palette = Bootstrap.BrushPalette.Create( name );

			// save the palette.
			Bootstrap.BrushPalette.SaveBrushPalettes();

			// repopulate the thumbnails.
			PopulateThumbnails();

			// select the palette.
			thumbnailSelector.SelectedName = palette.Name;

			// return the palette.
			return palette;
		}

        //=======================
        // private functionality
        //=======================

        private void ReloadScripts()
        {
            // get info about the brush folder.
            DirectoryInfo dir = new DirectoryInfo(Bootstrap.Settings.Get().GetBrushFolder());

			// load the brush palettes.
			string errors = "";
            Bootstrap.Engine.Get().LoadBrushPaletteScript( dir.FullName, errors );
			if( errors != "" )
				MessageBox.Show( "There were errors while loading brush palettes: " + errors );

            // add all thumbnails.
            PopulateThumbnails();
        }

		void PopulateThumbnails()
		{
            // remove existing brush palettes.
            thumbnailSelector.Clear();

            // get all brush palettes from the loaded scripts.
            string[] paletteNames = Bootstrap.BrushPalette.GetBrushPaletteNameList();

            // iterate through each brush palette and add a thumbnail.
            foreach( string name in paletteNames )
            {
                // create a manipulator for the current brush palette.
                using( Bootstrap.BrushPalette palette = new Bootstrap.BrushPalette(name) )
				{
	                // add the thumbnail to the selection dialog.
	                thumbnailSelector.Add( palette.Name, Program.ThumbnailMgr.GetThumbnail( palette ), palette.Name );
				}
            }
		}


        //=======================
        // events
        //=======================

		protected override void OnVisibleChanged( EventArgs e )
		{
			base.OnVisibleChanged( e );

			if( this.Visible )
			{
				// if there isn't anything in the thumbnail selector, try to
				// populate it.
				if( thumbnailSelector.ThumbnailCount == 0 )
					ReloadScripts();
			}
		}

		void thumbnailSelector_SelectionChanged( object sender, EventArgs e )
		{
            Controls.Thumbnail selected = thumbnailSelector.Selected;
            if( selected != null )
			{
                _selection = new Bootstrap.BrushPalette( selected.UserData );
			}
			else
			{
				_selection = null;
			}
		}

		void thumbnailSelector_SelectionMade( object sender, EventArgs e )
		{
            this.DialogResult = DialogResult.OK;
            this.Close();
		}

		private void bnNew_Click( object sender, EventArgs e )
		{
			NewPalette();
		}

		private void btnReload_Click( object sender, EventArgs e )
		{
			ReloadScripts();
		}
	}
}