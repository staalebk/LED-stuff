namespace RadioControlPlugin2
{
    partial class Form1
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
            this.components = new System.ComponentModel.Container();
            this.buttonConnect = new System.Windows.Forms.Button();
            this.buttonDisconnect = new System.Windows.Forms.Button();
            this.richResponse = new System.Windows.Forms.RichTextBox();
            this.buttonIncMhz = new System.Windows.Forms.Button();
            this.buttonDecMhz = new System.Windows.Forms.Button();
            this.buttonIncKhz = new System.Windows.Forms.Button();
            this.buttonDecKhz = new System.Windows.Forms.Button();
            this.serialPort = new System.IO.Ports.SerialPort(this.components);
            this.ddPort = new System.Windows.Forms.ComboBox();
            this.buttonXPlaneStart = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // buttonConnect
            // 
            this.buttonConnect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.buttonConnect.Location = new System.Drawing.Point(12, 344);
            this.buttonConnect.Name = "buttonConnect";
            this.buttonConnect.Size = new System.Drawing.Size(99, 23);
            this.buttonConnect.TabIndex = 0;
            this.buttonConnect.Text = "Connect FSX";
            this.buttonConnect.UseVisualStyleBackColor = true;
            this.buttonConnect.Click += new System.EventHandler(this.buttonConnect_Click);
            // 
            // buttonDisconnect
            // 
            this.buttonDisconnect.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonDisconnect.Location = new System.Drawing.Point(785, 344);
            this.buttonDisconnect.Name = "buttonDisconnect";
            this.buttonDisconnect.Size = new System.Drawing.Size(75, 23);
            this.buttonDisconnect.TabIndex = 2;
            this.buttonDisconnect.Text = "Disconnect";
            this.buttonDisconnect.UseVisualStyleBackColor = true;
            this.buttonDisconnect.Click += new System.EventHandler(this.buttonDisconnect_Click);
            // 
            // richResponse
            // 
            this.richResponse.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.richResponse.Location = new System.Drawing.Point(12, 12);
            this.richResponse.Name = "richResponse";
            this.richResponse.Size = new System.Drawing.Size(848, 298);
            this.richResponse.TabIndex = 3;
            this.richResponse.Text = "";
            // 
            // buttonIncMhz
            // 
            this.buttonIncMhz.Location = new System.Drawing.Point(328, 344);
            this.buttonIncMhz.Name = "buttonIncMhz";
            this.buttonIncMhz.Size = new System.Drawing.Size(75, 23);
            this.buttonIncMhz.TabIndex = 5;
            this.buttonIncMhz.Text = "Inc MHz";
            this.buttonIncMhz.UseVisualStyleBackColor = true;
            this.buttonIncMhz.Click += new System.EventHandler(this.buttonIncMhz_Click);
            // 
            // buttonDecMhz
            // 
            this.buttonDecMhz.Location = new System.Drawing.Point(409, 344);
            this.buttonDecMhz.Name = "buttonDecMhz";
            this.buttonDecMhz.Size = new System.Drawing.Size(75, 23);
            this.buttonDecMhz.TabIndex = 6;
            this.buttonDecMhz.Text = "Dec MHz";
            this.buttonDecMhz.UseVisualStyleBackColor = true;
            this.buttonDecMhz.Click += new System.EventHandler(this.buttonDecMhz_Click);
            // 
            // buttonIncKhz
            // 
            this.buttonIncKhz.Location = new System.Drawing.Point(490, 344);
            this.buttonIncKhz.Name = "buttonIncKhz";
            this.buttonIncKhz.Size = new System.Drawing.Size(75, 23);
            this.buttonIncKhz.TabIndex = 7;
            this.buttonIncKhz.Text = "Inc KHz";
            this.buttonIncKhz.UseVisualStyleBackColor = true;
            this.buttonIncKhz.Click += new System.EventHandler(this.buttonIncKhz_Click);
            // 
            // buttonDecKhz
            // 
            this.buttonDecKhz.Location = new System.Drawing.Point(571, 344);
            this.buttonDecKhz.Name = "buttonDecKhz";
            this.buttonDecKhz.Size = new System.Drawing.Size(75, 23);
            this.buttonDecKhz.TabIndex = 8;
            this.buttonDecKhz.Text = "Dec KHz";
            this.buttonDecKhz.UseVisualStyleBackColor = true;
            this.buttonDecKhz.Click += new System.EventHandler(this.buttonDecKhz_Click);
            // 
            // ddPort
            // 
            this.ddPort.FormattingEnabled = true;
            this.ddPort.Location = new System.Drawing.Point(117, 346);
            this.ddPort.Name = "ddPort";
            this.ddPort.Size = new System.Drawing.Size(87, 21);
            this.ddPort.TabIndex = 9;
            // 
            // buttonXPlaneStart
            // 
            this.buttonXPlaneStart.Location = new System.Drawing.Point(12, 315);
            this.buttonXPlaneStart.Name = "buttonXPlaneStart";
            this.buttonXPlaneStart.Size = new System.Drawing.Size(99, 23);
            this.buttonXPlaneStart.TabIndex = 10;
            this.buttonXPlaneStart.Text = "Connect X-Plane";
            this.buttonXPlaneStart.UseVisualStyleBackColor = true;
            this.buttonXPlaneStart.Click += new System.EventHandler(this.buttonXPlaneStart_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(872, 379);
            this.Controls.Add(this.buttonXPlaneStart);
            this.Controls.Add(this.ddPort);
            this.Controls.Add(this.buttonDecKhz);
            this.Controls.Add(this.buttonIncKhz);
            this.Controls.Add(this.buttonDecMhz);
            this.Controls.Add(this.buttonIncMhz);
            this.Controls.Add(this.richResponse);
            this.Controls.Add(this.buttonDisconnect);
            this.Controls.Add(this.buttonConnect);
            this.Name = "Form1";
            this.Text = "Radio Control Plugin";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button buttonConnect;
        private System.Windows.Forms.Button buttonDisconnect;
        private System.Windows.Forms.RichTextBox richResponse;
        private System.Windows.Forms.Button buttonIncMhz;
        private System.Windows.Forms.Button buttonDecMhz;
        private System.Windows.Forms.Button buttonIncKhz;
        private System.Windows.Forms.Button buttonDecKhz;
        private System.IO.Ports.SerialPort serialPort;
        private System.Windows.Forms.ComboBox ddPort;
        private System.Windows.Forms.Button buttonXPlaneStart;
    }
}

