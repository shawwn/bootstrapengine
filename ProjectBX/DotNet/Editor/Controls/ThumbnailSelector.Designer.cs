namespace Editor.Controls
{
    partial class ThumbnailSelector
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
			this.components = new System.ComponentModel.Container();
			this.flowLayout1 = new Syncfusion.Windows.Forms.Tools.FlowLayout( this.components );
			( ( System.ComponentModel.ISupportInitialize )( this.flowLayout1 ) ).BeginInit();
			this.SuspendLayout();
			// 
			// flowLayout1
			// 
			this.flowLayout1.Alignment = Syncfusion.Windows.Forms.Tools.FlowAlignment.Near;
			this.flowLayout1.BottomMargin = 5;
			this.flowLayout1.ContainerControl = this;
			this.flowLayout1.HorzFarMargin = 5;
			this.flowLayout1.HorzNearMargin = 5;
			this.flowLayout1.TopMargin = 5;
			// 
			// ThumbnailSelector
			// 
			this.AutoScroll = true;
			this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
			this.BackColor = System.Drawing.Color.Black;
			this.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.Name = "ThumbnailSelector";
			this.Size = new System.Drawing.Size( 370, 308 );
			( ( System.ComponentModel.ISupportInitialize )( this.flowLayout1 ) ).EndInit();
			this.ResumeLayout( false );

        }

        #endregion

        private Syncfusion.Windows.Forms.Tools.FlowLayout flowLayout1;


    }
}
