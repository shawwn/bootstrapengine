namespace Editor.Controls
{
    partial class VectorEditor
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
            if ( disposing && ( components != null ) )
            {
                components.Dispose();
            }
            base.Dispose( disposing );
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.numVecX = new Syncfusion.Windows.Forms.Tools.NumericUpDownExt();
            this.numVecY = new Syncfusion.Windows.Forms.Tools.NumericUpDownExt();
            this.numVecZ = new Syncfusion.Windows.Forms.Tools.NumericUpDownExt();
            ( (System.ComponentModel.ISupportInitialize)( this.numVecX ) ).BeginInit();
            ( (System.ComponentModel.ISupportInitialize)( this.numVecY ) ).BeginInit();
            ( (System.ComponentModel.ISupportInitialize)( this.numVecZ ) ).BeginInit();
            this.SuspendLayout();
            // 
            // numVecX
            // 
            this.numVecX.DecimalPlaces = 6;
            this.numVecX.Location = new System.Drawing.Point( 0, 0 );
            this.numVecX.Margin = new System.Windows.Forms.Padding( 0 );
            this.numVecX.Maximum = new decimal( new int[] {
            -1,
            -1,
            -1,
            0} );
            this.numVecX.Minimum = new decimal( new int[] {
            -1,
            -1,
            -1,
            -2147483648} );
            this.numVecX.Name = "numVecX";
            this.numVecX.Size = new System.Drawing.Size( 73, 20 );
            this.numVecX.TabIndex = 0;
            this.numVecX.ValueChanged += new System.EventHandler( this.numVec_ValueChanged );
            this.numVecX.Enter += new System.EventHandler( this.numVec_Enter );
            // 
            // numVecY
            // 
            this.numVecY.DecimalPlaces = 6;
            this.numVecY.Location = new System.Drawing.Point( 73, 0 );
            this.numVecY.Margin = new System.Windows.Forms.Padding( 0 );
            this.numVecY.Maximum = new decimal( new int[] {
            -1,
            -1,
            -1,
            0} );
            this.numVecY.Minimum = new decimal( new int[] {
            -1,
            -1,
            -1,
            -2147483648} );
            this.numVecY.Name = "numVecY";
            this.numVecY.Size = new System.Drawing.Size( 73, 20 );
            this.numVecY.TabIndex = 1;
            this.numVecY.ValueChanged += new System.EventHandler( this.numVec_ValueChanged );
            this.numVecY.Enter += new System.EventHandler( this.numVec_Enter );
            // 
            // numVecZ
            // 
            this.numVecZ.DecimalPlaces = 6;
            this.numVecZ.Location = new System.Drawing.Point( 146, 0 );
            this.numVecZ.Margin = new System.Windows.Forms.Padding( 0 );
            this.numVecZ.Maximum = new decimal( new int[] {
            -1,
            -1,
            -1,
            0} );
            this.numVecZ.Minimum = new decimal( new int[] {
            -1,
            -1,
            -1,
            -2147483648} );
            this.numVecZ.Name = "numVecZ";
            this.numVecZ.Size = new System.Drawing.Size( 73, 20 );
            this.numVecZ.TabIndex = 2;
            this.numVecZ.ValueChanged += new System.EventHandler( this.numVec_ValueChanged );
            this.numVecZ.Enter += new System.EventHandler( this.numVec_Enter );
            // 
            // VectorEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF( 6F, 13F );
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add( this.numVecZ );
            this.Controls.Add( this.numVecY );
            this.Controls.Add( this.numVecX );
            this.Name = "VectorEditor";
            this.Size = new System.Drawing.Size( 219, 20 );
            ( (System.ComponentModel.ISupportInitialize)( this.numVecX ) ).EndInit();
            ( (System.ComponentModel.ISupportInitialize)( this.numVecY ) ).EndInit();
            ( (System.ComponentModel.ISupportInitialize)( this.numVecZ ) ).EndInit();
            this.ResumeLayout( false );

        }

        #endregion

        private Syncfusion.Windows.Forms.Tools.NumericUpDownExt numVecX;
        private Syncfusion.Windows.Forms.Tools.NumericUpDownExt numVecY;
        private Syncfusion.Windows.Forms.Tools.NumericUpDownExt numVecZ;
    }
}
