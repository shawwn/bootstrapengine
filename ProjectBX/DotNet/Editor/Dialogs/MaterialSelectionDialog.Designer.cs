namespace Editor.Dialogs
{
    partial class MaterialSelectionDialog
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			this.groupBarItem1 = new Syncfusion.Windows.Forms.Tools.GroupBarItem();
			this.groupBarItem2 = new Syncfusion.Windows.Forms.Tools.GroupBarItem();
			this.groupBarItem3 = new Syncfusion.Windows.Forms.Tools.GroupBarItem();
			this.groupBarItem4 = new Syncfusion.Windows.Forms.Tools.GroupBarItem();
			this.bnReload = new Syncfusion.Windows.Forms.ButtonAdv();
			this.bnNew = new Syncfusion.Windows.Forms.ButtonAdv();
			this.bnCancel = new Syncfusion.Windows.Forms.ButtonAdv();
			this.bnOK = new Syncfusion.Windows.Forms.ButtonAdv();
			this.thumbnailSelector = new Editor.Controls.ThumbnailSelector();
			this.SuspendLayout();
			// 
			// groupBarItem1
			// 
			this.groupBarItem1.BackColor = System.Drawing.Color.FromArgb( ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ) );
			this.groupBarItem1.Client = null;
			this.groupBarItem1.Text = "Pass 1";
			// 
			// groupBarItem2
			// 
			this.groupBarItem2.BackColor = System.Drawing.Color.FromArgb( ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ) );
			this.groupBarItem2.Client = null;
			this.groupBarItem2.Text = "Pass 2";
			// 
			// groupBarItem3
			// 
			this.groupBarItem3.BackColor = System.Drawing.Color.FromArgb( ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ) );
			this.groupBarItem3.Client = null;
			this.groupBarItem3.Text = "Pass 3";
			// 
			// groupBarItem4
			// 
			this.groupBarItem4.BackColor = System.Drawing.Color.FromArgb( ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ) );
			this.groupBarItem4.Client = null;
			this.groupBarItem4.Text = "Pass 4";
			// 
			// bnReload
			// 
			this.bnReload.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
			this.bnReload.Location = new System.Drawing.Point( 12, 12 );
			this.bnReload.Name = "bnReload";
			this.bnReload.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
			this.bnReload.Size = new System.Drawing.Size( 165, 23 );
			this.bnReload.TabIndex = 14;
			this.bnReload.Text = "Reload Materials";
			this.bnReload.UseVisualStyle = true;
			this.bnReload.Click += new System.EventHandler( this.bnReload_Click );
			// 
			// bnNew
			// 
			this.bnNew.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left ) ) );
			this.bnNew.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
			this.bnNew.Location = new System.Drawing.Point( 12, 413 );
			this.bnNew.Name = "bnNew";
			this.bnNew.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
			this.bnNew.Size = new System.Drawing.Size( 75, 23 );
			this.bnNew.TabIndex = 17;
			this.bnNew.Text = "New...";
			this.bnNew.UseVisualStyle = true;
			// 
			// bnCancel
			// 
			this.bnCancel.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right ) ) );
			this.bnCancel.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
			this.bnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.bnCancel.Location = new System.Drawing.Point( 541, 413 );
			this.bnCancel.Name = "bnCancel";
			this.bnCancel.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
			this.bnCancel.Size = new System.Drawing.Size( 75, 23 );
			this.bnCancel.TabIndex = 16;
			this.bnCancel.Text = "Cancel";
			this.bnCancel.UseVisualStyle = true;
			// 
			// bnOK
			// 
			this.bnOK.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right ) ) );
			this.bnOK.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
			this.bnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.bnOK.Location = new System.Drawing.Point( 460, 413 );
			this.bnOK.Name = "bnOK";
			this.bnOK.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
			this.bnOK.Size = new System.Drawing.Size( 75, 23 );
			this.bnOK.TabIndex = 15;
			this.bnOK.Text = "OK";
			this.bnOK.UseVisualStyle = true;
			// 
			// thumbnailSelector
			// 
			this.thumbnailSelector.AllowMultiSelect = true;
			this.thumbnailSelector.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom ) 
            | System.Windows.Forms.AnchorStyles.Left ) 
            | System.Windows.Forms.AnchorStyles.Right ) ) );
			this.thumbnailSelector.AutoScroll = true;
			this.thumbnailSelector.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.thumbnailSelector.BackColor = System.Drawing.Color.Black;
			this.thumbnailSelector.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.thumbnailSelector.Location = new System.Drawing.Point( 12, 41 );
			this.thumbnailSelector.Name = "thumbnailSelector";
			this.thumbnailSelector.Selected = null;
			this.thumbnailSelector.SelectedName = "";
			this.thumbnailSelector.ShowDeleteButtons = false;
			this.thumbnailSelector.Size = new System.Drawing.Size( 604, 366 );
			this.thumbnailSelector.TabIndex = 9;
			this.thumbnailSelector.ThumbnailSize = new System.Drawing.Size( 64, 64 );
			this.thumbnailSelector.SelectionChanged += new System.EventHandler( this.thumbnailSelector_SelectionChanged );
			this.thumbnailSelector.SelectionMade += new System.EventHandler( this.thumbnailSelector_SelectionMade );
			// 
			// MaterialSelectionDialog
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.BackColor = System.Drawing.Color.FromArgb( ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ) );
			this.ClientSize = new System.Drawing.Size( 628, 448 );
			this.ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
			this.Controls.Add( this.bnNew );
			this.Controls.Add( this.bnCancel );
			this.Controls.Add( this.bnOK );
			this.Controls.Add( this.bnReload );
			this.Controls.Add( this.thumbnailSelector );
			this.MinimumSize = new System.Drawing.Size( 350, 350 );
			this.Name = "MaterialSelectionDialog";
			this.Text = "Material Selector";
			this.VisibleChanged += new System.EventHandler( this.MaterialSelectionDialog_VisibleChanged );
			this.ResumeLayout( false );

        }

        #endregion

        private Syncfusion.Windows.Forms.Tools.GroupBarItem groupBarItem1;
        private Syncfusion.Windows.Forms.Tools.GroupBarItem groupBarItem2;
        private Syncfusion.Windows.Forms.Tools.GroupBarItem groupBarItem3;
        private Syncfusion.Windows.Forms.Tools.GroupBarItem groupBarItem4;
        private Editor.Controls.ThumbnailSelector thumbnailSelector;
        private Syncfusion.Windows.Forms.ButtonAdv bnReload;
        private Syncfusion.Windows.Forms.ButtonAdv bnNew;
        private Syncfusion.Windows.Forms.ButtonAdv bnCancel;
        private Syncfusion.Windows.Forms.ButtonAdv bnOK;
    }
}
