namespace Editor
{
    partial class ImportSceneDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ImportSceneDialog));
            this.ribbonControlAdv1 = new Syncfusion.Windows.Forms.Tools.RibbonControlAdv();
            this.toolStripTabItem1 = new Syncfusion.Windows.Forms.Tools.ToolStripTabItem();
            this.toolStrip = new Syncfusion.Windows.Forms.Tools.ToolStripEx();
            this.cbOverwriteMaterials = new Syncfusion.Windows.Forms.Tools.ToolStripCheckBox();
            this.cbOverwriteLights = new Syncfusion.Windows.Forms.Tools.ToolStripCheckBox();
            this.toolStripEx2 = new Syncfusion.Windows.Forms.Tools.ToolStripEx();
            this.btnBrowse = new System.Windows.Forms.ToolStripButton();
            this.txtSceneName = new Syncfusion.Windows.Forms.Tools.TextBoxExt();
            this.label1 = new System.Windows.Forms.Label();
            this.toolStripEx3 = new Syncfusion.Windows.Forms.Tools.ToolStripEx();
            this.btnAccept = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.btnCancel = new System.Windows.Forms.ToolStripButton();
            this.gradientPanelExt1 = new Syncfusion.Windows.Forms.Tools.GradientPanelExt();
            this.lblError = new System.Windows.Forms.Label();
            this.layoutNameScene = new Syncfusion.Windows.Forms.Tools.FlowLayout(this.components);
            this.panel1 = new System.Windows.Forms.Panel();
            this.layoutContent = new Syncfusion.Windows.Forms.Tools.FlowLayout(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.ribbonControlAdv1)).BeginInit();
            this.ribbonControlAdv1.SuspendLayout();
            this.toolStripTabItem1.Panel.SuspendLayout();
            this.toolStrip.SuspendLayout();
            this.toolStripEx2.SuspendLayout();
            this.toolStripEx3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gradientPanelExt1)).BeginInit();
            this.gradientPanelExt1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.layoutNameScene)).BeginInit();
            this.panel1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.layoutContent)).BeginInit();
            this.SuspendLayout();
            // 
            // ribbonControlAdv1
            // 
            this.ribbonControlAdv1.AllowCollapse = false;
            this.ribbonControlAdv1.BorderStyle = Syncfusion.Windows.Forms.Tools.ToolStripBorderStyle.None;
            this.ribbonControlAdv1.CaptionMinHeight = 20;
            this.ribbonControlAdv1.Header.AddMainItem(toolStripTabItem1);
            this.ribbonControlAdv1.Location = new System.Drawing.Point(1, 0);
            this.ribbonControlAdv1.MenuButtonVisible = false;
            this.ribbonControlAdv1.Name = "ribbonControlAdv1";
            // 
            // ribbonControlAdv1.OfficeMenu
            // 
            this.ribbonControlAdv1.OfficeMenu.Name = "OfficeMenu";
            this.ribbonControlAdv1.OfficeMenu.Size = new System.Drawing.Size(12, 65);
            this.ribbonControlAdv1.QuickPanelVisible = false;
            this.ribbonControlAdv1.ShowCaption = false;
            this.ribbonControlAdv1.ShowLauncher = false;
            this.ribbonControlAdv1.Size = new System.Drawing.Size(421, 107);
            this.ribbonControlAdv1.SystemText.QuickAccessDialogDropDownName = "Start menu";
            this.ribbonControlAdv1.TabIndex = 0;
            this.ribbonControlAdv1.Text = "Import Scene";
            // 
            // toolStripTabItem1
            // 
            this.toolStripTabItem1.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.None;
            this.toolStripTabItem1.Name = "toolStripTabItem1";
            // 
            // ribbonControlAdv1.ribbonPanel1
            // 
            this.toolStripTabItem1.Panel.BorderStyle = Syncfusion.Windows.Forms.Tools.ToolStripBorderStyle.Etched;
            this.toolStripTabItem1.Panel.CaptionMinHeight = 10;
            this.toolStripTabItem1.Panel.Controls.Add(this.toolStrip);
            this.toolStripTabItem1.Panel.Controls.Add(this.toolStripEx2);
            this.toolStripTabItem1.Panel.Name = "ribbonPanel1";
            this.toolStripTabItem1.Panel.ScrollPosition = 0;
            this.toolStripTabItem1.Panel.TabIndex = 2;
            this.toolStripTabItem1.Panel.Text = "Import Scene";
            this.toolStripTabItem1.Size = new System.Drawing.Size(23, 6);
            this.toolStripTabItem1.Text = "Import Scene";
            // 
            // toolStrip
            // 
            this.toolStrip.AutoSize = false;
            this.toolStrip.CanOverflow = false;
            this.toolStrip.CaptionMinHeight = 20;
            this.toolStrip.CaptionStyle = Syncfusion.Windows.Forms.Tools.CaptionStyle.Top;
            this.toolStrip.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip.ForeColor = System.Drawing.Color.MidnightBlue;
            this.toolStrip.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStrip.Image = null;
            this.toolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.cbOverwriteMaterials,
            this.cbOverwriteLights});
            this.toolStrip.Location = new System.Drawing.Point(0, 1);
            this.toolStrip.Name = "toolStrip";
            this.toolStrip.Padding = new System.Windows.Forms.Padding(10, 0, 1, 0);
            this.toolStrip.ShowCaption = true;
            this.toolStrip.Size = new System.Drawing.Size(150, 46);
            this.toolStrip.TabIndex = 0;
            this.toolStrip.Text = "Overwrite Current?";
            // 
            // cbOverwriteMaterials
            // 
            this.cbOverwriteMaterials.Name = "cbOverwriteMaterials";
            this.cbOverwriteMaterials.Size = new System.Drawing.Size(68, 19);
            this.cbOverwriteMaterials.Text = "Materials";
            // 
            // cbOverwriteLights
            // 
            this.cbOverwriteLights.Name = "cbOverwriteLights";
            this.cbOverwriteLights.Size = new System.Drawing.Size(54, 19);
            this.cbOverwriteLights.Text = "Lights";
            // 
            // toolStripEx2
            // 
            this.toolStripEx2.AutoSize = false;
            this.toolStripEx2.CanOverflow = false;
            this.toolStripEx2.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStripEx2.ForeColor = System.Drawing.Color.MidnightBlue;
            this.toolStripEx2.GripStyle = System.Windows.Forms.ToolStripGripStyle.Hidden;
            this.toolStripEx2.Image = null;
            this.toolStripEx2.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnBrowse});
            this.toolStripEx2.Location = new System.Drawing.Point(150, 1);
            this.toolStripEx2.Name = "toolStripEx2";
            this.toolStripEx2.ShowCaption = false;
            this.toolStripEx2.Size = new System.Drawing.Size(258, 46);
            this.toolStripEx2.TabIndex = 1;
            this.toolStripEx2.Text = "Browse";
            // 
            // btnBrowse
            // 
            this.btnBrowse.AutoSize = false;
            this.btnBrowse.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Text;
            this.btnBrowse.Image = ((System.Drawing.Image)(resources.GetObject("btnBrowse.Image")));
            this.btnBrowse.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnBrowse.Name = "btnBrowse";
            this.btnBrowse.Size = new System.Drawing.Size(253, 39);
            this.btnBrowse.Text = "C:\\bin\\data\\metalroomnew\\example.dae";
            this.btnBrowse.ToolTipText = "Browse";
            this.btnBrowse.Click += new System.EventHandler(this.btnBrowse_Click);
            // 
            // txtSceneName
            // 
            this.txtSceneName.Location = new System.Drawing.Point(20, 24);
            this.txtSceneName.Margin = new System.Windows.Forms.Padding(5);
            this.txtSceneName.Name = "txtSceneName";
            this.txtSceneName.OverflowIndicatorToolTipText = null;
            this.txtSceneName.Size = new System.Drawing.Size(150, 20);
            this.txtSceneName.TabIndex = 3;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.ForeColor = System.Drawing.SystemColors.ActiveCaptionText;
            this.label1.Location = new System.Drawing.Point(43, 7);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(104, 14);
            this.label1.TabIndex = 2;
            this.label1.Text = "Name Your Scene";
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
            this.toolStripEx3.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.btnAccept,
            this.toolStripSeparator1,
            this.btnCancel});
            this.toolStripEx3.Location = new System.Drawing.Point(14, 119);
            this.toolStripEx3.Name = "toolStripEx3";
            this.toolStripEx3.ShowCaption = false;
            this.toolStripEx3.ShowLauncher = false;
            this.toolStripEx3.Size = new System.Drawing.Size(169, 50);
            this.toolStripEx3.TabIndex = 0;
            // 
            // btnAccept
            // 
            this.btnAccept.Image = ((System.Drawing.Image)(resources.GetObject("btnAccept.Image")));
            this.btnAccept.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.btnAccept.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnAccept.Name = "btnAccept";
            this.btnAccept.Size = new System.Drawing.Size(77, 43);
            this.btnAccept.Text = "Accept";
            this.btnAccept.Click += new System.EventHandler(this.btnAccept_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Margin = new System.Windows.Forms.Padding(10, 0, 10, 0);
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(6, 46);
            // 
            // btnCancel
            // 
            this.btnCancel.Image = ((System.Drawing.Image)(resources.GetObject("btnCancel.Image")));
            this.btnCancel.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(60, 43);
            this.btnCancel.Text = "Cancel";
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // gradientPanelExt1
            // 
            this.gradientPanelExt1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.gradientPanelExt1.BackColor = System.Drawing.Color.Transparent;
            this.gradientPanelExt1.BorderColor = System.Drawing.Color.Black;
            this.gradientPanelExt1.BorderGap = 2;
            this.gradientPanelExt1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.gradientPanelExt1.Controls.Add(this.label1);
            this.gradientPanelExt1.Controls.Add(this.txtSceneName);
            this.gradientPanelExt1.Controls.Add(this.lblError);
            this.gradientPanelExt1.CornerRadius = 8;
            this.gradientPanelExt1.ExpandLocation = new System.Drawing.Point(0, 0);
            this.gradientPanelExt1.ExpandSize = new System.Drawing.Size(0, 0);
            this.gradientPanelExt1.Location = new System.Drawing.Point(3, 7);
            this.gradientPanelExt1.Name = "gradientPanelExt1";
            this.gradientPanelExt1.Size = new System.Drawing.Size(192, 92);
            this.gradientPanelExt1.TabIndex = 6;
            // 
            // lblError
            // 
            this.lblError.Font = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Bold);
            this.lblError.ForeColor = System.Drawing.Color.Maroon;
            this.lblError.Location = new System.Drawing.Point(28, 47);
            this.lblError.Name = "lblError";
            this.lblError.Size = new System.Drawing.Size(133, 34);
            this.lblError.TabIndex = 4;
            this.lblError.Text = "error";
            this.lblError.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.lblError.Visible = false;
            // 
            // layoutNameScene
            // 
            this.layoutNameScene.BottomMargin = 3;
            this.layoutNameScene.ContainerControl = this.gradientPanelExt1;
            this.layoutNameScene.HorzNearMargin = 20;
            this.layoutNameScene.LayoutMode = Syncfusion.Windows.Forms.Tools.FlowLayoutMode.Vertical;
            this.layoutNameScene.VGap = 3;
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.Controls.Add(this.gradientPanelExt1);
            this.panel1.Controls.Add(this.toolStripEx3);
            this.panel1.Location = new System.Drawing.Point(119, 113);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(198, 176);
            this.panel1.TabIndex = 7;
            // 
            // layoutContent
            // 
            this.layoutContent.ContainerControl = this.panel1;
            this.layoutContent.HorzNearMargin = 3;
            this.layoutContent.LayoutMode = Syncfusion.Windows.Forms.Tools.FlowLayoutMode.Vertical;
            this.layoutContent.VGap = 20;
            // 
            // ImportSceneDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(113)))), ((int)(((byte)(113)))), ((int)(((byte)(113)))));
            this.ClientSize = new System.Drawing.Size(423, 295);
            this.ColorScheme = Syncfusion.Windows.Forms.Tools.RibbonForm.ColorSchemeType.Black;
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.ribbonControlAdv1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "ImportSceneDialog";
            this.Text = "Import Scene";
            this.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.ImportSceneDialog_PreviewKeyDown);
            ((System.ComponentModel.ISupportInitialize)(this.ribbonControlAdv1)).EndInit();
            this.ribbonControlAdv1.ResumeLayout(false);
            this.ribbonControlAdv1.PerformLayout();
            this.toolStripTabItem1.Panel.ResumeLayout(false);
            this.toolStrip.ResumeLayout(false);
            this.toolStrip.PerformLayout();
            this.toolStripEx2.ResumeLayout(false);
            this.toolStripEx2.PerformLayout();
            this.toolStripEx3.ResumeLayout(false);
            this.toolStripEx3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gradientPanelExt1)).EndInit();
            this.gradientPanelExt1.ResumeLayout(false);
            this.gradientPanelExt1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.layoutNameScene)).EndInit();
            this.panel1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.layoutContent)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private Syncfusion.Windows.Forms.Tools.RibbonControlAdv ribbonControlAdv1;
        private Syncfusion.Windows.Forms.Tools.ToolStripTabItem toolStripTabItem1;
        private Syncfusion.Windows.Forms.Tools.ToolStripEx toolStripEx2;
        private System.Windows.Forms.ToolStripButton btnBrowse;
        private Syncfusion.Windows.Forms.Tools.TextBoxExt txtSceneName;
        private System.Windows.Forms.Label label1;
        private Syncfusion.Windows.Forms.Tools.ToolStripEx toolStripEx3;
        private System.Windows.Forms.ToolStripButton btnAccept;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripButton btnCancel;
        private Syncfusion.Windows.Forms.Tools.ToolStripCheckBox cbOverwriteLights;
        private Syncfusion.Windows.Forms.Tools.GradientPanelExt gradientPanelExt1;
        private Syncfusion.Windows.Forms.Tools.FlowLayout layoutNameScene;
        public Syncfusion.Windows.Forms.Tools.ToolStripEx toolStrip;
        public Syncfusion.Windows.Forms.Tools.ToolStripCheckBox cbOverwriteMaterials;
        private System.Windows.Forms.Panel panel1;
        private Syncfusion.Windows.Forms.Tools.FlowLayout layoutContent;
        private System.Windows.Forms.Label lblError;

    }
}