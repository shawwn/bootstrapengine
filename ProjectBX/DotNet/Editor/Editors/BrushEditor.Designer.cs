namespace Editor.Editors
{
    partial class BrushEditor
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
            this.bnDiffuse = new Syncfusion.Windows.Forms.ButtonAdv();
            this.label7 = new System.Windows.Forms.Label();
            this.bnSpecular = new Syncfusion.Windows.Forms.ButtonAdv();
            this.label8 = new System.Windows.Forms.Label();
            this.bnBump = new Syncfusion.Windows.Forms.ButtonAdv();
            this.label5 = new System.Windows.Forms.Label();
            this.bnNormal = new Syncfusion.Windows.Forms.ButtonAdv();
            this.label6 = new System.Windows.Forms.Label();
            this.bnEmissive = new Syncfusion.Windows.Forms.ButtonAdv();
            this.label4 = new System.Windows.Forms.Label();
            this.bnAmbient = new Syncfusion.Windows.Forms.ButtonAdv();
            this.label3 = new System.Windows.Forms.Label();
            this.bnUser = new Syncfusion.Windows.Forms.ButtonAdv();
            this.label2 = new System.Windows.Forms.Label();
            this.pnControls = new System.Windows.Forms.GroupBox();
            this.lbBrushName = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.bnMask = new Syncfusion.Windows.Forms.ButtonAdv();
            this.pnControls.SuspendLayout();
            this.SuspendLayout();
            // 
            // bnDiffuse
            // 
            this.bnDiffuse.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnDiffuse.Location = new System.Drawing.Point( 96, 19 );
            this.bnDiffuse.Name = "bnDiffuse";
            this.bnDiffuse.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnDiffuse.Size = new System.Drawing.Size( 167, 23 );
            this.bnDiffuse.TabIndex = 28;
            this.bnDiffuse.Text = "{not set}";
            this.bnDiffuse.UseVisualStyle = true;
            this.bnDiffuse.Click += new System.EventHandler( this.bnTexture_Click );
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.ForeColor = System.Drawing.Color.White;
            this.label7.Location = new System.Drawing.Point( 61, 198 );
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size( 29, 13 );
            this.label7.TabIndex = 41;
            this.label7.Text = "User";
            this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // bnSpecular
            // 
            this.bnSpecular.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnSpecular.Location = new System.Drawing.Point( 96, 48 );
            this.bnSpecular.Name = "bnSpecular";
            this.bnSpecular.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnSpecular.Size = new System.Drawing.Size( 167, 23 );
            this.bnSpecular.TabIndex = 29;
            this.bnSpecular.Text = "{not set}";
            this.bnSpecular.UseVisualStyle = true;
            this.bnSpecular.Click += new System.EventHandler( this.bnTexture_Click );
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.ForeColor = System.Drawing.Color.White;
            this.label8.Location = new System.Drawing.Point( 45, 169 );
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size( 45, 13 );
            this.label8.TabIndex = 40;
            this.label8.Text = "Ambient";
            this.label8.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // bnBump
            // 
            this.bnBump.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnBump.Location = new System.Drawing.Point( 96, 106 );
            this.bnBump.Name = "bnBump";
            this.bnBump.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnBump.Size = new System.Drawing.Size( 167, 23 );
            this.bnBump.TabIndex = 30;
            this.bnBump.Text = "{not set}";
            this.bnBump.UseVisualStyle = true;
            this.bnBump.Click += new System.EventHandler( this.bnTexture_Click );
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.ForeColor = System.Drawing.Color.White;
            this.label5.Location = new System.Drawing.Point( 56, 111 );
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size( 34, 13 );
            this.label5.TabIndex = 39;
            this.label5.Text = "Bump";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // bnNormal
            // 
            this.bnNormal.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnNormal.Location = new System.Drawing.Point( 96, 77 );
            this.bnNormal.Name = "bnNormal";
            this.bnNormal.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnNormal.Size = new System.Drawing.Size( 167, 23 );
            this.bnNormal.TabIndex = 31;
            this.bnNormal.Text = "{not set}";
            this.bnNormal.UseVisualStyle = true;
            this.bnNormal.Click += new System.EventHandler( this.bnTexture_Click );
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.ForeColor = System.Drawing.Color.White;
            this.label6.Location = new System.Drawing.Point( 41, 53 );
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size( 49, 13 );
            this.label6.TabIndex = 38;
            this.label6.Text = "Specular";
            this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // bnEmissive
            // 
            this.bnEmissive.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnEmissive.Location = new System.Drawing.Point( 96, 135 );
            this.bnEmissive.Name = "bnEmissive";
            this.bnEmissive.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnEmissive.Size = new System.Drawing.Size( 167, 23 );
            this.bnEmissive.TabIndex = 32;
            this.bnEmissive.Text = "{not set}";
            this.bnEmissive.UseVisualStyle = true;
            this.bnEmissive.Click += new System.EventHandler( this.bnTexture_Click );
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.ForeColor = System.Drawing.Color.White;
            this.label4.Location = new System.Drawing.Point( 42, 140 );
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size( 48, 13 );
            this.label4.TabIndex = 37;
            this.label4.Text = "Emissive";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // bnAmbient
            // 
            this.bnAmbient.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnAmbient.Location = new System.Drawing.Point( 96, 164 );
            this.bnAmbient.Name = "bnAmbient";
            this.bnAmbient.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnAmbient.Size = new System.Drawing.Size( 167, 23 );
            this.bnAmbient.TabIndex = 33;
            this.bnAmbient.Text = "{not set}";
            this.bnAmbient.UseVisualStyle = true;
            this.bnAmbient.Click += new System.EventHandler( this.bnTexture_Click );
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.ForeColor = System.Drawing.Color.White;
            this.label3.Location = new System.Drawing.Point( 50, 82 );
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size( 40, 13 );
            this.label3.TabIndex = 36;
            this.label3.Text = "Normal";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // bnUser
            // 
            this.bnUser.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnUser.Location = new System.Drawing.Point( 96, 193 );
            this.bnUser.Name = "bnUser";
            this.bnUser.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnUser.Size = new System.Drawing.Size( 167, 23 );
            this.bnUser.TabIndex = 34;
            this.bnUser.Text = "{not set}";
            this.bnUser.UseVisualStyle = true;
            this.bnUser.Click += new System.EventHandler( this.bnTexture_Click );
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.ForeColor = System.Drawing.Color.White;
            this.label2.Location = new System.Drawing.Point( 50, 24 );
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size( 40, 13 );
            this.label2.TabIndex = 35;
            this.label2.Text = "Diffuse";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // pnControls
            // 
            this.pnControls.Controls.Add( this.bnDiffuse );
            this.pnControls.Controls.Add( this.label2 );
            this.pnControls.Controls.Add( this.bnMask );
            this.pnControls.Controls.Add( this.bnUser );
            this.pnControls.Controls.Add( this.label3 );
            this.pnControls.Controls.Add( this.label1 );
            this.pnControls.Controls.Add( this.label7 );
            this.pnControls.Controls.Add( this.bnAmbient );
            this.pnControls.Controls.Add( this.bnSpecular );
            this.pnControls.Controls.Add( this.label4 );
            this.pnControls.Controls.Add( this.label8 );
            this.pnControls.Controls.Add( this.bnEmissive );
            this.pnControls.Controls.Add( this.bnBump );
            this.pnControls.Controls.Add( this.label6 );
            this.pnControls.Controls.Add( this.label5 );
            this.pnControls.Controls.Add( this.bnNormal );
            this.pnControls.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pnControls.ForeColor = System.Drawing.Color.White;
            this.pnControls.Location = new System.Drawing.Point( 0, 35 );
            this.pnControls.Name = "pnControls";
            this.pnControls.Size = new System.Drawing.Size( 276, 262 );
            this.pnControls.TabIndex = 44;
            this.pnControls.TabStop = false;
            this.pnControls.Text = "Brush Parameters";
            // 
            // lbBrushName
            // 
            this.lbBrushName.AutoSize = true;
            this.lbBrushName.ForeColor = System.Drawing.Color.White;
            this.lbBrushName.Location = new System.Drawing.Point( 12, 11 );
            this.lbBrushName.Name = "lbBrushName";
            this.lbBrushName.Size = new System.Drawing.Size( 82, 13 );
            this.lbBrushName.TabIndex = 43;
            this.lbBrushName.Text = "{none selected}";
            this.lbBrushName.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.ForeColor = System.Drawing.Color.White;
            this.label1.Location = new System.Drawing.Point( 61, 241 );
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size( 33, 13 );
            this.label1.TabIndex = 41;
            this.label1.Text = "Mask";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // bnMask
            // 
            this.bnMask.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnMask.Location = new System.Drawing.Point( 96, 236 );
            this.bnMask.Name = "bnMask";
            this.bnMask.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnMask.Size = new System.Drawing.Size( 167, 23 );
            this.bnMask.TabIndex = 34;
            this.bnMask.Text = "{not set}";
            this.bnMask.UseVisualStyle = true;
            this.bnMask.Click += new System.EventHandler( this.bnTexture_Click );
            // 
            // BrushEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add( this.pnControls );
            this.Controls.Add( this.lbBrushName );
            this.Name = "BrushEditor";
            this.Size = new System.Drawing.Size( 276, 297 );
            this.pnControls.ResumeLayout( false );
            this.pnControls.PerformLayout();
            this.ResumeLayout( false );
            this.PerformLayout();

        }

        #endregion

        private Syncfusion.Windows.Forms.ButtonAdv bnDiffuse;
        private System.Windows.Forms.Label label7;
        private Syncfusion.Windows.Forms.ButtonAdv bnSpecular;
        private System.Windows.Forms.Label label8;
        private Syncfusion.Windows.Forms.ButtonAdv bnBump;
        private System.Windows.Forms.Label label5;
        private Syncfusion.Windows.Forms.ButtonAdv bnNormal;
        private System.Windows.Forms.Label label6;
        private Syncfusion.Windows.Forms.ButtonAdv bnEmissive;
        private System.Windows.Forms.Label label4;
        private Syncfusion.Windows.Forms.ButtonAdv bnAmbient;
        private System.Windows.Forms.Label label3;
        private Syncfusion.Windows.Forms.ButtonAdv bnUser;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.GroupBox pnControls;
        private System.Windows.Forms.Label lbBrushName;
        private Syncfusion.Windows.Forms.ButtonAdv bnMask;
        private System.Windows.Forms.Label label1;
    }
}
