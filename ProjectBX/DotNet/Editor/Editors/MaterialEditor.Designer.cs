namespace Editor.Editors
{
    partial class MaterialEditor
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.pbPreview = new System.Windows.Forms.PictureBox();
            this.pnGroupBarPanel = new System.Windows.Forms.Panel();
            this.gbPasses = new Syncfusion.Windows.Forms.Tools.GroupBar();
            this.panel2 = new System.Windows.Forms.Panel();
            this.groupBarItem1 = new Syncfusion.Windows.Forms.Tools.GroupBarItem();
            this.label4 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.cbStipple = new Syncfusion.Windows.Forms.Tools.CheckBoxAdv();
            this.lbName = new System.Windows.Forms.Label();
            this.materialPassControlGroup1 = new Editor.Controls.MaterialPassControlGroup();
            ((System.ComponentModel.ISupportInitialize)(this.pbPreview)).BeginInit();
            this.pnGroupBarPanel.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.gbPasses)).BeginInit();
            this.gbPasses.SuspendLayout();
            this.panel2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.cbStipple)).BeginInit();
            this.SuspendLayout();
            // 
            // pbPreview
            // 
            this.pbPreview.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.pbPreview.BackColor = System.Drawing.Color.Black;
            this.pbPreview.Location = new System.Drawing.Point(73, 22);
            this.pbPreview.Name = "pbPreview";
            this.pbPreview.Size = new System.Drawing.Size(128, 128);
            this.pbPreview.TabIndex = 11;
            this.pbPreview.TabStop = false;
            // 
            // pnGroupBarPanel
            // 
            this.pnGroupBarPanel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.pnGroupBarPanel.AutoScroll = true;
            this.pnGroupBarPanel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.pnGroupBarPanel.Controls.Add(this.gbPasses);
            this.pnGroupBarPanel.Location = new System.Drawing.Point(3, 214);
            this.pnGroupBarPanel.Name = "pnGroupBarPanel";
            this.pnGroupBarPanel.Size = new System.Drawing.Size(270, 283);
            this.pnGroupBarPanel.TabIndex = 10;
            // 
            // gbPasses
            // 
            this.gbPasses.AllowDrop = true;
            this.gbPasses.Controls.Add(this.panel2);
            this.gbPasses.GroupBarItems.AddRange(new Syncfusion.Windows.Forms.Tools.GroupBarItem[] {
            this.groupBarItem1});
            this.gbPasses.Location = new System.Drawing.Point(0, 0);
            this.gbPasses.MinimumSize = new System.Drawing.Size(250, 1100);
            this.gbPasses.Name = "gbPasses";
            this.gbPasses.SelectedItem = 0;
            this.gbPasses.Size = new System.Drawing.Size(250, 1100);
            this.gbPasses.TabIndex = 0;
            this.gbPasses.Text = "gbPassGroup";
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.materialPassControlGroup1);
            this.panel2.Location = new System.Drawing.Point(2, 24);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(246, 1074);
            this.panel2.TabIndex = 0;
            // 
            // groupBarItem1
            // 
            this.groupBarItem1.Client = this.panel2;
            this.groupBarItem1.Text = "GroupBarItem0";
            // 
            // label4
            // 
            this.label4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label4.AutoSize = true;
            this.label4.ForeColor = System.Drawing.Color.Yellow;
            this.label4.Location = new System.Drawing.Point(116, 153);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(45, 13);
            this.label4.TabIndex = 12;
            this.label4.Text = "Preview";
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.BackColor = System.Drawing.Color.Transparent;
            this.groupBox1.Controls.Add(this.cbStipple);
            this.groupBox1.ForeColor = System.Drawing.Color.White;
            this.groupBox1.Location = new System.Drawing.Point(3, 169);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(270, 39);
            this.groupBox1.TabIndex = 9;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Render States";
            // 
            // cbStipple
            // 
            this.cbStipple.BorderColor = System.Drawing.SystemColors.WindowFrame;
            this.cbStipple.ForeColor = System.Drawing.Color.White;
            this.cbStipple.GradientEnd = System.Drawing.SystemColors.ControlDark;
            this.cbStipple.GradientStart = System.Drawing.SystemColors.Control;
            this.cbStipple.HotBorderColor = System.Drawing.SystemColors.WindowFrame;
            this.cbStipple.ImageCheckBoxSize = new System.Drawing.Size(13, 13);
            this.cbStipple.Location = new System.Drawing.Point(8, 12);
            this.cbStipple.Name = "cbStipple";
            this.cbStipple.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.cbStipple.ShadowColor = System.Drawing.Color.Black;
            this.cbStipple.ShadowOffset = new System.Drawing.Point(2, 2);
            this.cbStipple.Size = new System.Drawing.Size(150, 21);
            this.cbStipple.TabIndex = 0;
            this.cbStipple.Text = "Stipple";
            this.cbStipple.ThemesEnabled = false;
            this.cbStipple.CheckStateChanged += new System.EventHandler(this.cbStipple_CheckStateChanged);
            // 
            // lbName
            // 
            this.lbName.AutoSize = true;
            this.lbName.Location = new System.Drawing.Point(4, 4);
            this.lbName.Name = "lbName";
            this.lbName.Size = new System.Drawing.Size(35, 13);
            this.lbName.TabIndex = 13;
            this.lbName.Text = "label1";
            // 
            // materialPassControlGroup1
            // 
            this.materialPassControlGroup1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(73)))), ((int)(((byte)(73)))), ((int)(((byte)(73)))));
            this.materialPassControlGroup1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.materialPassControlGroup1.ForeColor = System.Drawing.Color.Yellow;
            this.materialPassControlGroup1.Location = new System.Drawing.Point(0, 0);
            this.materialPassControlGroup1.Name = "materialPassControlGroup1";
            this.materialPassControlGroup1.Size = new System.Drawing.Size(246, 1074);
            this.materialPassControlGroup1.TabIndex = 0;
            // 
            // MaterialEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.lbName);
            this.Controls.Add(this.pbPreview);
            this.Controls.Add(this.pnGroupBarPanel);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.groupBox1);
            this.MinimumSize = new System.Drawing.Size(276, 400);
            this.Name = "MaterialEditor";
            this.Size = new System.Drawing.Size(276, 500);
            ((System.ComponentModel.ISupportInitialize)(this.pbPreview)).EndInit();
            this.pnGroupBarPanel.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.gbPasses)).EndInit();
            this.gbPasses.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.cbStipple)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pbPreview;
        private System.Windows.Forms.Panel pnGroupBarPanel;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.GroupBox groupBox1;
        private Syncfusion.Windows.Forms.Tools.CheckBoxAdv cbStipple;
        private Syncfusion.Windows.Forms.Tools.GroupBar gbPasses;
        private System.Windows.Forms.Panel panel2;
        private Editor.Controls.MaterialPassControlGroup materialPassControlGroup1;
        private Syncfusion.Windows.Forms.Tools.GroupBarItem groupBarItem1;
        private System.Windows.Forms.Label lbName;

    }
}
