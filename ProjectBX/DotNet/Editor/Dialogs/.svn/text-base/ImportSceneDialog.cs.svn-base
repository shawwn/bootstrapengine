using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Syncfusion.Windows.Forms.Tools;

namespace Editor
{
	public partial class ImportSceneDialog : RibbonForm
	{
		public ImportSceneDialog()
		{
			InitializeComponent();
			this.ControlBox = false;

			if( _OpenFile == null )
				_OpenFile = new OpenFileDialog();
			_OpenFile.Filter = "Collada File (*.dae)|*.dae|All files (*.*)|*.*";
			//_OpenFile.RestoreDirectory = true;
			_OpenFile.CheckFileExists = true;
			lblError.Visible = false;
		}

		~ImportSceneDialog()
		{
			_OpenFile.Dispose();
		}

		static OpenFileDialog _OpenFile;

		public new DialogResult ShowDialog()
		{
			DialogResult openResult = _OpenFile.ShowDialog();
			if( openResult != DialogResult.OK )
			{
				this.Hide();
				return openResult;
			}

			SceneFileName = _OpenFile.FileName;
			txtSceneName.Focus();

			return base.ShowDialog();
		}

		public string InitialDirectory
		{
			get { return _OpenFile.InitialDirectory; }
			set { _OpenFile.InitialDirectory = value; }
		}

		public string SceneName
		{
			get { return txtSceneName.Text; }
			set { txtSceneName.Text = value; }
		}

		public string SceneFileName
		{
			get { return btnBrowse.Text; }
			set { btnBrowse.Text = value; }
		}

		public bool OverwriteLights
		{
			get { return cbOverwriteLights.Checked; }
			set { cbOverwriteLights.Checked = value; }
		}

		public bool OverwriteMaterials
		{
			get { return cbOverwriteMaterials.Checked; }
			set { cbOverwriteMaterials.Checked = value; }
		}

		void WhenInvalid( string errorMessage )
		{
			txtSceneName.Focus();
			txtSceneName.SelectAll();
			lblError.Text = errorMessage;
			lblError.Visible = true;
		}

		bool ValidateSceneName()
		{
			if( SceneName.Length == 0 )
			{
				WhenInvalid( "Input a scene name." );
				return false;
			}

			foreach( char c in SceneName )
			{
				if( !Util.ValidateFileCharacter( c ) )
				{
					WhenInvalid( "Use A-Z, 0-9, dashes, and underscores." );
					return false;
				}
			}

			lblError.Visible = false;

			return true;
		}

		protected override bool ProcessDialogKey( Keys key )
		{
			switch( key )
			{
				case Keys.Enter:  
					btnAccept.PerformClick(); 
					return true; 

				case Keys.Escape: 
					btnCancel.PerformClick(); 
					return true;
			}
		 	 return base.ProcessDialogKey( key );
		}

		private void ImportSceneDialog_PreviewKeyDown( object sender, PreviewKeyDownEventArgs e )
		{
			switch( e.KeyCode )
			{
				case Keys.Return:
				case Keys.Escape:
					e.IsInputKey = true;
					break;
			}
		}

		private void btnBrowse_Click( object sender, EventArgs e )
		{
			if( _OpenFile.ShowDialog() == DialogResult.OK )
				SceneFileName = _OpenFile.FileName;
		}
		
		private void btnAccept_Click( object sender, EventArgs e )
		{
			if( ValidateSceneName() )
			{
                this.DialogResult = DialogResult.OK;
                this.Close();
            }
		}

		private void btnCancel_Click( object sender, EventArgs e )
		{
			this.DialogResult = DialogResult.Cancel;
			this.Close();
		}
	}
}