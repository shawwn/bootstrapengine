namespace Editor.Dialogs
{
    partial class TextureSelectionDialog
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
            this.components = new System.ComponentModel.Container();
            this.xpToolBar1 = new Syncfusion.Windows.Forms.Tools.XPMenus.XPToolBar();
            this.flowLayout1 = new Syncfusion.Windows.Forms.Tools.FlowLayout( this.components );
            this.bnOK = new Syncfusion.Windows.Forms.ButtonAdv();
            this.bnCancel = new Syncfusion.Windows.Forms.ButtonAdv();
            this.bnNone = new Syncfusion.Windows.Forms.ButtonAdv();
            this.label1 = new System.Windows.Forms.Label();
            this.cbPackage = new Editor.Controls.PackageComboBox();
            this.tbFilter = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.thumbnailSelector = new Editor.Controls.ThumbnailSelector();
            this.bnCreate = new Syncfusion.Windows.Forms.ButtonAdv();
            ( ( System.ComponentModel.ISupportInitialize )( this.flowLayout1 ) ).BeginInit();
            this.SuspendLayout();
            // 
            // xpToolBar1
            // 
            // 
            // 
            // 
            this.xpToolBar1.Bar.BarName = "";
            this.xpToolBar1.Bar.Manager = null;
            this.xpToolBar1.Location = new System.Drawing.Point( 410, 457 );
            this.xpToolBar1.Name = "xpToolBar1";
            this.xpToolBar1.Size = new System.Drawing.Size( 100, 23 );
            this.xpToolBar1.TabIndex = 1;
            this.xpToolBar1.Text = "xpToolBar1";
            // 
            // flowLayout1
            // 
            this.flowLayout1.Alignment = Syncfusion.Windows.Forms.Tools.FlowAlignment.Near;
            this.flowLayout1.BottomMargin = 5;
            this.flowLayout1.HorzFarMargin = 5;
            this.flowLayout1.HorzNearMargin = 5;
            this.flowLayout1.TopMargin = 5;
            // 
            // bnOK
            // 
            this.bnOK.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.bnOK.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.bnOK.Location = new System.Drawing.Point( 353, 413 );
            this.bnOK.Name = "bnOK";
            this.bnOK.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnOK.Size = new System.Drawing.Size( 75, 23 );
            this.bnOK.TabIndex = 3;
            this.bnOK.Text = "OK";
            this.bnOK.UseVisualStyle = true;
            // 
            // bnCancel
            // 
            this.bnCancel.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.bnCancel.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.bnCancel.Location = new System.Drawing.Point( 434, 413 );
            this.bnCancel.Name = "bnCancel";
            this.bnCancel.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnCancel.Size = new System.Drawing.Size( 75, 23 );
            this.bnCancel.TabIndex = 4;
            this.bnCancel.Text = "Cancel";
            this.bnCancel.UseVisualStyle = true;
            // 
            // bnNone
            // 
            this.bnNone.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left ) ) );
            this.bnNone.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnNone.DialogResult = System.Windows.Forms.DialogResult.No;
            this.bnNone.Location = new System.Drawing.Point( 515, 413 );
            this.bnNone.Name = "bnNone";
            this.bnNone.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnNone.Size = new System.Drawing.Size( 101, 23 );
            this.bnNone.TabIndex = 5;
            this.bnNone.Text = "No Texture";
            this.bnNone.UseVisualStyle = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point( 13, 16 );
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size( 50, 13 );
            this.label1.TabIndex = 7;
            this.label1.Text = "Package";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // cbPackage
            // 
            this.cbPackage.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbPackage.FormattingEnabled = true;
            this.cbPackage.Location = new System.Drawing.Point( 69, 13 );
            this.cbPackage.Name = "cbPackage";
            this.cbPackage.Size = new System.Drawing.Size( 184, 21 );
            this.cbPackage.Sorted = true;
            this.cbPackage.TabIndex = 8;
            this.cbPackage.SelectedIndexChanged += new System.EventHandler( this.cbPackage_SelectedIndexChanged );
            // 
            // tbFilter
            // 
            this.tbFilter.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.tbFilter.Location = new System.Drawing.Point( 450, 16 );
            this.tbFilter.Name = "tbFilter";
            this.tbFilter.Size = new System.Drawing.Size( 166, 20 );
            this.tbFilter.TabIndex = 9;
            this.tbFilter.Text = "*.*";
            this.tbFilter.KeyDown += new System.Windows.Forms.KeyEventHandler( this.tbFilter_KeyDown );
            // 
            // label2
            // 
            this.label2.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point( 415, 19 );
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size( 29, 13 );
            this.label2.TabIndex = 10;
            this.label2.Text = "Filter";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
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
            this.thumbnailSelector.Location = new System.Drawing.Point( 13, 39 );
            this.thumbnailSelector.Name = "thumbnailSelector";
            this.thumbnailSelector.Selected = null;
            this.thumbnailSelector.SelectedName = "";
            this.thumbnailSelector.ShowDeleteButtons = false;
            this.thumbnailSelector.Size = new System.Drawing.Size( 603, 362 );
            this.thumbnailSelector.TabIndex = 2;
            this.thumbnailSelector.ThumbnailSize = new System.Drawing.Size( 64, 64 );
            this.thumbnailSelector.SelectionMade += new System.EventHandler( this.thumbnailSelector_SelectionMade );
            // 
            // bnCreate
            // 
            this.bnCreate.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left ) ) );
            this.bnCreate.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnCreate.Location = new System.Drawing.Point( 13, 413 );
            this.bnCreate.Name = "bnCreate";
            this.bnCreate.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnCreate.Size = new System.Drawing.Size( 76, 23 );
            this.bnCreate.TabIndex = 11;
            this.bnCreate.Text = "Create...";
            this.bnCreate.UseVisualStyle = true;
            this.bnCreate.Click += new System.EventHandler( this.bnCreate_Click );
            // 
            // TextureSelectionDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb( ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ) );
            this.ClientSize = new System.Drawing.Size( 628, 448 );
            this.ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.Controls.Add( this.bnCreate );
            this.Controls.Add( this.label2 );
            this.Controls.Add( this.tbFilter );
            this.Controls.Add( this.cbPackage );
            this.Controls.Add( this.label1 );
            this.Controls.Add( this.bnNone );
            this.Controls.Add( this.bnCancel );
            this.Controls.Add( this.bnOK );
            this.Controls.Add( this.thumbnailSelector );
            this.Controls.Add( this.xpToolBar1 );
            this.MaximizeBox = false;
            this.MinimumSize = new System.Drawing.Size( 350, 350 );
            this.Name = "TextureSelectionDialog";
            this.Text = "Texture Selector";
            ( ( System.ComponentModel.ISupportInitialize )( this.flowLayout1 ) ).EndInit();
            this.ResumeLayout( false );
            this.PerformLayout();

        }

        #endregion

        private Syncfusion.Windows.Forms.Tools.XPMenus.XPToolBar xpToolBar1;
        private Syncfusion.Windows.Forms.Tools.FlowLayout flowLayout1;
        private Editor.Controls.ThumbnailSelector thumbnailSelector;
        private Syncfusion.Windows.Forms.ButtonAdv bnOK;
        private Syncfusion.Windows.Forms.ButtonAdv bnCancel;
        private Syncfusion.Windows.Forms.ButtonAdv bnNone;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbFilter;
        private System.Windows.Forms.Label label2;
        public Editor.Controls.PackageComboBox cbPackage;
        private Syncfusion.Windows.Forms.ButtonAdv bnCreate;



    }
}
