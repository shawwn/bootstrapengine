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
	public partial class BaseDialog : Office2007Form
	{
		public BaseDialog()
		{
			InitializeComponent();
		}

		protected override bool ProcessDialogKey( Keys keyData )
		{
			if( keyData == Keys.Enter )
			{
				this.DialogResult = DialogResult.OK;
				this.Close();
				return true;
			}
			else if( keyData == Keys.Escape )
			{
				this.DialogResult = DialogResult.Cancel;
				this.Close();
				return true;
			}

			return base.ProcessDialogKey( keyData );
		}
	}
}