namespace Editor.Dialogs
{
    partial class HDRColorDialog
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
            this.toolStripTabItem1 = new Syncfusion.Windows.Forms.Tools.ToolStripTabItem();
            this.saturationImageBox = new System.Windows.Forms.PictureBox();
            this.hueImageBox = new System.Windows.Forms.PictureBox();
            this.hueTextBox = new System.Windows.Forms.TextBox();
            this.saturationTextBox = new System.Windows.Forms.TextBox();
            this.intensityTextBox = new System.Windows.Forms.TextBox();
            this.redTextBox = new System.Windows.Forms.TextBox();
            this.greenTextBox = new System.Windows.Forms.TextBox();
            this.blueTextBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.intensityTrackBar = new System.Windows.Forms.TrackBar();
            this.intensityGrouper = new System.Windows.Forms.GroupBox();
            this.alphaTextBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.finalColorImageBox = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.saturationImageBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.hueImageBox)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.intensityTrackBar)).BeginInit();
            this.intensityGrouper.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.finalColorImageBox)).BeginInit();
            this.SuspendLayout();
            // 
            // toolStripTabItem1
            // 
            this.toolStripTabItem1.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(115)))), ((int)(((byte)(115)))), ((int)(((byte)(115)))));
            this.toolStripTabItem1.Name = "toolStripTabItem1";
            // 
            // 
            // 
            this.toolStripTabItem1.Panel.Name = "ribbonPanel1";
            this.toolStripTabItem1.Panel.ScrollPosition = 0;
            this.toolStripTabItem1.Panel.TabIndex = 2;
            this.toolStripTabItem1.Size = new System.Drawing.Size(23, 6);
            // 
            // saturationImageBox
            // 
            this.saturationImageBox.BackColor = System.Drawing.Color.Black;
            this.saturationImageBox.Location = new System.Drawing.Point(12, 12);
            this.saturationImageBox.Name = "saturationImageBox";
            this.saturationImageBox.Size = new System.Drawing.Size(32, 256);
            this.saturationImageBox.TabIndex = 1;
            this.saturationImageBox.TabStop = false;
            this.saturationImageBox.MouseMove += new System.Windows.Forms.MouseEventHandler(this.saturationImageBox_MouseEvent);
            this.saturationImageBox.MouseDown += new System.Windows.Forms.MouseEventHandler(this.saturationImageBox_MouseEvent);
            this.saturationImageBox.Paint += new System.Windows.Forms.PaintEventHandler(this.saturationImageBox_Paint);
            // 
            // hueImageBox
            // 
            this.hueImageBox.BackColor = System.Drawing.Color.Black;
            this.hueImageBox.Location = new System.Drawing.Point(50, 12);
            this.hueImageBox.Name = "hueImageBox";
            this.hueImageBox.Size = new System.Drawing.Size(32, 256);
            this.hueImageBox.TabIndex = 2;
            this.hueImageBox.TabStop = false;
            this.hueImageBox.MouseMove += new System.Windows.Forms.MouseEventHandler(this.hueImageBox_MouseEvent);
            this.hueImageBox.MouseDown += new System.Windows.Forms.MouseEventHandler(this.hueImageBox_MouseEvent);
            this.hueImageBox.Paint += new System.Windows.Forms.PaintEventHandler(this.hueImageBox_Paint);
            // 
            // hueTextBox
            // 
            this.hueTextBox.Location = new System.Drawing.Point(197, 12);
            this.hueTextBox.Name = "hueTextBox";
            this.hueTextBox.Size = new System.Drawing.Size(60, 20);
            this.hueTextBox.TabIndex = 3;
            this.hueTextBox.TextChanged += new System.EventHandler(this.hueTextBox_TextChanged);
            // 
            // saturationTextBox
            // 
            this.saturationTextBox.Location = new System.Drawing.Point(197, 38);
            this.saturationTextBox.Name = "saturationTextBox";
            this.saturationTextBox.Size = new System.Drawing.Size(60, 20);
            this.saturationTextBox.TabIndex = 4;
            this.saturationTextBox.TextChanged += new System.EventHandler(this.saturationTextBox_TextChanged);
            // 
            // intensityTextBox
            // 
            this.intensityTextBox.Location = new System.Drawing.Point(6, 19);
            this.intensityTextBox.Name = "intensityTextBox";
            this.intensityTextBox.Size = new System.Drawing.Size(60, 20);
            this.intensityTextBox.TabIndex = 5;
            this.intensityTextBox.TextChanged += new System.EventHandler(this.intensityTextBox_TextChanged);
            // 
            // redTextBox
            // 
            this.redTextBox.Location = new System.Drawing.Point(197, 82);
            this.redTextBox.Name = "redTextBox";
            this.redTextBox.Size = new System.Drawing.Size(60, 20);
            this.redTextBox.TabIndex = 6;
            this.redTextBox.TextChanged += new System.EventHandler(this.redTextBox_TextChanged);
            // 
            // greenTextBox
            // 
            this.greenTextBox.Location = new System.Drawing.Point(197, 108);
            this.greenTextBox.Name = "greenTextBox";
            this.greenTextBox.Size = new System.Drawing.Size(60, 20);
            this.greenTextBox.TabIndex = 7;
            this.greenTextBox.TextChanged += new System.EventHandler(this.greenTextBox_TextChanged);
            // 
            // blueTextBox
            // 
            this.blueTextBox.Location = new System.Drawing.Point(197, 134);
            this.blueTextBox.Name = "blueTextBox";
            this.blueTextBox.Size = new System.Drawing.Size(60, 20);
            this.blueTextBox.TabIndex = 8;
            this.blueTextBox.TextChanged += new System.EventHandler(this.blueTextBox_TextChanged);
            // 
            // label1
            // 
            this.label1.ForeColor = System.Drawing.Color.White;
            this.label1.Location = new System.Drawing.Point(136, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 23);
            this.label1.TabIndex = 9;
            this.label1.Text = "Hue";
            this.label1.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label2
            // 
            this.label2.ForeColor = System.Drawing.Color.White;
            this.label2.Location = new System.Drawing.Point(136, 41);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(55, 23);
            this.label2.TabIndex = 10;
            this.label2.Text = "Saturation";
            this.label2.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label4
            // 
            this.label4.ForeColor = System.Drawing.Color.White;
            this.label4.Location = new System.Drawing.Point(141, 85);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(50, 23);
            this.label4.TabIndex = 12;
            this.label4.Text = "Red";
            this.label4.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label5
            // 
            this.label5.ForeColor = System.Drawing.Color.White;
            this.label5.Location = new System.Drawing.Point(141, 111);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(50, 23);
            this.label5.TabIndex = 13;
            this.label5.Text = "Green";
            this.label5.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label6
            // 
            this.label6.ForeColor = System.Drawing.Color.White;
            this.label6.Location = new System.Drawing.Point(141, 137);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(50, 13);
            this.label6.TabIndex = 14;
            this.label6.Text = "Blue";
            this.label6.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // intensityTrackBar
            // 
            this.intensityTrackBar.AutoSize = false;
            this.intensityTrackBar.Location = new System.Drawing.Point(72, 19);
            this.intensityTrackBar.Maximum = 100;
            this.intensityTrackBar.Name = "intensityTrackBar";
            this.intensityTrackBar.Size = new System.Drawing.Size(118, 36);
            this.intensityTrackBar.TabIndex = 16;
            this.intensityTrackBar.Scroll += new System.EventHandler(this.intensityTrackBar_Scroll);
            // 
            // intensityGrouper
            // 
            this.intensityGrouper.Controls.Add(this.intensityTextBox);
            this.intensityGrouper.Controls.Add(this.intensityTrackBar);
            this.intensityGrouper.ForeColor = System.Drawing.Color.White;
            this.intensityGrouper.Location = new System.Drawing.Point(102, 207);
            this.intensityGrouper.Name = "intensityGrouper";
            this.intensityGrouper.Size = new System.Drawing.Size(192, 61);
            this.intensityGrouper.TabIndex = 17;
            this.intensityGrouper.TabStop = false;
            this.intensityGrouper.Text = "Intensity";
            // 
            // alphaTextBox
            // 
            this.alphaTextBox.Location = new System.Drawing.Point(197, 181);
            this.alphaTextBox.Name = "alphaTextBox";
            this.alphaTextBox.Size = new System.Drawing.Size(60, 20);
            this.alphaTextBox.TabIndex = 18;
            this.alphaTextBox.TextChanged += new System.EventHandler(this.alphaTextBox_TextChanged);
            // 
            // label3
            // 
            this.label3.ForeColor = System.Drawing.Color.White;
            this.label3.Location = new System.Drawing.Point(141, 184);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(50, 13);
            this.label3.TabIndex = 19;
            this.label3.Text = "Alpha";
            this.label3.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // finalColorImageBox
            // 
            this.finalColorImageBox.BackColor = System.Drawing.Color.Black;
            this.finalColorImageBox.Location = new System.Drawing.Point(103, 169);
            this.finalColorImageBox.Name = "finalColorImageBox";
            this.finalColorImageBox.Size = new System.Drawing.Size(32, 32);
            this.finalColorImageBox.TabIndex = 20;
            this.finalColorImageBox.TabStop = false;
            this.finalColorImageBox.Paint += new System.Windows.Forms.PaintEventHandler(this.finalColorImageBox_Paint);
            // 
            // HDRColorDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(73)))), ((int)(((byte)(73)))), ((int)(((byte)(73)))));
            this.ClientSize = new System.Drawing.Size(307, 282);
            this.ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.Controls.Add(this.finalColorImageBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.alphaTextBox);
            this.Controls.Add(this.intensityGrouper);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.blueTextBox);
            this.Controls.Add(this.greenTextBox);
            this.Controls.Add(this.redTextBox);
            this.Controls.Add(this.saturationTextBox);
            this.Controls.Add(this.hueTextBox);
            this.Controls.Add(this.hueImageBox);
            this.Controls.Add(this.saturationImageBox);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "HDRColorDialog";
            this.Text = "HDR Color Selector";
            this.TopMost = true;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.HDRColorDialog_FormClosing);
            ((System.ComponentModel.ISupportInitialize)(this.saturationImageBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.hueImageBox)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.intensityTrackBar)).EndInit();
            this.intensityGrouper.ResumeLayout(false);
            this.intensityGrouper.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.finalColorImageBox)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private Syncfusion.Windows.Forms.Tools.ToolStripTabItem toolStripTabItem1;
        private System.Windows.Forms.PictureBox saturationImageBox;
        private System.Windows.Forms.PictureBox hueImageBox;
        private System.Windows.Forms.TextBox hueTextBox;
        private System.Windows.Forms.TextBox saturationTextBox;
        private System.Windows.Forms.TextBox intensityTextBox;
        private System.Windows.Forms.TextBox redTextBox;
        private System.Windows.Forms.TextBox greenTextBox;
        private System.Windows.Forms.TextBox blueTextBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TrackBar intensityTrackBar;
        private System.Windows.Forms.GroupBox intensityGrouper;
        private System.Windows.Forms.TextBox alphaTextBox;
        private System.Windows.Forms.Label label3;
      private System.Windows.Forms.PictureBox finalColorImageBox;
    }
}