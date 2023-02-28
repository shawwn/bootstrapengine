namespace Editor.Controls
{
    partial class BrushAttributes
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.tbBrushSize = new Editor.Controls.LabeledSliderControl();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.ForeColor = System.Drawing.Color.White;
            this.label1.Location = new System.Drawing.Point(10, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(57, 13);
            this.label1.TabIndex = 1;
            this.label1.Text = "Brush Size";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.tbBrushSize);
            this.groupBox1.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox1.ForeColor = System.Drawing.Color.White;
            this.groupBox1.Location = new System.Drawing.Point(0, 0);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(276, 42);
            this.groupBox1.TabIndex = 2;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Brush Size";
            // 
            // tbBrushSize
            // 
            this.tbBrushSize.AllowMaxOverride = false;
            this.tbBrushSize.AllowMinOverride = false;
            this.tbBrushSize.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbBrushSize.Label = "Size (m)";
            this.tbBrushSize.Location = new System.Drawing.Point(3, 16);
            this.tbBrushSize.MaxValue = 1F;
            this.tbBrushSize.MinValue = 0F;
            this.tbBrushSize.Name = "tbBrushSize";
            this.tbBrushSize.Size = new System.Drawing.Size(270, 23);
            this.tbBrushSize.SliderResolution = 100;
            this.tbBrushSize.TabIndex = 0;
            this.tbBrushSize.Value = 0F;
            // 
            // BrushAttributes
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.label1);
            this.Name = "BrushAttributes";
            this.Size = new System.Drawing.Size(276, 55);
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox1;
        private LabeledSliderControl tbBrushSize;
    }
}
