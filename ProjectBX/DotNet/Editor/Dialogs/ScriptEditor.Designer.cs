namespace Editor.Dialogs
{
    partial class ScriptEditor
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
            this.tbScript = new System.Windows.Forms.TextBox();
            this.btnCompile = new Syncfusion.Windows.Forms.ButtonAdv();
            this.btnOK = new Syncfusion.Windows.Forms.ButtonAdv();
            this.tbErrors = new System.Windows.Forms.TextBox();
            this.btnCancel = new Syncfusion.Windows.Forms.ButtonAdv();
            this.SuspendLayout();
            // 
            // tbScript
            // 
            this.tbScript.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tbScript.Location = new System.Drawing.Point(13, 13);
            this.tbScript.Multiline = true;
            this.tbScript.Name = "tbScript";
            this.tbScript.Size = new System.Drawing.Size(329, 60);
            this.tbScript.TabIndex = 0;
            // 
            // btnCompile
            // 
            this.btnCompile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnCompile.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.btnCompile.Location = new System.Drawing.Point(13, 106);
            this.btnCompile.Name = "btnCompile";
            this.btnCompile.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.btnCompile.Size = new System.Drawing.Size(90, 23);
            this.btnCompile.TabIndex = 1;
            this.btnCompile.Text = "Compile";
            this.btnCompile.UseVisualStyle = true;
            this.btnCompile.Click += new System.EventHandler(this.btnCompile_Click);
            // 
            // btnOK
            // 
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Location = new System.Drawing.Point(168, 106);
            this.btnOK.Name = "btnOK";
            this.btnOK.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.btnOK.Size = new System.Drawing.Size(84, 23);
            this.btnOK.TabIndex = 2;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyle = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // tbErrors
            // 
            this.tbErrors.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tbErrors.Location = new System.Drawing.Point(13, 80);
            this.tbErrors.Name = "tbErrors";
            this.tbErrors.ReadOnly = true;
            this.tbErrors.Size = new System.Drawing.Size(329, 20);
            this.tbErrors.TabIndex = 3;
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(258, 106);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.btnCancel.Size = new System.Drawing.Size(84, 23);
            this.btnCancel.TabIndex = 4;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyle = true;
            // 
            // ScriptEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(354, 139);
            this.ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.tbErrors);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.btnCompile);
            this.Controls.Add(this.tbScript);
            this.MinimumSize = new System.Drawing.Size(320, 131);
            this.Name = "ScriptEditor";
            this.Text = "ScriptEditor";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tbScript;
        private Syncfusion.Windows.Forms.ButtonAdv btnCompile;
        private Syncfusion.Windows.Forms.ButtonAdv btnOK;
        private System.Windows.Forms.TextBox tbErrors;
        private Syncfusion.Windows.Forms.ButtonAdv btnCancel;
    }
}