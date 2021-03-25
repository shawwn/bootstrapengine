namespace Editor.Controls
{
    partial class Thumbnail
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
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager( typeof( Thumbnail ) );
			this.pImageTarget = new Editor.Controls.Misc.DoubleBufferedPanel();
			this.lblCaption = new System.Windows.Forms.Label();
			this.pDelete = new Editor.Controls.Misc.DoubleBufferedPanel();
			this.SuspendLayout();
			// 
			// pImageTarget
			// 
			this.pImageTarget.Anchor = System.Windows.Forms.AnchorStyles.None;
			this.pImageTarget.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.pImageTarget.Location = new System.Drawing.Point( 15, 0 );
			this.pImageTarget.Name = "pImageTarget";
			this.pImageTarget.Size = new System.Drawing.Size( 64, 64 );
			this.pImageTarget.TabIndex = 2;
			this.pImageTarget.Paint += new System.Windows.Forms.PaintEventHandler( this.pImageTarget_Paint );
			// 
			// lblCaption
			// 
			this.lblCaption.AutoEllipsis = true;
			this.lblCaption.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.lblCaption.ForeColor = System.Drawing.Color.White;
			this.lblCaption.Location = new System.Drawing.Point( 0, 69 );
			this.lblCaption.Margin = new System.Windows.Forms.Padding( 0 );
			this.lblCaption.MaximumSize = new System.Drawing.Size( 100, 30 );
			this.lblCaption.Name = "lblCaption";
			this.lblCaption.Size = new System.Drawing.Size( 99, 30 );
			this.lblCaption.TabIndex = 1;
			this.lblCaption.Text = "namenamenamenamenamenamenamenamename";
			this.lblCaption.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			// 
			// pDelete
			// 
			this.pDelete.Anchor = System.Windows.Forms.AnchorStyles.None;
			this.pDelete.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.pDelete.BackgroundImage = ( ( System.Drawing.Image )( resources.GetObject( "pDelete.BackgroundImage" ) ) );
			this.pDelete.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
			this.pDelete.Location = new System.Drawing.Point( 82, 0 );
			this.pDelete.Margin = new System.Windows.Forms.Padding( 0 );
			this.pDelete.Name = "pDelete";
			this.pDelete.Size = new System.Drawing.Size( 14, 14 );
			this.pDelete.TabIndex = 2;
			this.pDelete.Click += new System.EventHandler( this.pDelete_Click );
			// 
			// Thumbnail
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.BackColor = System.Drawing.Color.Transparent;
			this.Controls.Add( this.pDelete );
			this.Controls.Add( this.pImageTarget );
			this.Controls.Add( this.lblCaption );
			this.Name = "Thumbnail";
			this.Size = new System.Drawing.Size( 99, 99 );
			this.ResumeLayout( false );

        }

        #endregion

		private System.Windows.Forms.Label lblCaption;
        private Editor.Controls.Misc.DoubleBufferedPanel pImageTarget;
		private Editor.Controls.Misc.DoubleBufferedPanel pDelete;



    }
}
