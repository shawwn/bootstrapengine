namespace Editor.Controls
{
    partial class LabeledSliderControl
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
            this.lbLabel = new System.Windows.Forms.Label();
            this.tbValue = new System.Windows.Forms.TextBox();
            this.tbSlider = new System.Windows.Forms.TrackBar();
            ((System.ComponentModel.ISupportInitialize)(this.tbSlider)).BeginInit();
            this.SuspendLayout();
            // 
            // lbLabel
            // 
            this.lbLabel.Location = new System.Drawing.Point(5, 0);
            this.lbLabel.Name = "lbLabel";
            this.lbLabel.Size = new System.Drawing.Size(73, 19);
            this.lbLabel.TabIndex = 2;
            this.lbLabel.Text = "label1";
            this.lbLabel.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // tbValue
            // 
            this.tbValue.Location = new System.Drawing.Point(84, 0);
            this.tbValue.Name = "tbValue";
            this.tbValue.Size = new System.Drawing.Size(80, 20);
            this.tbValue.TabIndex = 1;
            this.tbValue.TextChanged += new System.EventHandler(this.tbValue_TextChanged);
            // 
            // tbSlider
            // 
            this.tbSlider.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tbSlider.AutoSize = false;
            this.tbSlider.Location = new System.Drawing.Point(170, 0);
            this.tbSlider.Maximum = 100;
            this.tbSlider.Name = "tbSlider";
            this.tbSlider.Size = new System.Drawing.Size(114, 19);
            this.tbSlider.TabIndex = 0;
            this.tbSlider.Scroll += new System.EventHandler(this.tbSlider_Scroll);
            // 
            // LabeledSliderControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.lbLabel);
            this.Controls.Add(this.tbValue);
            this.Controls.Add(this.tbSlider);
            this.Name = "LabeledSliderControl";
            this.Size = new System.Drawing.Size(287, 21);
            ((System.ComponentModel.ISupportInitialize)(this.tbSlider)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label lbLabel;
        private System.Windows.Forms.TextBox tbValue;
        private System.Windows.Forms.TrackBar tbSlider;
    }
}
