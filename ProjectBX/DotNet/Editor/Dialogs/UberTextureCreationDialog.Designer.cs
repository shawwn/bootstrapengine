namespace Editor.Dialogs
{
    partial class UberTextureCreationDialog
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
            this.cbWidth = new System.Windows.Forms.ComboBox();
            this.cbHeight = new System.Windows.Forms.ComboBox();
            this.tbName = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // bnCancel
            // 
            this.bnCancel.Location = new System.Drawing.Point( 139, 101 );
            this.bnCancel.TabIndex = 4;
            // 
            // bnOK
            // 
            this.bnOK.Location = new System.Drawing.Point( 49, 101 );
            this.bnOK.TabIndex = 3;
            this.bnOK.Click += new System.EventHandler( this.bnOK_Click );
            // 
            // cbWidth
            // 
            this.cbWidth.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.cbWidth.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbWidth.FormattingEnabled = true;
            this.cbWidth.Items.AddRange( new object[] {
            "512",
            "1024",
            "2048",
            "4096",
            "8192",
            "16384",
            "32768",
            "65536",
            "131072"} );
            this.cbWidth.Location = new System.Drawing.Point( 92, 38 );
            this.cbWidth.MaxDropDownItems = 9;
            this.cbWidth.Name = "cbWidth";
            this.cbWidth.Size = new System.Drawing.Size( 121, 21 );
            this.cbWidth.TabIndex = 1;
            // 
            // cbHeight
            // 
            this.cbHeight.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.cbHeight.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbHeight.FormattingEnabled = true;
            this.cbHeight.Items.AddRange( new object[] {
            "512",
            "1024",
            "2048",
            "4096",
            "8192",
            "16384",
            "32768",
            "65536",
            "131072"} );
            this.cbHeight.Location = new System.Drawing.Point( 92, 65 );
            this.cbHeight.Name = "cbHeight";
            this.cbHeight.Size = new System.Drawing.Size( 121, 21 );
            this.cbHeight.TabIndex = 2;
            // 
            // tbName
            // 
            this.tbName.Anchor = ( ( System.Windows.Forms.AnchorStyles )( ( System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right ) ) );
            this.tbName.Location = new System.Drawing.Point( 92, 12 );
            this.tbName.Name = "tbName";
            this.tbName.Size = new System.Drawing.Size( 122, 20 );
            this.tbName.TabIndex = 0;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point( 51, 15 );
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size( 35, 13 );
            this.label1.TabIndex = 15;
            this.label1.Text = "Name";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point( 51, 41 );
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size( 35, 13 );
            this.label2.TabIndex = 16;
            this.label2.Text = "Width";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point( 48, 68 );
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size( 38, 13 );
            this.label3.TabIndex = 17;
            this.label3.Text = "Height";
            // 
            // UberTextureCreationDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size( 227, 141 );
            this.Controls.Add( this.label3 );
            this.Controls.Add( this.label2 );
            this.Controls.Add( this.label1 );
            this.Controls.Add( this.tbName );
            this.Controls.Add( this.cbHeight );
            this.Controls.Add( this.cbWidth );
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "UberTextureCreationDialog";
            this.Text = "UberTextureCreationDialog";
            this.Controls.SetChildIndex( this.cbWidth, 0 );
            this.Controls.SetChildIndex( this.bnOK, 0 );
            this.Controls.SetChildIndex( this.bnCancel, 0 );
            this.Controls.SetChildIndex( this.cbHeight, 0 );
            this.Controls.SetChildIndex( this.tbName, 0 );
            this.Controls.SetChildIndex( this.label1, 0 );
            this.Controls.SetChildIndex( this.label2, 0 );
            this.Controls.SetChildIndex( this.label3, 0 );
            this.ResumeLayout( false );
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        public System.Windows.Forms.ComboBox cbWidth;
        public System.Windows.Forms.ComboBox cbHeight;
        public System.Windows.Forms.TextBox tbName;
    }
}