namespace Editor
{
    partial class ImportModelDialog
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
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager( typeof( ImportModelDialog ) );
            this.ribbonControlAdv1 = new Syncfusion.Windows.Forms.Tools.RibbonControlAdv();
            this.toolStripTabItem1 = new Syncfusion.Windows.Forms.Tools.ToolStripTabItem();
            this.toolStripEx1 = new Syncfusion.Windows.Forms.Tools.ToolStripEx();
            this.cbImportAnimations = new Syncfusion.Windows.Forms.Tools.ToolStripCheckBox();
            this.toolStripEx2 = new Syncfusion.Windows.Forms.Tools.ToolStripEx();
            this.btnBrowse = new System.Windows.Forms.ToolStripButton();
            this.panel1 = new System.Windows.Forms.Panel();
            this.gradientPanelExt2 = new Syncfusion.Windows.Forms.Tools.GradientPanelExt();
            this.label2 = new System.Windows.Forms.Label();
            this.txtModelName = new Syncfusion.Windows.Forms.Tools.TextBoxExt();
            this.lblModelNameError = new System.Windows.Forms.Label();
            this.gradientPanelExt1 = new Syncfusion.Windows.Forms.Tools.GradientPanelExt();
            this.label1 = new System.Windows.Forms.Label();
            this.txtPackageName = new Syncfusion.Windows.Forms.Tools.TextBoxExt();
            this.lblPackageNameError = new System.Windows.Forms.Label();
            this.toolStripEx3 = new Syncfusion.Windows.Forms.Tools.ToolStripEx();
            this.btnAccept = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.btnCancel = new System.Windows.Forms.ToolStripButton();
            this.layoutChoosePackage = new Syncfusion.Windows.Forms.Tools.FlowLayout( this.components );
            this.layoutContent = new Syncfusion.Windows.Forms.Tools.FlowLayout( this.components );
            this.flowLayout1 = new Syncfusion.Windows.Forms.Tools.FlowLayout( this.components );
            this.layoutNameModel = new Syncfusion.Windows.Forms.Tools.FlowLayout( this.components );
            ( ( System.ComponentModel.ISupportInitialize )( this.ribbonControlAdv1 ) ).BeginInit();
            this.ribbonControlAdv1.SuspendLayout();
            this.toolStripTabItem1.Panel.SuspendLayout();
            this.toolStripEx1.SuspendLayout();
            this.toolStripEx2.SuspendLayout();
            this.panel1.SuspendLayout();
            ( ( System.ComponentModel.ISupportInitialize )( this.gradientPanelExt2 ) ).BeginInit();
            this.gradientPanelExt2.SuspendLayout();
            ( ( System.ComponentModel.ISupportInitialize )( this.gradientPanelExt1 ) ).BeginInit();
            this.gradientPanelExt1.SuspendLayout();
            this.toolStripEx3.SuspendLayout();
            ( ( System.ComponentModel.ISupportInitialize )( this.layoutChoosePackage ) ).BeginInit();
            ( ( System.ComponentModel.ISupportInitialize )( this.layoutContent ) ).BeginInit();
            ( ( System.ComponentModel.ISupportInitialize )( this.flowLayout1 ) ).BeginInit();
            ( ( System.ComponentModel.ISupportInitialize )( this.layoutNameModel ) ).BeginInit();
            this.SuspendLayout();
            // 
            // ribbonControlAdv1
            // 
            this.ribbonControlAdv1.Header.AddMainItem( toolStripTabItem1 );
            this.ribbonControlAdv1.Location = new System.Drawing.Point( 1, 0 );
            this.ribbonControlAdv1.MenuButtonVisible = false;
            this.ribbonControlAdv1.Name = "ribbonControlAdv1";
            // 
            // ribbonControlAdv1.OfficeMenu
            // 
            this.ribbonControlAdv1.OfficeMenu.Name = "OfficeMenu";
            this.ribbonControlAdv1.OfficeMenu.Size = new System.Drawing.Size( 12, 65 );
            this.ribbonControlAdv1.QuickPanelVisible = false;
            this.ribbonControlAdv1.ShowCaption = false;
            this.ribbonControlAdv1.ShowLauncher = false;
            this.ribbonControlAdv1.Size = new System.Drawing.Size( 421, 119 );
            this.ribbonControlAdv1.SystemText.QuickAccessDialogDropDownName = "Start menu";
            this.ribbonControlAdv1.TabIndex = 0;
            this.ribbonControlAdv1.Text = "ribbonControlAdv1";
            // 
            // toolStripTabItem1
            // 
            this.toolStripTabItem1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.None;
            this.toolStripTabItem1.Name = "toolStripTabItem1";
            // 
            // ribbonControlAdv1.ribbonPanel1
            // 
            this.toolStripTabItem1.Panel.Controls.Add( this.toolStripEx1 );
            this.toolStripTabItem1.Panel.Controls.Add( this.toolStripEx2 );
            this.toolStripTabItem1.Panel.Name = "ribbonPanel1";
            this.toolStripTabItem1.Panel.ScrollPosition = 0;
            this.toolStripTabItem1.Panel.ShowCaption = false;
            this.toolStripTabItem1.Panel.ShowLauncher = false;
            this.toolStripTabItem1.Panel.TabIndex = 2;
            this.toolStripTabItem1.Panel.Text = "toolStripTabItem1";
            this.toolStripTabItem1.Size = new System.Drawing.Size( 23, 6 );
            this.toolStripTabItem1.Text = "toolStripTabItem1";
            // 
            // toolStripEx1
            // 
            this.toolStripEx1.AutoSize = false;
            this.toolStripEx1.CanOverflow = false;
            this.toolStripEx1.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStripEx1.ForeColor = System.Drawing.Color.MidnightBlue;
            this.toolStripEx1.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStripEx1.Image = null;
            this.toolStripEx1.Items.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.cbImportAnimations} );
            this.toolStripEx1.Location = new System.Drawing.Point( 0, 1 );
            this.toolStripEx1.Name = "toolStripEx1";
            this.toolStripEx1.Size = new System.Drawing.Size( 130, 58 );
            this.toolStripEx1.TabIndex = 0;
            // 
            // cbImportAnimations
            // 
            this.cbImportAnimations.Checked = true;
            this.cbImportAnimations.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbImportAnimations.Name = "cbImportAnimations";
            this.cbImportAnimations.Size = new System.Drawing.Size( 113, 51 );
            this.cbImportAnimations.Text = "Import Animations";
            // 
            // toolStripEx2
            // 
            this.toolStripEx2.AutoSize = false;
            this.toolStripEx2.CanOverflow = false;
            this.toolStripEx2.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStripEx2.ForeColor = System.Drawing.Color.MidnightBlue;
            this.toolStripEx2.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStripEx2.Image = null;
            this.toolStripEx2.Items.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.btnBrowse} );
            this.toolStripEx2.Location = new System.Drawing.Point( 130, 1 );
            this.toolStripEx2.Name = "toolStripEx2";
            this.toolStripEx2.Size = new System.Drawing.Size( 277, 58 );
            this.toolStripEx2.TabIndex = 1;
            // 
            // btnBrowse
            // 
            this.btnBrowse.AutoSize = false;
            this.btnBrowse.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btnBrowse.Image = ( ( System.Drawing.Image )( resources.GetObject( "btnBrowse.Image" ) ) );
            this.btnBrowse.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnBrowse.Name = "btnBrowse";
            this.btnBrowse.Size = new System.Drawing.Size( 250, 51 );
            this.btnBrowse.Text = "C:\\bin\\data\\metalroomnew\\example.dae";
            this.btnBrowse.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.btnBrowse.Click += new System.EventHandler( this.btnBrowse_Click );
            // 
            // panel1
            // 
            this.panel1.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom ) 
            | System.Windows.Forms.AnchorStyles.Left ) 
            | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.panel1.Controls.Add( this.gradientPanelExt2 );
            this.panel1.Controls.Add( this.gradientPanelExt1 );
            this.panel1.Controls.Add( this.toolStripEx3 );
            this.panel1.Location = new System.Drawing.Point( 119, 125 );
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size( 198, 251 );
            this.panel1.TabIndex = 8;
            // 
            // gradientPanelExt2
            // 
            this.gradientPanelExt2.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left ) 
            | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.gradientPanelExt2.BackColor = System.Drawing.Color.Transparent;
            this.gradientPanelExt2.BorderColor = System.Drawing.Color.Black;
            this.gradientPanelExt2.BorderGap = 2;
            this.gradientPanelExt2.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.gradientPanelExt2.Controls.Add( this.label2 );
            this.gradientPanelExt2.Controls.Add( this.txtModelName );
            this.gradientPanelExt2.Controls.Add( this.lblModelNameError );
            this.gradientPanelExt2.CornerRadius = 8;
            this.gradientPanelExt2.ExpandLocation = new System.Drawing.Point( 0, 0 );
            this.gradientPanelExt2.ExpandSize = new System.Drawing.Size( 0, 0 );
            this.gradientPanelExt2.Location = new System.Drawing.Point( 3, 0 );
            this.gradientPanelExt2.Name = "gradientPanelExt2";
            this.gradientPanelExt2.Size = new System.Drawing.Size( 192, 92 );
            this.gradientPanelExt2.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font( "Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ( ( byte )( 0 ) ) );
            this.label2.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.label2.Location = new System.Drawing.Point( 43, 7 );
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size( 104, 14 );
            this.label2.TabIndex = 2;
            this.label2.Text = "Name Your Model";
            // 
            // txtModelName
            // 
            this.txtModelName.Location = new System.Drawing.Point( 20, 24 );
            this.txtModelName.Margin = new System.Windows.Forms.Padding( 5 );
            this.txtModelName.Name = "txtModelName";
            this.txtModelName.OverflowIndicatorToolTipText = null;
            this.txtModelName.Size = new System.Drawing.Size( 150, 20 );
            this.txtModelName.TabIndex = 1;
            // 
            // lblModelNameError
            // 
            this.lblModelNameError.Font = new System.Drawing.Font( "Arial", 8.25F, System.Drawing.FontStyle.Bold );
            this.lblModelNameError.ForeColor = System.Drawing.Color.Maroon;
            this.lblModelNameError.Location = new System.Drawing.Point( 28, 47 );
            this.lblModelNameError.Name = "lblModelNameError";
            this.lblModelNameError.Size = new System.Drawing.Size( 133, 34 );
            this.lblModelNameError.TabIndex = 4;
            this.lblModelNameError.Text = "error";
            this.lblModelNameError.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.lblModelNameError.Visible = false;
            // 
            // gradientPanelExt1
            // 
            this.gradientPanelExt1.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left ) 
            | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.gradientPanelExt1.BackColor = System.Drawing.Color.Transparent;
            this.gradientPanelExt1.BorderColor = System.Drawing.Color.Black;
            this.gradientPanelExt1.BorderGap = 2;
            this.gradientPanelExt1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.gradientPanelExt1.Controls.Add( this.label1 );
            this.gradientPanelExt1.Controls.Add( this.txtPackageName );
            this.gradientPanelExt1.Controls.Add( this.lblPackageNameError );
            this.gradientPanelExt1.CornerRadius = 8;
            this.gradientPanelExt1.ExpandLocation = new System.Drawing.Point( 0, 0 );
            this.gradientPanelExt1.ExpandSize = new System.Drawing.Size( 0, 0 );
            this.gradientPanelExt1.Location = new System.Drawing.Point( 3, 97 );
            this.gradientPanelExt1.Name = "gradientPanelExt1";
            this.gradientPanelExt1.Size = new System.Drawing.Size( 192, 92 );
            this.gradientPanelExt1.TabIndex = 6;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font( "Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ( ( byte )( 0 ) ) );
            this.label1.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.label1.Location = new System.Drawing.Point( 41, 7 );
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size( 108, 14 );
            this.label1.TabIndex = 2;
            this.label1.Text = "Choose a Package";
            // 
            // txtPackageName
            // 
            this.txtPackageName.Location = new System.Drawing.Point( 20, 24 );
            this.txtPackageName.Margin = new System.Windows.Forms.Padding( 5 );
            this.txtPackageName.Name = "txtPackageName";
            this.txtPackageName.OverflowIndicatorToolTipText = null;
            this.txtPackageName.Size = new System.Drawing.Size( 150, 20 );
            this.txtPackageName.TabIndex = 2;
            // 
            // lblPackageNameError
            // 
            this.lblPackageNameError.Font = new System.Drawing.Font( "Arial", 8.25F, System.Drawing.FontStyle.Bold );
            this.lblPackageNameError.ForeColor = System.Drawing.Color.Maroon;
            this.lblPackageNameError.Location = new System.Drawing.Point( 28, 47 );
            this.lblPackageNameError.Name = "lblPackageNameError";
            this.lblPackageNameError.Size = new System.Drawing.Size( 133, 34 );
            this.lblPackageNameError.TabIndex = 4;
            this.lblPackageNameError.Text = "error";
            this.lblPackageNameError.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.lblPackageNameError.Visible = false;
            // 
            // toolStripEx3
            // 
            this.toolStripEx3.AutoSize = false;
            this.toolStripEx3.BorderStyle = Syncfusion.Windows.Forms.Tools.ToolStripBorderStyle.Etched;
            this.toolStripEx3.CanOverflow = false;
            this.toolStripEx3.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStripEx3.ForeColor = System.Drawing.Color.MidnightBlue;
            this.toolStripEx3.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStripEx3.Image = null;
            this.toolStripEx3.Items.AddRange( new System.Windows.Forms.ToolStripItem[] {
            this.btnAccept,
            this.toolStripSeparator1,
            this.btnCancel} );
            this.toolStripEx3.Location = new System.Drawing.Point( 14, 194 );
            this.toolStripEx3.Name = "toolStripEx3";
            this.toolStripEx3.ShowCaption = false;
            this.toolStripEx3.ShowLauncher = false;
            this.toolStripEx3.Size = new System.Drawing.Size( 169, 50 );
            this.toolStripEx3.TabIndex = 0;
            // 
            // btnAccept
            // 
            this.btnAccept.Image = ( ( System.Drawing.Image )( resources.GetObject( "btnAccept.Image" ) ) );
            this.btnAccept.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.btnAccept.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnAccept.Name = "btnAccept";
            this.btnAccept.Size = new System.Drawing.Size( 76, 43 );
            this.btnAccept.Text = "Accept";
            this.btnAccept.Click += new System.EventHandler( this.btnAccept_Click );
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Margin = new System.Windows.Forms.Padding( 10, 0, 10, 0 );
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size( 6, 46 );
            // 
            // btnCancel
            // 
            this.btnCancel.Image = ( ( System.Drawing.Image )( resources.GetObject( "btnCancel.Image" ) ) );
            this.btnCancel.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size( 59, 43 );
            this.btnCancel.Text = "Cancel";
            this.btnCancel.Click += new System.EventHandler( this.btnCancel_Click );
            // 
            // layoutChoosePackage
            // 
            this.layoutChoosePackage.BottomMargin = 3;
            this.layoutChoosePackage.ContainerControl = this.gradientPanelExt1;
            this.layoutChoosePackage.HorzNearMargin = 20;
            this.layoutChoosePackage.LayoutMode = Syncfusion.Windows.Forms.Tools.FlowLayoutMode.Vertical;
            this.layoutChoosePackage.VGap = 3;
            // 
            // layoutContent
            // 
            this.layoutContent.ContainerControl = this.panel1;
            // 
            // layoutNameModel
            // 
            this.layoutNameModel.BottomMargin = 3;
            this.layoutNameModel.ContainerControl = this.gradientPanelExt2;
            this.layoutNameModel.HorzNearMargin = 20;
            this.layoutNameModel.LayoutMode = Syncfusion.Windows.Forms.Tools.FlowLayoutMode.Vertical;
            this.layoutNameModel.VGap = 3;
            // 
            // ImportModelDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb( ( ( int )( ( ( byte )( 115 ) ) ) ), ( ( int )( ( ( byte )( 115 ) ) ) ), ( ( int )( ( ( byte )( 115 ) ) ) ) );
            this.ClientSize = new System.Drawing.Size( 423, 382 );
            this.ColorScheme = Syncfusion.Windows.Forms.Tools.RibbonForm.ColorSchemeType.Black;
            this.Controls.Add( this.panel1 );
            this.Controls.Add( this.ribbonControlAdv1 );
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "ImportModelDialog";
            this.Text = "Import Model";
            this.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler( this.ImportModelDialog_PreviewKeyDown );
            ( ( System.ComponentModel.ISupportInitialize )( this.ribbonControlAdv1 ) ).EndInit();
            this.ribbonControlAdv1.ResumeLayout( false );
            this.ribbonControlAdv1.PerformLayout();
            this.toolStripTabItem1.Panel.ResumeLayout( false );
            this.toolStripEx1.ResumeLayout( false );
            this.toolStripEx1.PerformLayout();
            this.toolStripEx2.ResumeLayout( false );
            this.toolStripEx2.PerformLayout();
            this.panel1.ResumeLayout( false );
            ( ( System.ComponentModel.ISupportInitialize )( this.gradientPanelExt2 ) ).EndInit();
            this.gradientPanelExt2.ResumeLayout( false );
            this.gradientPanelExt2.PerformLayout();
            ( ( System.ComponentModel.ISupportInitialize )( this.gradientPanelExt1 ) ).EndInit();
            this.gradientPanelExt1.ResumeLayout( false );
            this.gradientPanelExt1.PerformLayout();
            this.toolStripEx3.ResumeLayout( false );
            this.toolStripEx3.PerformLayout();
            ( ( System.ComponentModel.ISupportInitialize )( this.layoutChoosePackage ) ).EndInit();
            ( ( System.ComponentModel.ISupportInitialize )( this.layoutContent ) ).EndInit();
            ( ( System.ComponentModel.ISupportInitialize )( this.flowLayout1 ) ).EndInit();
            ( ( System.ComponentModel.ISupportInitialize )( this.layoutNameModel ) ).EndInit();
            this.ResumeLayout( false );

        }

        #endregion

        private Syncfusion.Windows.Forms.Tools.RibbonControlAdv ribbonControlAdv1;
        private System.Windows.Forms.Panel panel1;
        private Syncfusion.Windows.Forms.Tools.GradientPanelExt gradientPanelExt1;
        private System.Windows.Forms.Label label1;
        private Syncfusion.Windows.Forms.Tools.TextBoxExt txtPackageName;
        private System.Windows.Forms.Label lblPackageNameError;
        private Syncfusion.Windows.Forms.Tools.ToolStripEx toolStripEx3;
        private System.Windows.Forms.ToolStripButton btnAccept;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton btnCancel;
        private Syncfusion.Windows.Forms.Tools.ToolStripTabItem toolStripTabItem1;
        private Syncfusion.Windows.Forms.Tools.FlowLayout layoutChoosePackage;
        private Syncfusion.Windows.Forms.Tools.FlowLayout layoutContent;
        private Syncfusion.Windows.Forms.Tools.ToolStripEx toolStripEx1;
        private Syncfusion.Windows.Forms.Tools.ToolStripCheckBox cbImportAnimations;
        private Syncfusion.Windows.Forms.Tools.ToolStripEx toolStripEx2;
        private System.Windows.Forms.ToolStripButton btnBrowse;
        private Syncfusion.Windows.Forms.Tools.GradientPanelExt gradientPanelExt2;
        private System.Windows.Forms.Label label2;
        private Syncfusion.Windows.Forms.Tools.TextBoxExt txtModelName;
        private System.Windows.Forms.Label lblModelNameError;
        private Syncfusion.Windows.Forms.Tools.FlowLayout flowLayout1;
        private Syncfusion.Windows.Forms.Tools.FlowLayout layoutNameModel;
    }
}