namespace Editor.Dialogs
{
    partial class BaseDialog
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
            this.bnCancel = new Syncfusion.Windows.Forms.ButtonAdv();
            this.bnOK = new Syncfusion.Windows.Forms.ButtonAdv();
            this.SuspendLayout();
            // 
            // bnCancel
            // 
            this.bnCancel.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.bnCancel.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.bnCancel.Location = new System.Drawing.Point( 194, 69 );
            this.bnCancel.Name = "bnCancel";
            this.bnCancel.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnCancel.Size = new System.Drawing.Size( 75, 23 );
            this.bnCancel.TabIndex = 11;
            this.bnCancel.Text = "Cancel";
            this.bnCancel.UseVisualStyle = true;
            // 
            // bnOK
            // 
            this.bnOK.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.bnOK.Appearance = Syncfusion.Windows.Forms.ButtonAppearance.Office2007;
            this.bnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.bnOK.Location = new System.Drawing.Point( 104, 69 );
            this.bnOK.Name = "bnOK";
            this.bnOK.Office2007ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.bnOK.Size = new System.Drawing.Size( 75, 23 );
            this.bnOK.TabIndex = 10;
            this.bnOK.Text = "OK";
            this.bnOK.UseVisualStyle = true;
            // 
            // BaseDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.FromArgb( ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ), ( ( int )( ( ( byte )( 73 ) ) ) ) );
            this.ClientSize = new System.Drawing.Size( 278, 104 );
            this.ColorScheme = Syncfusion.Windows.Forms.Office2007Theme.Black;
            this.Controls.Add( this.bnCancel );
            this.Controls.Add( this.bnOK );
            this.Name = "BaseDialog";
            this.ResumeLayout( false );

        }

        #endregion

        protected Syncfusion.Windows.Forms.ButtonAdv bnCancel;
        protected Syncfusion.Windows.Forms.ButtonAdv bnOK;

    }
}