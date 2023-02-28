namespace Editor.Dialogs
{
    partial class DetailCastDialog
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
            this.tbBumpScale = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tbRayLength = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.bnSelectFile = new Syncfusion.Windows.Forms.ButtonAdv();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.cbSmoothSource = new System.Windows.Forms.CheckBox();
            this.rbCastBoth = new System.Windows.Forms.RadioButton();
            this.rbCastNormal = new System.Windows.Forms.RadioButton();
            this.rbCastBump = new System.Windows.Forms.RadioButton();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // bnCancel
            // 
            this.bnCancel.Location = new System.Drawing.Point(252, 181);
            // 
            // bnOK
            // 
            this.bnOK.Location = new System.Drawing.Point(171, 181);
            this.bnOK.Click += new System.EventHandler(this.bnOK_Click);
            // 
            // tbBumpScale
            // 
            this.tbBumpScale.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tbBumpScale.Location = new System.Drawing.Point(240, 19);
            this.tbBumpScale.Name = "tbBumpScale";
            this.tbBumpScale.Size = new System.Drawing.Size(69, 20);
            this.tbBumpScale.TabIndex = 12;
            this.tbBumpScale.Text = "1.0";
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(170, 22);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(64, 13);
            this.label1.TabIndex = 13;
            this.label1.Text = "Bump Scale";
            // 
            // tbRayLength
            // 
            this.tbRayLength.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tbRayLength.Location = new System.Drawing.Point(240, 45);
            this.tbRayLength.Name = "tbRayLength";
            this.tbRayLength.Size = new System.Drawing.Size(69, 20);
            this.tbRayLength.TabIndex = 14;
            this.tbRayLength.Text = "10.0";
            // 
            // label2
            // 
            this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(172, 48);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(62, 13);
            this.label2.TabIndex = 15;
            this.label2.Text = "Ray Length";
            // 
            // bnSelectFile
            // 
            this.bnSelectFile.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.bnSelectFile.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnSelectFile.Location = new System.Drawing.Point(12, 13);
            this.bnSelectFile.Name = "bnSelectFile";
            this.bnSelectFile.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnSelectFile.Size = new System.Drawing.Size(315, 23);
            this.bnSelectFile.TabIndex = 16;
            this.bnSelectFile.Text = "(click here to select the high-poly detail file)";
            this.bnSelectFile.UseVisualStyle = true;
            this.bnSelectFile.Click += new System.EventHandler(this.buttonAdv1_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.cbSmoothSource);
            this.groupBox1.Controls.Add(this.rbCastBoth);
            this.groupBox1.Controls.Add(this.rbCastNormal);
            this.groupBox1.Controls.Add(this.rbCastBump);
            this.groupBox1.Controls.Add(this.tbBumpScale);
            this.groupBox1.Controls.Add(this.tbRayLength);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(12, 42);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(315, 127);
            this.groupBox1.TabIndex = 17;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Options";
            // 
            // cbSmoothSource
            // 
            this.cbSmoothSource.AutoSize = true;
            this.cbSmoothSource.Checked = true;
            this.cbSmoothSource.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbSmoothSource.Location = new System.Drawing.Point(6, 98);
            this.cbSmoothSource.Name = "cbSmoothSource";
            this.cbSmoothSource.Size = new System.Drawing.Size(99, 17);
            this.cbSmoothSource.TabIndex = 18;
            this.cbSmoothSource.Text = "Smooth Source";
            this.cbSmoothSource.UseVisualStyleBackColor = true;
            // 
            // rbCastBoth
            // 
            this.rbCastBoth.AutoSize = true;
            this.rbCastBoth.Checked = true;
            this.rbCastBoth.Location = new System.Drawing.Point(6, 72);
            this.rbCastBoth.Name = "rbCastBoth";
            this.rbCastBoth.Size = new System.Drawing.Size(134, 17);
            this.rbCastBoth.TabIndex = 17;
            this.rbCastBoth.TabStop = true;
            this.rbCastBoth.Text = "Write bump and normal";
            this.rbCastBoth.UseVisualStyleBackColor = true;
            this.rbCastBoth.CheckedChanged += new System.EventHandler(this.rbCastBoth_CheckedChanged);
            // 
            // rbCastNormal
            // 
            this.rbCastNormal.AutoSize = true;
            this.rbCastNormal.Location = new System.Drawing.Point(6, 46);
            this.rbCastNormal.Name = "rbCastNormal";
            this.rbCastNormal.Size = new System.Drawing.Size(84, 17);
            this.rbCastNormal.TabIndex = 16;
            this.rbCastNormal.Text = "Write normal";
            this.rbCastNormal.UseVisualStyleBackColor = true;
            this.rbCastNormal.CheckedChanged += new System.EventHandler(this.rbCastNormal_CheckedChanged);
            // 
            // rbCastBump
            // 
            this.rbCastBump.AutoSize = true;
            this.rbCastBump.Location = new System.Drawing.Point(6, 20);
            this.rbCastBump.Name = "rbCastBump";
            this.rbCastBump.Size = new System.Drawing.Size(79, 17);
            this.rbCastBump.TabIndex = 0;
            this.rbCastBump.Text = "Write bump";
            this.rbCastBump.UseVisualStyleBackColor = true;
            this.rbCastBump.CheckedChanged += new System.EventHandler(this.rbCastBump_CheckedChanged);
            // 
            // DetailCastDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(339, 216);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.bnSelectFile);
            this.Name = "DetailCastDialog";
            this.Text = "Cast Options...";
            this.Controls.SetChildIndex(this.bnOK, 0);
            this.Controls.SetChildIndex(this.bnCancel, 0);
            this.Controls.SetChildIndex(this.bnSelectFile, 0);
            this.Controls.SetChildIndex(this.groupBox1, 0);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TextBox tbBumpScale;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbRayLength;
        private System.Windows.Forms.Label label2;
        private Syncfusion.Windows.Forms.ButtonAdv bnSelectFile;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton rbCastNormal;
        private System.Windows.Forms.RadioButton rbCastBump;
        private System.Windows.Forms.RadioButton rbCastBoth;
        private System.Windows.Forms.CheckBox cbSmoothSource;
    }
}