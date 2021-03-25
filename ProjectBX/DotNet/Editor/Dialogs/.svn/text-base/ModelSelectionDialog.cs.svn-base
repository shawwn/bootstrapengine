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
	public partial class ModelSelectionDialog : Office2007Form
	{
		//=======================
		// constructor & finalizer
		//=======================

		//-----------------------
		public ModelSelectionDialog()
		{
			InitializeComponent();
			this.VisibleChanged += new EventHandler( OnShow );

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
				string path = "data/" + this.cbPackage.Text + "/models/";
				foreach( Controls.Thumbnail thumbnail in thumbnailSelector.Selection )
					selection.Add( path + thumbnail.Text );
				return selection;
			}
		}

		//-----------------------
		public void Add( string name, Bitmap image )
		{
			thumbnailSelector.Add( name, image );
		}

		//-----------------------
		public void Remove( string name )
		{
			thumbnailSelector.Remove( name );
		}

		//-----------------------
		public void Clear()
		{
			thumbnailSelector.Clear();
		}

		//-----------------------
		protected override bool ProcessDialogKey( Keys keyData )
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
		private void cbPackage_SelectedIndexChanged( object sender, EventArgs e )
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
			string packageFolder = Bootstrap.Settings.Get().GetWorkingFolder() + "\\data\\" + cbPackage.SelectedItem;
			string packageTexFolder = packageFolder + "\\models";
			if( Directory.Exists( packageTexFolder ) )
			{
				DirectoryInfo texFolder = new DirectoryInfo( packageTexFolder );
				FileInfo[] modelFiles = texFolder.GetFiles( tbFilter.Text );
				foreach( FileInfo file in modelFiles )
				{
					if( file.Extension != ".mdl" )
						continue;

					// get the thumbnail from the thumbnail manager.
					Bitmap image = Program.ThumbnailMgr.GetThumbnail( file.FullName );
					if( image == null )
						image = Program.ThumbnailMgr.GetErrorThumbnail();

					thumbnailSelector.Add( file.Name, image );
				}
			}
		}

		//-----------------------
		private void tbFilter_KeyDown( object sender, KeyEventArgs e )
		{
			if( e.KeyCode == Keys.Enter )
				UpdateThumbnails();
		}

		//-----------------------
		private void OnShow( object sender, EventArgs e )
		{
			// don't do anything if we're not being made visible.
			if( !this.Visible )
				return;

			// update all of the thumbnails.
			UpdateThumbnails();
		}

		//-----------------------
		private void thumbnailSelector_SelectionMade( object sender, EventArgs e )
		{
			// set the dialog result and close the form.
			this.DialogResult = DialogResult.OK;
			this.Close();
		}

		private void thumbnailSelector_SelectionChanged( object sender, EventArgs e )
		{

		}
	}
}
