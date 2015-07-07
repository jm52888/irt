﻿namespace BikeSignalProcessing.View
{
    partial class ChartView
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
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ChartView));
            this.button1 = new System.Windows.Forms.Button();
            this.chart1 = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.btnReset = new System.Windows.Forms.Button();
            this.upDownThreshold = new System.Windows.Forms.NumericUpDown();
            this.btnBest = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.upDownMinWindow = new System.Windows.Forms.NumericUpDown();
            this.label2 = new System.Windows.Forms.Label();
            this.btnViewData = new System.Windows.Forms.Button();
            this.txtDrag = new System.Windows.Forms.TextBox();
            this.txtRR = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.upDownSkipRows = new System.Windows.Forms.NumericUpDown();
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).BeginInit();
            this.contextMenuStrip1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.upDownThreshold)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.upDownMinWindow)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.upDownSkipRows)).BeginInit();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(24, 29);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(104, 31);
            this.button1.TabIndex = 0;
            this.button1.Text = "Load";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // chart1
            // 
            chartArea1.Name = "ChartArea1";
            this.chart1.ChartAreas.Add(chartArea1);
            this.chart1.Dock = System.Windows.Forms.DockStyle.Fill;
            legend1.Name = "Legend1";
            this.chart1.Legends.Add(legend1);
            this.chart1.Location = new System.Drawing.Point(0, 0);
            this.chart1.Name = "chart1";
            series1.ChartArea = "ChartArea1";
            series1.Legend = "Legend1";
            series1.Name = "Series1";
            this.chart1.Series.Add(series1);
            this.chart1.Size = new System.Drawing.Size(992, 658);
            this.chart1.TabIndex = 1;
            this.chart1.Text = "chart1";
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.ImageScalingSize = new System.Drawing.Size(24, 24);
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(124, 34);
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.openToolStripMenuItem});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(123, 30);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // openToolStripMenuItem
            // 
            this.openToolStripMenuItem.Name = "openToolStripMenuItem";
            this.openToolStripMenuItem.Size = new System.Drawing.Size(141, 30);
            this.openToolStripMenuItem.Text = "Open";
            // 
            // btnReset
            // 
            this.btnReset.Location = new System.Drawing.Point(24, 105);
            this.btnReset.Name = "btnReset";
            this.btnReset.Size = new System.Drawing.Size(104, 31);
            this.btnReset.TabIndex = 3;
            this.btnReset.Text = "Reset";
            this.btnReset.UseVisualStyleBackColor = true;
            this.btnReset.Click += new System.EventHandler(this.btnReset_Click);
            // 
            // upDownThreshold
            // 
            this.upDownThreshold.DecimalPlaces = 1;
            this.upDownThreshold.Increment = new decimal(new int[] {
            5,
            0,
            0,
            65536});
            this.upDownThreshold.Location = new System.Drawing.Point(24, 149);
            this.upDownThreshold.Maximum = new decimal(new int[] {
            50,
            0,
            0,
            0});
            this.upDownThreshold.Name = "upDownThreshold";
            this.upDownThreshold.Size = new System.Drawing.Size(69, 26);
            this.upDownThreshold.TabIndex = 4;
            this.upDownThreshold.ValueChanged += new System.EventHandler(this.upDownThreshold_ValueChanged);
            // 
            // btnBest
            // 
            this.btnBest.Location = new System.Drawing.Point(24, 66);
            this.btnBest.Name = "btnBest";
            this.btnBest.Size = new System.Drawing.Size(104, 31);
            this.btnBest.TabIndex = 5;
            this.btnBest.Text = "Best";
            this.btnBest.UseVisualStyleBackColor = true;
            this.btnBest.Click += new System.EventHandler(this.btnBest_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.BackColor = System.Drawing.SystemColors.Window;
            this.label1.Location = new System.Drawing.Point(99, 152);
            this.label1.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(62, 20);
            this.label1.TabIndex = 6;
            this.label1.Text = "StdDev";
            // 
            // upDownMinWindow
            // 
            this.upDownMinWindow.Location = new System.Drawing.Point(24, 186);
            this.upDownMinWindow.Maximum = new decimal(new int[] {
            500,
            0,
            0,
            0});
            this.upDownMinWindow.Minimum = new decimal(new int[] {
            9,
            0,
            0,
            0});
            this.upDownMinWindow.Name = "upDownMinWindow";
            this.upDownMinWindow.Size = new System.Drawing.Size(69, 26);
            this.upDownMinWindow.TabIndex = 7;
            this.upDownMinWindow.Value = new decimal(new int[] {
            9,
            0,
            0,
            0});
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.BackColor = System.Drawing.SystemColors.Window;
            this.label2.Location = new System.Drawing.Point(99, 189);
            this.label2.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(65, 20);
            this.label2.TabIndex = 8;
            this.label2.Text = "Window";
            // 
            // btnViewData
            // 
            this.btnViewData.Location = new System.Drawing.Point(24, 361);
            this.btnViewData.Name = "btnViewData";
            this.btnViewData.Size = new System.Drawing.Size(104, 31);
            this.btnViewData.TabIndex = 24;
            this.btnViewData.Text = "Data";
            this.btnViewData.UseVisualStyleBackColor = true;
            this.btnViewData.Click += new System.EventHandler(this.btnData_Click);
            // 
            // txtDrag
            // 
            this.txtDrag.Location = new System.Drawing.Point(74, 282);
            this.txtDrag.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.txtDrag.Name = "txtDrag";
            this.txtDrag.Size = new System.Drawing.Size(102, 26);
            this.txtDrag.TabIndex = 25;
            // 
            // txtRR
            // 
            this.txtRR.Location = new System.Drawing.Point(74, 322);
            this.txtRR.Margin = new System.Windows.Forms.Padding(4, 5, 4, 5);
            this.txtRR.Name = "txtRR";
            this.txtRR.Size = new System.Drawing.Size(102, 26);
            this.txtRR.TabIndex = 26;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.BackColor = System.Drawing.SystemColors.Window;
            this.label3.Location = new System.Drawing.Point(20, 287);
            this.label3.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(44, 20);
            this.label3.TabIndex = 27;
            this.label3.Text = "Drag";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.BackColor = System.Drawing.SystemColors.Window;
            this.label4.Location = new System.Drawing.Point(20, 327);
            this.label4.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(33, 20);
            this.label4.TabIndex = 28;
            this.label4.Text = "RR";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.BackColor = System.Drawing.SystemColors.Window;
            this.label5.Location = new System.Drawing.Point(99, 228);
            this.label5.Margin = new System.Windows.Forms.Padding(4, 0, 4, 0);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(84, 20);
            this.label5.TabIndex = 30;
            this.label5.Text = "Skip Rows";
            // 
            // upDownSkipRows
            // 
            this.upDownSkipRows.Location = new System.Drawing.Point(24, 225);
            this.upDownSkipRows.Maximum = new decimal(new int[] {
            9999,
            0,
            0,
            0});
            this.upDownSkipRows.Name = "upDownSkipRows";
            this.upDownSkipRows.Size = new System.Drawing.Size(69, 26);
            this.upDownSkipRows.TabIndex = 29;
            this.upDownSkipRows.Value = new decimal(new int[] {
            300,
            0,
            0,
            0});
            // 
            // ChartView
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 20F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(992, 658);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.upDownSkipRows);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.txtRR);
            this.Controls.Add(this.txtDrag);
            this.Controls.Add(this.btnViewData);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.upDownMinWindow);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.btnBest);
            this.Controls.Add(this.upDownThreshold);
            this.Controls.Add(this.btnReset);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.chart1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "ChartView";
            this.Text = "Chart View";
            ((System.ComponentModel.ISupportInitialize)(this.chart1)).EndInit();
            this.contextMenuStrip1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.upDownThreshold)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.upDownMinWindow)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.upDownSkipRows)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.DataVisualization.Charting.Chart chart1;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem openToolStripMenuItem;
        private System.Windows.Forms.Button btnReset;
        private System.Windows.Forms.NumericUpDown upDownThreshold;
        private System.Windows.Forms.Button btnBest;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.NumericUpDown upDownMinWindow;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button btnViewData;
        private System.Windows.Forms.TextBox txtDrag;
        private System.Windows.Forms.TextBox txtRR;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown upDownSkipRows;
    }
}

