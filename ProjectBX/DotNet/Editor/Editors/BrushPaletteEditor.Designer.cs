namespace Editor.Editors
{
    partial class BrushPaletteEditor
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager( typeof( BrushPaletteEditor ) );
            this.pnPalette = new System.Windows.Forms.GroupBox();
            this.pnEdit = new System.Windows.Forms.Panel();
            this.bnRemove = new Syncfusion.Windows.Forms.ButtonAdv();
            this.bnAdd = new Syncfusion.Windows.Forms.ButtonAdv();
            this.pnPaletteControls = new System.Windows.Forms.Panel();
            this.bnNew = new Syncfusion.Windows.Forms.ButtonAdv();
            this.bnOpen = new Syncfusion.Windows.Forms.ButtonAdv();
            this.thumbnailSelector = new Editor.Controls.ThumbnailSelector();
            this.popupEditBrush = new Editor.Controls.OpaqueMiniToolbar();
            this.tsBrushEditor = new Editor.ToolStripBrushEditor();
            this.popupEditBrush_pnControlBox = new Editor.Controls.ColoredToolStripPanelItem();
            this.popupEditBrush_bnClose = new System.Windows.Forms.ToolStripButton();
            this.pnPalette.SuspendLayout();
            this.pnEdit.SuspendLayout();
            this.pnPaletteControls.SuspendLayout();
            this.popupEditBrush.SuspendLayout();
            this.SuspendLayout();
            // 
            // pnPalette
            // 
            this.pnPalette.Controls.Add( this.thumbnailSelector );
            this.pnPalette.Controls.Add( this.pnEdit );
            this.pnPalette.Dock = System.Windows.Forms.DockStyle.Fill;
            this.pnPalette.ForeColor = System.Drawing.Color.White;
            this.pnPalette.Location = new System.Drawing.Point( 0, 33 );
            this.pnPalette.Name = "pnPalette";
            this.pnPalette.Size = new System.Drawing.Size( 280, 191 );
            this.pnPalette.TabIndex = 2;
            this.pnPalette.TabStop = false;
            this.pnPalette.Text = "Brushes";
            // 
            // pnEdit
            // 
            this.pnEdit.Controls.Add( this.bnRemove );
            this.pnEdit.Controls.Add( this.bnAdd );
            this.pnEdit.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnEdit.Location = new System.Drawing.Point( 3, 16 );
            this.pnEdit.Name = "pnEdit";
            this.pnEdit.Size = new System.Drawing.Size( 274, 37 );
            this.pnEdit.TabIndex = 3;
            // 
            // bnRemove
            // 
            this.bnRemove.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnRemove.Dock = System.Windows.Forms.DockStyle.Left;
            this.bnRemove.Image = ( ( System.Drawing.Image )( resources.GetObject( "bnRemove.Image" ) ) );
            this.bnRemove.Location = new System.Drawing.Point( 100, 0 );
            this.bnRemove.Name = "bnRemove";
            this.bnRemove.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnRemove.Size = new System.Drawing.Size( 110, 37 );
            this.bnRemove.TabIndex = 1;
            this.bnRemove.Text = "Remove";
            this.bnRemove.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.bnRemove.UseVisualStyle = true;
            this.bnRemove.Click += new System.EventHandler( this.bnRemove_Click );
            // 
            // bnAdd
            // 
            this.bnAdd.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnAdd.Dock = System.Windows.Forms.DockStyle.Left;
            this.bnAdd.Image = ( ( System.Drawing.Image )( resources.GetObject( "bnAdd.Image" ) ) );
            this.bnAdd.Location = new System.Drawing.Point( 0, 0 );
            this.bnAdd.Name = "bnAdd";
            this.bnAdd.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnAdd.Size = new System.Drawing.Size( 100, 37 );
            this.bnAdd.TabIndex = 1;
            this.bnAdd.Text = "Add";
            this.bnAdd.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.bnAdd.UseVisualStyle = true;
            this.bnAdd.Click += new System.EventHandler( this.bnAdd_Click );
            // 
            // pnPaletteControls
            // 
            this.pnPaletteControls.Controls.Add( this.bnNew );
            this.pnPaletteControls.Controls.Add( this.bnOpen );
            this.pnPaletteControls.Dock = System.Windows.Forms.DockStyle.Top;
            this.pnPaletteControls.Location = new System.Drawing.Point( 0, 0 );
            this.pnPaletteControls.Name = "pnPaletteControls";
            this.pnPaletteControls.Size = new System.Drawing.Size( 280, 33 );
            this.pnPaletteControls.TabIndex = 5;
            // 
            // bnNew
            // 
            this.bnNew.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnNew.Dock = System.Windows.Forms.DockStyle.Left;
            this.bnNew.Image = ( ( System.Drawing.Image )( resources.GetObject( "bnNew.Image" ) ) );
            this.bnNew.Location = new System.Drawing.Point( 72, 0 );
            this.bnNew.Name = "bnNew";
            this.bnNew.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnNew.Size = new System.Drawing.Size( 72, 33 );
            this.bnNew.TabIndex = 2;
            this.bnNew.Text = "New";
            this.bnNew.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.bnNew.UseVisualStyle = true;
            this.bnNew.Click += new System.EventHandler( this.bnNew_Click );
            // 
            // bnOpen
            // 
            this.bnOpen.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnOpen.Dock = System.Windows.Forms.DockStyle.Left;
            this.bnOpen.Image = ( ( System.Drawing.Image )( resources.GetObject( "bnOpen.Image" ) ) );
            this.bnOpen.Location = new System.Drawing.Point( 0, 0 );
            this.bnOpen.Name = "bnOpen";
            this.bnOpen.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnOpen.Size = new System.Drawing.Size( 72, 33 );
            this.bnOpen.TabIndex = 2;
            this.bnOpen.Text = "Open";
            this.bnOpen.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.bnOpen.UseVisualStyle = true;
            this.bnOpen.Click += new System.EventHandler( this.bnOpen_Click );
            // 
            // thumbnailSelector
            // 
            this.thumbnailSelector.AllowMultiSelect = true;
            this.thumbnailSelector.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.thumbnailSelector.BackColor = System.Drawing.Color.Black;
            this.thumbnailSelector.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.thumbnailSelector.Dock = System.Windows.Forms.DockStyle.Fill;
            this.thumbnailSelector.Location = new System.Drawing.Point( 3, 53 );
            this.thumbnailSelector.Name = "thumbnailSelector";
            this.thumbnailSelector.Selected = null;
            this.thumbnailSelector.SelectedName = "";
            this.thumbnailSelector.Size = new System.Drawing.Size( 274, 135 );
            this.thumbnailSelector.TabIndex = 2;
            this.thumbnailSelector.ThumbnailSize = new System.Drawing.Size( 64, 64 );
            // 
            // popupEditBrush
            // 
            this.popupEditBrush.BackColor = System.Drawing.Color.FromArgb( ( ( int )( ( ( byte )( 113 ) ) ) ), ( ( int )( ( ( byte )( 113 ) ) ) ), ( ( int )( ( ( byte )( 113 ) ) ) ) );
            this.popupEditBrush.Items.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.tsBrushEditor,
            this.popupEditBrush_pnControlBox} );
            this.popupEditBrush.Name = "popupEditBrush";
            this.popupEditBrush.ShowItemToolTips = false;
            this.popupEditBrush.Size = new System.Drawing.Size( 307, 275 );
            // 
            // tsBrushEditor
            // 
            this.tsBrushEditor.AutoSize = false;
            this.tsBrushEditor.Brush = null;
            this.tsBrushEditor.Name = "tsBrushEditor";
            this.tsBrushEditor.Size = new System.Drawing.Size( 276, 268 );
            // 
            // popupEditBrush_pnControlBox
            // 
            this.popupEditBrush_pnControlBox.CausesValidation = false;
            this.popupEditBrush_pnControlBox.ForeColor = System.Drawing.Color.MidnightBlue;
            this.popupEditBrush_pnControlBox.Items.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.popupEditBrush_bnClose} );
            this.popupEditBrush_pnControlBox.Name = "popupEditBrush_pnControlBox";
            this.popupEditBrush_pnControlBox.ShowItemToolTips = false;
            this.popupEditBrush_pnControlBox.Size = new System.Drawing.Size( 27, 271 );
            this.popupEditBrush_pnControlBox.Text = "toolStripPanelItem1";
            this.popupEditBrush_pnControlBox.Transparent = true;
            // 
            // popupEditBrush_bnClose
            // 
            this.popupEditBrush_bnClose.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.popupEditBrush_bnClose.Image = ( ( System.Drawing.Image )( resources.GetObject( "popupEditBrush_bnClose.Image" ) ) );
            this.popupEditBrush_bnClose.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.popupEditBrush_bnClose.Name = "popupEditBrush_bnClose";
            this.popupEditBrush_bnClose.Size = new System.Drawing.Size( 23, 20 );
            this.popupEditBrush_bnClose.Text = "toolStripButton1";
            this.popupEditBrush_bnClose.Click += new System.EventHandler( this.popupEditBrush_bnClose_Click );
            // 
            // BrushPaletteEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add( this.pnPalette );
            this.Controls.Add( this.pnPaletteControls );
            this.Name = "BrushPaletteEditor";
            this.Size = new System.Drawing.Size( 280, 224 );
            this.pnPalette.ResumeLayout( false );
            this.pnEdit.ResumeLayout( false );
            this.pnPaletteControls.ResumeLayout( false );
            this.popupEditBrush.ResumeLayout( false );
            this.popupEditBrush.PerformLayout();
            this.ResumeLayout( false );
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox pnPalette;
        private Editor.Controls.ThumbnailSelector thumbnailSelector;
        private System.Windows.Forms.Panel pnEdit;
        private Syncfusion.Windows.Forms.ButtonAdv bnAdd;
        private Syncfusion.Windows.Forms.ButtonAdv bnRemove;
        private System.Windows.Forms.Panel pnPaletteControls;
        private Syncfusion.Windows.Forms.ButtonAdv bnNew;
        private Syncfusion.Windows.Forms.ButtonAdv bnOpen;
        private Editor.Controls.OpaqueMiniToolbar popupEditBrush;
        private ToolStripBrushEditor tsBrushEditor;
        private Editor.Controls.ColoredToolStripPanelItem popupEditBrush_pnControlBox;
        private System.Windows.Forms.ToolStripButton popupEditBrush_bnClose;

    }
}
