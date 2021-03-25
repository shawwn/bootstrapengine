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
	public partial class ImportModelDialog : RibbonForm
	{
		public ImportModelDialog()
		{
			InitializeComponent();
			this.ControlBox = false;

			if( _OpenFile == null )
				_OpenFile = new OpenFileDialog();
			_OpenFile.Filter = "Collada File (*.dae)|*.dae|All files (*.*)|*.*";
			//_OpenFile.RestoreDirectory = true;
			_OpenFile.CheckFileExists = true;

			lblModelNameError.Visible = false;
			lblPackageNameError.Visible = false;
		}

		~ImportModelDialog()
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

			ModelFileName = _OpenFile.FileName;
			txtModelName.Focus();

			return base.ShowDialog();
		}

		public string InitialDirectory
		{
			get { return _OpenFile.InitialDirectory; }
			set { _OpenFile.InitialDirectory = value; }
		}

		public string ModelFileName
		{
			get { return btnBrowse.Text; }
			set { btnBrowse.Text = value; }
		}

		public string ModelName
		{
			get { return txtModelName.Text; }
			set { txtModelName.Text = value; }
		}

		public string PackageName
		{
			get { return txtPackageName.Text; }
			set { txtPackageName.Text = value; }
		}

		public bool ImportAnimations
		{
			get { return cbImportAnimations.Checked; }
			set { cbImportAnimations.Checked = true; }
		}

		void WhenModelNameInvalid( string errorMessage )
		{
			txtModelName.Focus();
			txtModelName.SelectAll();
			lblModelNameError.Text = errorMessage;
			lblModelNameError.Visible = true;
		}

		void WhenPackageNameInvalid( string errorMessage )
		{
			txtPackageName.Focus();
			txtPackageName.SelectAll();
			lblPackageNameError.Text = errorMessage;
			lblPackageNameError.Visible = true;
		}

		bool ValidateModelName()
		{
			if( ModelName.Length == 0 )
			{
				WhenModelNameInvalid( "Input a model name." );
				return false;
			}

			foreach( char c in ModelName )
			{
				if( !Util.ValidateFileCharacter( c ) )
				{
					WhenModelNameInvalid( "Use A-Z, 0-9, dashes, and underscores." );
					return false;
				}
			}

			lblModelNameError.Visible = false;

			return true;
		}

		bool ValidatePackageName()
		{
			if( PackageName.Length == 0 )
			{
				WhenPackageNameInvalid( "Input a package name." );
				return false;
			}

			foreach( char c in PackageName )
			{
				if( !Util.ValidateFileCharacter( c ) )
				{
					WhenPackageNameInvalid( "Use A-Z, 0-9, dashes, and underscores." );
					return false;
				}
			}

			lblPackageNameError.Visible = false;

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

		private void ImportModelDialog_PreviewKeyDown( object sender, PreviewKeyDownEventArgs e )
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
				ModelFileName = _OpenFile.FileName;
		}

		private void btnAccept_Click( object sender, EventArgs e )
		{
			bool packageValid = ValidatePackageName();
			bool nameValid = ValidateModelName();
			if( packageValid && nameValid )
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