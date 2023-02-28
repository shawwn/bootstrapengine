namespace Editor.Controls
{
    partial class TransformControl
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
            this.gbTransform = new System.Windows.Forms.GroupBox();
            this.veScale = new Editor.Controls.VectorEditor();
            this.veRotate = new Editor.Controls.VectorEditor();
            this.veTranslate = new Editor.Controls.VectorEditor();
            this.lblScale = new System.Windows.Forms.Label();
            this.lblRotate = new System.Windows.Forms.Label();
            this.lblTranslate = new System.Windows.Forms.Label();
            this.gbTransform.SuspendLayout();
            this.SuspendLayout();
            // 
            // gbTransform
            // 
            this.gbTransform.Controls.Add( this.veScale );
            this.gbTransform.Controls.Add( this.veRotate );
            this.gbTransform.Controls.Add( this.veTranslate );
            this.gbTransform.Controls.Add( this.lblScale );
            this.gbTransform.Controls.Add( this.lblRotate );
            this.gbTransform.Controls.Add( this.lblTranslate );
            this.gbTransform.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gbTransform.ForeColor = System.Drawing.SystemColors.ControlText;
            this.gbTransform.Location = new System.Drawing.Point( 0, 0 );
            this.gbTransform.Name = "gbTransform";
            this.gbTransform.Size = new System.Drawing.Size( 270, 136 );
            this.gbTransform.TabIndex = 15;
            this.gbTransform.TabStop = false;
            this.gbTransform.Text = "Transform Attributes";
            // 
            // veScale
            // 
            this.veScale.DecimalPlaces = 6;
            this.veScale.Enabled = false;
            this.veScale.Increment = new decimal( new int[] {
            5,
            0,
            0,
            131072} );
            this.veScale.Location = new System.Drawing.Point( 25, 110 );
            this.veScale.Maximum = new decimal( new int[] {
            -1,
            -1,
            -1,
            0} );
            this.veScale.Minimum = new decimal( new int[] {
            0,
            0,
            0,
            0} );
            this.veScale.Name = "veScale";
            this.veScale.ReadOnly = false;
            this.veScale.Size = new System.Drawing.Size( 219, 20 );
            this.veScale.TabIndex = 2;
            this.veScale.VectorChanged += new System.EventHandler( this.veScale_VectorChanged );
            // 
            // veRotate
            // 
            this.veRotate.DecimalPlaces = 6;
            this.veRotate.Enabled = false;
            this.veRotate.Increment = new decimal( new int[] {
            1,
            0,
            0,
            0} );
            this.veRotate.Location = new System.Drawing.Point( 25, 71 );
            this.veRotate.Maximum = new decimal( new int[] {
            -1,
            -1,
            -1,
            0} );
            this.veRotate.Minimum = new decimal( new int[] {
            -1,
            -1,
            -1,
            -2147483648} );
            this.veRotate.Name = "veRotate";
            this.veRotate.ReadOnly = false;
            this.veRotate.Size = new System.Drawing.Size( 219, 20 );
            this.veRotate.TabIndex = 1;
            this.veRotate.VectorChanged += new System.EventHandler( this.veRotate_VectorChanged );
            // 
            // veTranslate
            // 
            this.veTranslate.DecimalPlaces = 6;
            this.veTranslate.Enabled = false;
            this.veTranslate.Increment = new decimal( new int[] {
            1,
            0,
            0,
            65536} );
            this.veTranslate.Location = new System.Drawing.Point( 25, 32 );
            this.veTranslate.Maximum = new decimal( new int[] {
            -1,
            -1,
            -1,
            0} );
            this.veTranslate.Minimum = new decimal( new int[] {
            -1,
            -1,
            -1,
            -2147483648} );
            this.veTranslate.Name = "veTranslate";
            this.veTranslate.ReadOnly = false;
            this.veTranslate.Size = new System.Drawing.Size( 219, 20 );
            this.veTranslate.TabIndex = 0;
            this.veTranslate.VectorChanged += new System.EventHandler( this.veTranslate_VectorChanged );
            // 
            // lblScale
            // 
            this.lblScale.AutoSize = true;
            this.lblScale.Location = new System.Drawing.Point( 22, 94 );
            this.lblScale.Name = "lblScale";
            this.lblScale.Size = new System.Drawing.Size( 34, 13 );
            this.lblScale.TabIndex = 0;
            this.lblScale.Text = "Scale";
            this.lblScale.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // lblRotate
            // 
            this.lblRotate.AutoSize = true;
            this.lblRotate.Location = new System.Drawing.Point( 22, 55 );
            this.lblRotate.Name = "lblRotate";
            this.lblRotate.Size = new System.Drawing.Size( 39, 13 );
            this.lblRotate.TabIndex = 0;
            this.lblRotate.Text = "Rotate";
            this.lblRotate.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // lblTranslate
            // 
            this.lblTranslate.AutoSize = true;
            this.lblTranslate.Location = new System.Drawing.Point( 22, 16 );
            this.lblTranslate.Name = "lblTranslate";
            this.lblTranslate.Size = new System.Drawing.Size( 51, 13 );
            this.lblTranslate.TabIndex = 0;
            this.lblTranslate.Text = "Translate";
            this.lblTranslate.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // TransformControl
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.Controls.Add( this.gbTransform );
            this.MaximumSize = new System.Drawing.Size( 1000, 1000 );
            this.MinimumSize = new System.Drawing.Size( 270, 136 );
            this.Name = "TransformControl";
            this.Size = new System.Drawing.Size( 270, 136 );
            this.gbTransform.ResumeLayout( false );
            this.gbTransform.PerformLayout();
            this.ResumeLayout( false );

        }

        #endregion

        private System.Windows.Forms.GroupBox gbTransform;
        private System.Windows.Forms.Label lblTranslate;
        private VectorEditor veTranslate;
        private VectorEditor veScale;
        private VectorEditor veRotate;
        private System.Windows.Forms.Label lblScale;
        private System.Windows.Forms.Label lblRotate;
    }
}
