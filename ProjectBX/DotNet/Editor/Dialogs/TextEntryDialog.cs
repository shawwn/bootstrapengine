using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

namespace Editor.Dialogs
{
	public partial class TextEntryDialog : BaseDialog
	{
		public TextEntryDialog()
		{
			InitializeComponent();
			txtEntry.Select();
		}

		public string Title
		{
			get { return _title; }
			set 
			{
				if( _title == value ) 
					return;
				_title = value;
				this.Text = value;
			}
		}
		string _title;

		public string Caption
		{
			get { return _caption; }
			set 
			{
				if( _caption == value ) 
					return;
				_caption = value;
				lblCaption.Text = value;
			}
		}
		string _caption;

		public string DefaultText
		{
			get { return _defaultText; }
			set
			{
				if( _defaultText == value )
					return;
				_defaultText = value;
				txtEntry.Text = _defaultText;
			}
		}
		string _defaultText;

		public string TextResult
		{
			get { return txtEntry.Text; }
		}

		protected override void OnVisibleChanged( EventArgs e )
		{
			base.OnVisibleChanged( e );
			if( this.Visible )
			{
				txtEntry.Focus();
				txtEntry.SelectAll();
			}
		}
	}
}