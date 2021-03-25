namespace Editor.Dialogs
{
	partial class TextEntryDialog
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose( bool disposing )
		{
			if( disposing && ( components != null ) )
			{
				components.Dispose();
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.lblCaption = new System.Windows.Forms.Label();
			this.txtEntry = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// bnCancel
			// 
			this.bnCancel.Location = new System.Drawing.Point( 132, 65 );
			// 
			// bnOK
			// 
			this.bnOK.Location = new System.Drawing.Point( 42, 65 );
			// 
			// lblCaption
			// 
			this.lblCaption.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left ) 
            | System.Windows.Forms.AnchorStyles.Right ) ) );
			this.lblCaption.ForeColor = System.Drawing.Color.White;
			this.lblCaption.Location = new System.Drawing.Point( 7, 9 );
			this.lblCaption.Name = "lblCaption";
			this.lblCaption.Size = new System.Drawing.Size( 201, 19 );
			this.lblCaption.TabIndex = 12;
			this.lblCaption.Text = "Caption";
			// 
			// txtEntry
			// 
			this.txtEntry.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left ) 
            | System.Windows.Forms.AnchorStyles.Right ) ) );
			this.txtEntry.Location = new System.Drawing.Point( 7, 31 );
			this.txtEntry.Name = "txtEntry";
			this.txtEntry.Size = new System.Drawing.Size( 200, 20 );
			this.txtEntry.TabIndex = 13;
			// 
			// TextEntryDialog
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size( 220, 102 );
			this.Controls.Add( this.lblCaption );
			this.Controls.Add( this.txtEntry );
			this.Name = "TextEntryDialog";
			this.Controls.SetChildIndex( this.txtEntry, 0 );
			this.Controls.SetChildIndex( this.lblCaption, 0 );
			this.Controls.SetChildIndex( this.bnOK, 0 );
			this.Controls.SetChildIndex( this.bnCancel, 0 );
			this.ResumeLayout( false );
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label lblCaption;
		private System.Windows.Forms.TextBox txtEntry;
	}
}