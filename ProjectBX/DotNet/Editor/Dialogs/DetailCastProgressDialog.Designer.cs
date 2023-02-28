namespace Editor.Dialogs
{
    partial class DetailCastProgressDialog
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
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.lblTrisTotal = new System.Windows.Forms.Label();
            this.lblTrisRemaining = new System.Windows.Forms.Label();
            this.lblRaysCast = new System.Windows.Forms.Label();
            this.pbProgress = new System.Windows.Forms.ProgressBar();
            this.lblStatus = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // bnCancel
            // 
            this.bnCancel.Location = new System.Drawing.Point(212, 122);
            this.bnCancel.Visible = false;
            // 
            // bnOK
            // 
            this.bnOK.Location = new System.Drawing.Point(101, 122);
            this.bnOK.Click += new System.EventHandler(this.bnOK_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(106, 13);
            this.label1.TabIndex = 12;
            this.label1.Text = "Total Triangle Count:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 30);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(106, 13);
            this.label2.TabIndex = 13;
            this.label2.Text = "Triangles Remaining:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(60, 50);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(58, 13);
            this.label3.TabIndex = 14;
            this.label3.Text = "Rays Cast:";
            // 
            // lblTrisTotal
            // 
            this.lblTrisTotal.Location = new System.Drawing.Point(124, 10);
            this.lblTrisTotal.Name = "lblTrisTotal";
            this.lblTrisTotal.Size = new System.Drawing.Size(70, 13);
            this.lblTrisTotal.TabIndex = 15;
            this.lblTrisTotal.Text = "0";
            // 
            // lblTrisRemaining
            // 
            this.lblTrisRemaining.Location = new System.Drawing.Point(124, 30);
            this.lblTrisRemaining.Name = "lblTrisRemaining";
            this.lblTrisRemaining.Size = new System.Drawing.Size(70, 13);
            this.lblTrisRemaining.TabIndex = 16;
            this.lblTrisRemaining.Text = "0";
            // 
            // lblRaysCast
            // 
            this.lblRaysCast.Location = new System.Drawing.Point(124, 50);
            this.lblRaysCast.Name = "lblRaysCast";
            this.lblRaysCast.Size = new System.Drawing.Size(70, 13);
            this.lblRaysCast.TabIndex = 17;
            this.lblRaysCast.Text = "0";
            // 
            // pbProgress
            // 
            this.pbProgress.Location = new System.Drawing.Point(15, 79);
            this.pbProgress.Maximum = 1000;
            this.pbProgress.Name = "pbProgress";
            this.pbProgress.Size = new System.Drawing.Size(254, 23);
            this.pbProgress.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.pbProgress.TabIndex = 18;
            // 
            // lblStatus
            // 
            this.lblStatus.AutoSize = true;
            this.lblStatus.Location = new System.Drawing.Point(12, 105);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(61, 13);
            this.lblStatus.TabIndex = 20;
            this.lblStatus.Text = "Initializing...";
            // 
            // DetailCastProgressDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(278, 157);
            this.Controls.Add(this.lblStatus);
            this.Controls.Add(this.pbProgress);
            this.Controls.Add(this.lblRaysCast);
            this.Controls.Add(this.lblTrisRemaining);
            this.Controls.Add(this.lblTrisTotal);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Name = "DetailCastProgressDialog";
            this.Text = "DetailCastProgressDialog";
            this.Controls.SetChildIndex(this.label1, 0);
            this.Controls.SetChildIndex(this.bnOK, 0);
            this.Controls.SetChildIndex(this.bnCancel, 0);
            this.Controls.SetChildIndex(this.label2, 0);
            this.Controls.SetChildIndex(this.label3, 0);
            this.Controls.SetChildIndex(this.lblTrisTotal, 0);
            this.Controls.SetChildIndex(this.lblTrisRemaining, 0);
            this.Controls.SetChildIndex(this.lblRaysCast, 0);
            this.Controls.SetChildIndex(this.pbProgress, 0);
            this.Controls.SetChildIndex(this.lblStatus, 0);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label lblTrisTotal;
        private System.Windows.Forms.Label lblTrisRemaining;
        private System.Windows.Forms.Label lblRaysCast;
        private System.Windows.Forms.ProgressBar pbProgress;
        private System.Windows.Forms.Label lblStatus;
    }
}