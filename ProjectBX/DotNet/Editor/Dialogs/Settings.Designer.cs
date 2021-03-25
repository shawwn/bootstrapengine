namespace Editor.Dialogs
{
    partial class Settings
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
            this.bnWorkingFolder = new Syncfusion.Windows.Forms.ButtonAdv();
            this.bnBrushLibrary = new Syncfusion.Windows.Forms.ButtonAdv();
            this.bnOK = new Syncfusion.Windows.Forms.ButtonAdv();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // bnWorkingFolder
            // 
            this.bnWorkingFolder.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnWorkingFolder.Location = new System.Drawing.Point(6, 19);
            this.bnWorkingFolder.Name = "bnWorkingFolder";
            this.bnWorkingFolder.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnWorkingFolder.Size = new System.Drawing.Size(435, 23);
            this.bnWorkingFolder.TabIndex = 0;
            this.bnWorkingFolder.Text = "Internal error";
            this.bnWorkingFolder.UseVisualStyle = true;
            // 
            // bnBrushLibrary
            // 
            this.bnBrushLibrary.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnBrushLibrary.Location = new System.Drawing.Point(7, 19);
            this.bnBrushLibrary.Name = "bnBrushLibrary";
            this.bnBrushLibrary.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnBrushLibrary.Size = new System.Drawing.Size(435, 23);
            this.bnBrushLibrary.TabIndex = 1;
            this.bnBrushLibrary.Text = "Internal error";
            this.bnBrushLibrary.UseVisualStyle = true;
            // 
            // bnOK
            // 
            this.bnOK.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.bnOK.Location = new System.Drawing.Point(385, 138);
            this.bnOK.Name = "bnOK";
            this.bnOK.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnOK.Size = new System.Drawing.Size(75, 23);
            this.bnOK.TabIndex = 3;
            this.bnOK.Text = "OK";
            this.bnOK.UseVisualStyle = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.bnWorkingFolder);
            this.groupBox1.Location = new System.Drawing.Point(13, 13);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(447, 52);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Working Folder";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.bnBrushLibrary);
            this.groupBox2.Location = new System.Drawing.Point(12, 71);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(448, 52);
            this.groupBox2.TabIndex = 5;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Brush Library Root";
            // 
            // Settings
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(73)))), ((int)(((byte)(73)))), ((int)(((byte)(73)))));
            this.ClientSize = new System.Drawing.Size(472, 173);
            this.ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.bnOK);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "Settings";
            this.Text = "Settings";
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private Syncfusion.Windows.Forms.ButtonAdv bnWorkingFolder;
        private Syncfusion.Windows.Forms.ButtonAdv bnBrushLibrary;
        private Syncfusion.Windows.Forms.ButtonAdv bnOK;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
    }
}