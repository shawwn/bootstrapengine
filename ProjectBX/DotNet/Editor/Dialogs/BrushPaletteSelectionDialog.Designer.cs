namespace Editor.Dialogs
{
	partial class BrushPaletteSelectionDialog
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
			this.thumbnailSelector = new Editor.Controls.ThumbnailSelector();
			this.bnNew = new Syncfusion.Windows.Forms.ButtonAdv();
			this.btnReload = new Syncfusion.Windows.Forms.ButtonAdv();
			this.SuspendLayout();
			// 
			// bnCancel
			// 
			this.bnCancel.Location = new System.Drawing.Point( 427, 343 );
			// 
			// bnOK
			// 
			this.bnOK.Location = new System.Drawing.Point( 337, 343 );
			// 
			// thumbnailSelector
			// 
			this.thumbnailSelector.AllowMultiSelect = false;
			this.thumbnailSelector.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom ) 
            | System.Windows.Forms.AnchorStyles.Left ) 
            | System.Windows.Forms.AnchorStyles.Right ) ) );
			this.thumbnailSelector.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.thumbnailSelector.BackColor = System.Drawing.Color.Black;
			this.thumbnailSelector.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.thumbnailSelector.Location = new System.Drawing.Point( 12, 37 );
			this.thumbnailSelector.Name = "thumbnailSelector";
			this.thumbnailSelector.Selected = null;
			this.thumbnailSelector.SelectedName = "";
			this.thumbnailSelector.Size = new System.Drawing.Size( 492, 300 );
			this.thumbnailSelector.TabIndex = 1;
			this.thumbnailSelector.ThumbnailSize = new System.Drawing.Size( 64, 64 );
			// 
			// bnNew
			// 
			this.bnNew.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left ) ) );
			this.bnNew.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
			this.bnNew.Location = new System.Drawing.Point( 12, 343 );
			this.bnNew.Name = "bnNew";
			this.bnNew.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
			this.bnNew.Size = new System.Drawing.Size( 75, 23 );
			this.bnNew.TabIndex = 10;
			this.bnNew.Text = "New...";
			this.bnNew.UseVisualStyle = true;
			this.bnNew.Click += new System.EventHandler( this.bnNew_Click );
			// 
			// btnReload
			// 
			this.btnReload.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left ) ) );
			this.btnReload.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
			this.btnReload.Location = new System.Drawing.Point( 12, 12 );
			this.btnReload.Name = "btnReload";
			this.btnReload.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
			this.btnReload.Size = new System.Drawing.Size( 117, 23 );
			this.btnReload.TabIndex = 10;
			this.btnReload.Text = "Reload Palettes";
			this.btnReload.UseVisualStyle = true;
			this.btnReload.Click += new System.EventHandler( this.btnReload_Click );
			// 
			// BrushPaletteSelectionDialog
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size( 513, 378 );
			this.Controls.Add( this.btnReload );
			this.Controls.Add( this.bnNew );
			this.Controls.Add( this.thumbnailSelector );
			this.Name = "BrushPaletteSelectionDialog";
			this.Text = "Palette Selector";
			this.Controls.SetChildIndex( this.bnOK, 0 );
			this.Controls.SetChildIndex( this.bnCancel, 0 );
			this.Controls.SetChildIndex( this.thumbnailSelector, 0 );
			this.Controls.SetChildIndex( this.bnNew, 0 );
			this.Controls.SetChildIndex( this.btnReload, 0 );
			this.ResumeLayout( false );

		}

		#endregion

		private Editor.Controls.ThumbnailSelector thumbnailSelector;
		private Syncfusion.Windows.Forms.ButtonAdv bnNew;
		private Syncfusion.Windows.Forms.ButtonAdv btnReload;
	}
}