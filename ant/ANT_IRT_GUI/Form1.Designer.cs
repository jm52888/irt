﻿namespace ANT_IRT_GUI
{
    partial class frmIrtGui
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmIrtGui));
            this.txtLog = new System.Windows.Forms.TextBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.txtEmrDeviceId = new System.Windows.Forms.TextBox();
            this.lblEmrSerialNo = new System.Windows.Forms.Label();
            this.lblEmrFirmwareRev = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.lblEmrHardwareRev = new System.Windows.Forms.Label();
            this.lblEmrModel = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.lblRefPwrManuf = new System.Windows.Forms.Label();
            this.txtRefPwrDeviceId = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.lblRefPwrType = new System.Windows.Forms.Label();
            this.lblRefPwrModel = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.lblEmrBattVolt = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.lblEmrBattTime = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.txtSlope = new System.Windows.Forms.TextBox();
            this.txtOffset = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.btnRefPwrSearch = new System.Windows.Forms.Button();
            this.btnEmrSearch = new System.Windows.Forms.Button();
            this.label13 = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.label18 = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.btnCalibrationSet = new System.Windows.Forms.Button();
            this.lblChargeStatus = new System.Windows.Forms.Label();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.cmbResistanceMode = new System.Windows.Forms.ComboBox();
            this.btnResistanceSet = new System.Windows.Forms.Button();
            this.pnlResistanceStd = new System.Windows.Forms.Panel();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.label22 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.pnlErg = new System.Windows.Forms.Panel();
            this.txtResistanceErgWatts = new System.Windows.Forms.TextBox();
            this.label20 = new System.Windows.Forms.Label();
            this.pnlResistanceSim = new System.Windows.Forms.Panel();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label23 = new System.Windows.Forms.Label();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.label24 = new System.Windows.Forms.Label();
            this.textBox4 = new System.Windows.Forms.TextBox();
            this.label25 = new System.Windows.Forms.Label();
            this.label26 = new System.Windows.Forms.Label();
            this.txtTotalWeight = new System.Windows.Forms.TextBox();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.label21 = new System.Windows.Forms.Label();
            this.txtWheelSizeMm = new System.Windows.Forms.TextBox();
            this.btnSettingsSet = new System.Windows.Forms.Button();
            this.chkLstSettings = new System.Windows.Forms.CheckedListBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.txtParamGet = new System.Windows.Forms.TextBox();
            this.btnParamSet = new System.Windows.Forms.Button();
            this.btnParamGet = new System.Windows.Forms.Button();
            this.txtParamSet = new System.Windows.Forms.TextBox();
            this.lblServoOffset = new System.Windows.Forms.Label();
            this.label28 = new System.Windows.Forms.Label();
            this.txtServoOffset = new System.Windows.Forms.TextBox();
            this.btnServoOffset = new System.Windows.Forms.Button();
            this.lblResistanceStdLevel = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.pnlResistanceStd.SuspendLayout();
            this.pnlErg.SuspendLayout();
            this.pnlResistanceSim.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.SuspendLayout();
            // 
            // txtLog
            // 
            this.txtLog.Location = new System.Drawing.Point(13, 334);
            this.txtLog.Multiline = true;
            this.txtLog.Name = "txtLog";
            this.txtLog.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.txtLog.Size = new System.Drawing.Size(759, 215);
            this.txtLog.TabIndex = 0;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.btnServoOffset);
            this.groupBox1.Controls.Add(this.txtServoOffset);
            this.groupBox1.Controls.Add(this.lblServoOffset);
            this.groupBox1.Controls.Add(this.label28);
            this.groupBox1.Controls.Add(this.label18);
            this.groupBox1.Controls.Add(this.lblChargeStatus);
            this.groupBox1.Controls.Add(this.label19);
            this.groupBox1.Controls.Add(this.lblEmrBattTime);
            this.groupBox1.Controls.Add(this.label10);
            this.groupBox1.Controls.Add(this.label11);
            this.groupBox1.Controls.Add(this.lblEmrBattVolt);
            this.groupBox1.Controls.Add(this.label16);
            this.groupBox1.Controls.Add(this.label13);
            this.groupBox1.Controls.Add(this.btnEmrSearch);
            this.groupBox1.Controls.Add(this.lblEmrModel);
            this.groupBox1.Controls.Add(this.label9);
            this.groupBox1.Controls.Add(this.lblEmrHardwareRev);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.lblEmrFirmwareRev);
            this.groupBox1.Controls.Add(this.lblEmrSerialNo);
            this.groupBox1.Controls.Add(this.txtEmrDeviceId);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(346, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(211, 312);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "E-Motion Rollers";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(7, 26);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(55, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Device ID";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 45);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(50, 13);
            this.label2.TabIndex = 1;
            this.label2.Text = "Serial No";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(7, 64);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(72, 13);
            this.label3.TabIndex = 2;
            this.label3.Text = "Firmware Rev";
            // 
            // txtEmrDeviceId
            // 
            this.txtEmrDeviceId.Location = new System.Drawing.Point(111, 19);
            this.txtEmrDeviceId.Name = "txtEmrDeviceId";
            this.txtEmrDeviceId.Size = new System.Drawing.Size(77, 20);
            this.txtEmrDeviceId.TabIndex = 3;
            this.txtEmrDeviceId.Text = "0";
            // 
            // lblEmrSerialNo
            // 
            this.lblEmrSerialNo.AutoSize = true;
            this.lblEmrSerialNo.Location = new System.Drawing.Point(108, 45);
            this.lblEmrSerialNo.Name = "lblEmrSerialNo";
            this.lblEmrSerialNo.Size = new System.Drawing.Size(16, 13);
            this.lblEmrSerialNo.TabIndex = 4;
            this.lblEmrSerialNo.Text = "...";
            // 
            // lblEmrFirmwareRev
            // 
            this.lblEmrFirmwareRev.AutoSize = true;
            this.lblEmrFirmwareRev.Location = new System.Drawing.Point(108, 64);
            this.lblEmrFirmwareRev.Name = "lblEmrFirmwareRev";
            this.lblEmrFirmwareRev.Size = new System.Drawing.Size(16, 13);
            this.lblEmrFirmwareRev.TabIndex = 5;
            this.lblEmrFirmwareRev.Text = "...";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(7, 83);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(76, 13);
            this.label6.TabIndex = 6;
            this.label6.Text = "Hardware Rev";
            // 
            // lblEmrHardwareRev
            // 
            this.lblEmrHardwareRev.AutoSize = true;
            this.lblEmrHardwareRev.Location = new System.Drawing.Point(108, 83);
            this.lblEmrHardwareRev.Name = "lblEmrHardwareRev";
            this.lblEmrHardwareRev.Size = new System.Drawing.Size(16, 13);
            this.lblEmrHardwareRev.TabIndex = 7;
            this.lblEmrHardwareRev.Text = "...";
            // 
            // lblEmrModel
            // 
            this.lblEmrModel.AutoSize = true;
            this.lblEmrModel.Location = new System.Drawing.Point(108, 104);
            this.lblEmrModel.Name = "lblEmrModel";
            this.lblEmrModel.Size = new System.Drawing.Size(13, 13);
            this.lblEmrModel.TabIndex = 9;
            this.lblEmrModel.Text = "..";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(7, 102);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(36, 13);
            this.label9.TabIndex = 8;
            this.label9.Text = "Model";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(7, 83);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(31, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "Type";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(108, 64);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(16, 13);
            this.label5.TabIndex = 5;
            this.label5.Text = "...";
            // 
            // lblRefPwrManuf
            // 
            this.lblRefPwrManuf.AutoSize = true;
            this.lblRefPwrManuf.Location = new System.Drawing.Point(108, 45);
            this.lblRefPwrManuf.Name = "lblRefPwrManuf";
            this.lblRefPwrManuf.Size = new System.Drawing.Size(16, 13);
            this.lblRefPwrManuf.TabIndex = 4;
            this.lblRefPwrManuf.Text = "...";
            // 
            // txtRefPwrDeviceId
            // 
            this.txtRefPwrDeviceId.Location = new System.Drawing.Point(111, 19);
            this.txtRefPwrDeviceId.Name = "txtRefPwrDeviceId";
            this.txtRefPwrDeviceId.Size = new System.Drawing.Size(77, 20);
            this.txtRefPwrDeviceId.TabIndex = 3;
            this.txtRefPwrDeviceId.Text = "0";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(7, 26);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(55, 13);
            this.label8.TabIndex = 0;
            this.label8.Text = "Device ID";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label17);
            this.groupBox2.Controls.Add(this.label15);
            this.groupBox2.Controls.Add(this.btnRefPwrSearch);
            this.groupBox2.Controls.Add(this.lblRefPwrType);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.lblRefPwrManuf);
            this.groupBox2.Controls.Add(this.txtRefPwrDeviceId);
            this.groupBox2.Controls.Add(this.lblRefPwrModel);
            this.groupBox2.Controls.Add(this.label14);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Location = new System.Drawing.Point(563, 12);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(211, 312);
            this.groupBox2.TabIndex = 10;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Power Meter";
            // 
            // lblRefPwrType
            // 
            this.lblRefPwrType.AutoSize = true;
            this.lblRefPwrType.Location = new System.Drawing.Point(108, 83);
            this.lblRefPwrType.Name = "lblRefPwrType";
            this.lblRefPwrType.Size = new System.Drawing.Size(16, 13);
            this.lblRefPwrType.TabIndex = 7;
            this.lblRefPwrType.Text = "...";
            // 
            // lblRefPwrModel
            // 
            this.lblRefPwrModel.AutoSize = true;
            this.lblRefPwrModel.Location = new System.Drawing.Point(7, 64);
            this.lblRefPwrModel.Name = "lblRefPwrModel";
            this.lblRefPwrModel.Size = new System.Drawing.Size(36, 13);
            this.lblRefPwrModel.TabIndex = 2;
            this.lblRefPwrModel.Text = "Model";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(7, 45);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(70, 13);
            this.label14.TabIndex = 1;
            this.label14.Text = "Manufacturer";
            // 
            // lblEmrBattVolt
            // 
            this.lblEmrBattVolt.AutoSize = true;
            this.lblEmrBattVolt.Location = new System.Drawing.Point(108, 121);
            this.lblEmrBattVolt.Name = "lblEmrBattVolt";
            this.lblEmrBattVolt.Size = new System.Drawing.Size(13, 13);
            this.lblEmrBattVolt.TabIndex = 11;
            this.lblEmrBattVolt.Text = "..";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(7, 119);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(79, 13);
            this.label10.TabIndex = 10;
            this.label10.Text = "Battery Voltage";
            // 
            // lblEmrBattTime
            // 
            this.lblEmrBattTime.AutoSize = true;
            this.lblEmrBattTime.Location = new System.Drawing.Point(108, 139);
            this.lblEmrBattTime.Name = "lblEmrBattTime";
            this.lblEmrBattTime.Size = new System.Drawing.Size(13, 13);
            this.lblEmrBattTime.TabIndex = 13;
            this.lblEmrBattTime.Text = "..";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(7, 137);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(79, 13);
            this.label11.TabIndex = 12;
            this.label11.Text = "Operating Time";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(22, 22);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(34, 13);
            this.label12.TabIndex = 14;
            this.label12.Text = "Slope";
            // 
            // txtSlope
            // 
            this.txtSlope.Location = new System.Drawing.Point(61, 19);
            this.txtSlope.Name = "txtSlope";
            this.txtSlope.Size = new System.Drawing.Size(75, 20);
            this.txtSlope.TabIndex = 10;
            // 
            // txtOffset
            // 
            this.txtOffset.Location = new System.Drawing.Point(61, 42);
            this.txtOffset.Name = "txtOffset";
            this.txtOffset.Size = new System.Drawing.Size(75, 20);
            this.txtOffset.TabIndex = 15;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(22, 44);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(35, 13);
            this.label7.TabIndex = 16;
            this.label7.Text = "Offset";
            // 
            // btnRefPwrSearch
            // 
            this.btnRefPwrSearch.Location = new System.Drawing.Point(70, 195);
            this.btnRefPwrSearch.Name = "btnRefPwrSearch";
            this.btnRefPwrSearch.Size = new System.Drawing.Size(75, 23);
            this.btnRefPwrSearch.TabIndex = 8;
            this.btnRefPwrSearch.Text = "Search";
            this.btnRefPwrSearch.UseVisualStyleBackColor = true;
            // 
            // btnEmrSearch
            // 
            this.btnEmrSearch.Location = new System.Drawing.Point(60, 195);
            this.btnEmrSearch.Name = "btnEmrSearch";
            this.btnEmrSearch.Size = new System.Drawing.Size(75, 23);
            this.btnEmrSearch.TabIndex = 9;
            this.btnEmrSearch.Text = "Search";
            this.btnEmrSearch.UseVisualStyleBackColor = true;
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Font = new System.Drawing.Font("Microsoft Sans Serif", 36F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label13.Location = new System.Drawing.Point(124, 225);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(51, 55);
            this.label13.TabIndex = 10;
            this.label13.Text = "0";
            this.label13.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label15
            // 
            this.label15.AutoSize = true;
            this.label15.Font = new System.Drawing.Font("Microsoft Sans Serif", 36F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label15.Location = new System.Drawing.Point(73, 224);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(51, 55);
            this.label15.TabIndex = 11;
            this.label15.Text = "0";
            this.label15.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label16
            // 
            this.label16.AutoSize = true;
            this.label16.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label16.Location = new System.Drawing.Point(131, 282);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(40, 13);
            this.label16.TabIndex = 11;
            this.label16.Text = "Watts";
            // 
            // label17
            // 
            this.label17.AutoSize = true;
            this.label17.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label17.Location = new System.Drawing.Point(76, 281);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(40, 13);
            this.label17.TabIndex = 12;
            this.label17.Text = "Watts";
            // 
            // label18
            // 
            this.label18.AutoSize = true;
            this.label18.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label18.Location = new System.Drawing.Point(36, 282);
            this.label18.Name = "label18";
            this.label18.Size = new System.Drawing.Size(34, 13);
            this.label18.TabIndex = 13;
            this.label18.Text = "MPH";
            // 
            // label19
            // 
            this.label19.AutoSize = true;
            this.label19.Font = new System.Drawing.Font("Microsoft Sans Serif", 36F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label19.Location = new System.Drawing.Point(29, 225);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(51, 55);
            this.label19.TabIndex = 12;
            this.label19.Text = "0";
            this.label19.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.btnCalibrationSet);
            this.groupBox3.Controls.Add(this.label12);
            this.groupBox3.Controls.Add(this.txtOffset);
            this.groupBox3.Controls.Add(this.txtSlope);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Location = new System.Drawing.Point(13, 12);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(146, 105);
            this.groupBox3.TabIndex = 17;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Calibration";
            // 
            // btnCalibrationSet
            // 
            this.btnCalibrationSet.Location = new System.Drawing.Point(40, 68);
            this.btnCalibrationSet.Name = "btnCalibrationSet";
            this.btnCalibrationSet.Size = new System.Drawing.Size(75, 23);
            this.btnCalibrationSet.TabIndex = 17;
            this.btnCalibrationSet.Text = "Set";
            this.btnCalibrationSet.UseVisualStyleBackColor = true;
            // 
            // lblChargeStatus
            // 
            this.lblChargeStatus.AutoSize = true;
            this.lblChargeStatus.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblChargeStatus.Location = new System.Drawing.Point(145, 121);
            this.lblChargeStatus.Name = "lblChargeStatus";
            this.lblChargeStatus.Size = new System.Drawing.Size(19, 13);
            this.lblChargeStatus.TabIndex = 14;
            this.lblChargeStatus.Text = "...";
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.pnlResistanceStd);
            this.groupBox5.Controls.Add(this.btnResistanceSet);
            this.groupBox5.Controls.Add(this.textBox2);
            this.groupBox5.Controls.Add(this.cmbResistanceMode);
            this.groupBox5.Controls.Add(this.label22);
            this.groupBox5.Controls.Add(this.pnlResistanceSim);
            this.groupBox5.Location = new System.Drawing.Point(13, 207);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(327, 117);
            this.groupBox5.TabIndex = 19;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Resistance";
            // 
            // cmbResistanceMode
            // 
            this.cmbResistanceMode.FormattingEnabled = true;
            this.cmbResistanceMode.Items.AddRange(new object[] {
            "Standard",
            "Percentage",
            "Erg",
            "Simulation"});
            this.cmbResistanceMode.Location = new System.Drawing.Point(25, 21);
            this.cmbResistanceMode.Name = "cmbResistanceMode";
            this.cmbResistanceMode.Size = new System.Drawing.Size(121, 21);
            this.cmbResistanceMode.TabIndex = 19;
            this.cmbResistanceMode.Text = "Standard";
            // 
            // btnResistanceSet
            // 
            this.btnResistanceSet.Location = new System.Drawing.Point(40, 78);
            this.btnResistanceSet.Name = "btnResistanceSet";
            this.btnResistanceSet.Size = new System.Drawing.Size(75, 23);
            this.btnResistanceSet.TabIndex = 18;
            this.btnResistanceSet.Text = "Set";
            this.btnResistanceSet.UseVisualStyleBackColor = true;
            // 
            // pnlResistanceStd
            // 
            this.pnlResistanceStd.Controls.Add(this.pnlErg);
            this.pnlResistanceStd.Location = new System.Drawing.Point(156, 19);
            this.pnlResistanceStd.Name = "pnlResistanceStd";
            this.pnlResistanceStd.Size = new System.Drawing.Size(164, 91);
            this.pnlResistanceStd.TabIndex = 20;
            // 
            // textBox2
            // 
            this.textBox2.Location = new System.Drawing.Point(83, 48);
            this.textBox2.Name = "textBox2";
            this.textBox2.Size = new System.Drawing.Size(63, 20);
            this.textBox2.TabIndex = 20;
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Location = new System.Drawing.Point(22, 51);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(44, 13);
            this.label22.TabIndex = 19;
            this.label22.Text = "Position";
            // 
            // button1
            // 
            this.button1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button1.Location = new System.Drawing.Point(28, 27);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(23, 28);
            this.button1.TabIndex = 0;
            this.button1.Text = "&+";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            this.button2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button2.Location = new System.Drawing.Point(114, 27);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(23, 28);
            this.button2.TabIndex = 1;
            this.button2.Text = "&-";
            this.button2.UseVisualStyleBackColor = true;
            // 
            // pnlErg
            // 
            this.pnlErg.Controls.Add(this.lblResistanceStdLevel);
            this.pnlErg.Controls.Add(this.button2);
            this.pnlErg.Controls.Add(this.button1);
            this.pnlErg.Controls.Add(this.txtResistanceErgWatts);
            this.pnlErg.Controls.Add(this.label20);
            this.pnlErg.Location = new System.Drawing.Point(0, 1);
            this.pnlErg.Name = "pnlErg";
            this.pnlErg.Size = new System.Drawing.Size(164, 91);
            this.pnlErg.TabIndex = 21;
            this.pnlErg.Visible = false;
            // 
            // txtResistanceErgWatts
            // 
            this.txtResistanceErgWatts.Location = new System.Drawing.Point(89, 35);
            this.txtResistanceErgWatts.Name = "txtResistanceErgWatts";
            this.txtResistanceErgWatts.Size = new System.Drawing.Size(63, 20);
            this.txtResistanceErgWatts.TabIndex = 22;
            this.txtResistanceErgWatts.Visible = false;
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Location = new System.Drawing.Point(6, 38);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(69, 13);
            this.label20.TabIndex = 21;
            this.label20.Text = "Target Watts";
            // 
            // pnlResistanceSim
            // 
            this.pnlResistanceSim.Controls.Add(this.textBox4);
            this.pnlResistanceSim.Controls.Add(this.label25);
            this.pnlResistanceSim.Controls.Add(this.textBox3);
            this.pnlResistanceSim.Controls.Add(this.label24);
            this.pnlResistanceSim.Controls.Add(this.textBox1);
            this.pnlResistanceSim.Controls.Add(this.label23);
            this.pnlResistanceSim.Location = new System.Drawing.Point(156, 20);
            this.pnlResistanceSim.Name = "pnlResistanceSim";
            this.pnlResistanceSim.Size = new System.Drawing.Size(164, 91);
            this.pnlResistanceSim.TabIndex = 23;
            this.pnlResistanceSim.Visible = false;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(89, 9);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(63, 20);
            this.textBox1.TabIndex = 22;
            this.textBox1.Visible = false;
            // 
            // label23
            // 
            this.label23.AutoSize = true;
            this.label23.Location = new System.Drawing.Point(6, 12);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(45, 13);
            this.label23.TabIndex = 21;
            this.label23.Text = "Slope %";
            // 
            // textBox3
            // 
            this.textBox3.Location = new System.Drawing.Point(89, 32);
            this.textBox3.Name = "textBox3";
            this.textBox3.Size = new System.Drawing.Size(63, 20);
            this.textBox3.TabIndex = 24;
            this.textBox3.Visible = false;
            // 
            // label24
            // 
            this.label24.AutoSize = true;
            this.label24.Location = new System.Drawing.Point(6, 35);
            this.label24.Name = "label24";
            this.label24.Size = new System.Drawing.Size(20, 13);
            this.label24.TabIndex = 23;
            this.label24.Text = "Crr";
            // 
            // textBox4
            // 
            this.textBox4.Location = new System.Drawing.Point(89, 55);
            this.textBox4.Name = "textBox4";
            this.textBox4.Size = new System.Drawing.Size(63, 20);
            this.textBox4.TabIndex = 26;
            this.textBox4.Visible = false;
            // 
            // label25
            // 
            this.label25.AutoSize = true;
            this.label25.Location = new System.Drawing.Point(6, 58);
            this.label25.Name = "label25";
            this.label25.Size = new System.Drawing.Size(60, 13);
            this.label25.TabIndex = 25;
            this.label25.Text = "Wind (mps)";
            // 
            // label26
            // 
            this.label26.AutoSize = true;
            this.label26.Location = new System.Drawing.Point(6, 24);
            this.label26.Name = "label26";
            this.label26.Size = new System.Drawing.Size(63, 13);
            this.label26.TabIndex = 19;
            this.label26.Text = "Weight (lbs)";
            // 
            // txtTotalWeight
            // 
            this.txtTotalWeight.Location = new System.Drawing.Point(84, 21);
            this.txtTotalWeight.Name = "txtTotalWeight";
            this.txtTotalWeight.Size = new System.Drawing.Size(75, 20);
            this.txtTotalWeight.TabIndex = 18;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.chkLstSettings);
            this.groupBox6.Controls.Add(this.btnSettingsSet);
            this.groupBox6.Controls.Add(this.label21);
            this.groupBox6.Controls.Add(this.txtWheelSizeMm);
            this.groupBox6.Controls.Add(this.label26);
            this.groupBox6.Controls.Add(this.txtTotalWeight);
            this.groupBox6.Location = new System.Drawing.Point(169, 12);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(164, 187);
            this.groupBox6.TabIndex = 19;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Profile";
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Location = new System.Drawing.Point(5, 50);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(63, 13);
            this.label21.TabIndex = 21;
            this.label21.Text = "Wheel (mm)";
            // 
            // txtWheelSizeMm
            // 
            this.txtWheelSizeMm.Location = new System.Drawing.Point(83, 47);
            this.txtWheelSizeMm.Name = "txtWheelSizeMm";
            this.txtWheelSizeMm.Size = new System.Drawing.Size(75, 20);
            this.txtWheelSizeMm.TabIndex = 20;
            // 
            // btnSettingsSet
            // 
            this.btnSettingsSet.Location = new System.Drawing.Point(46, 158);
            this.btnSettingsSet.Name = "btnSettingsSet";
            this.btnSettingsSet.Size = new System.Drawing.Size(75, 23);
            this.btnSettingsSet.TabIndex = 18;
            this.btnSettingsSet.Text = "Set";
            this.btnSettingsSet.UseVisualStyleBackColor = true;
            // 
            // chkLstSettings
            // 
            this.chkLstSettings.FormattingEnabled = true;
            this.chkLstSettings.Items.AddRange(new object[] {
            "Accelerometer Sleeps",
            "Bluetooth Enabled",
            "ANT+ Control",
            "ANT+ Bike Power",
            "ANT+ FE-C",
            "ANT+ Extra Info"});
            this.chkLstSettings.Location = new System.Drawing.Point(8, 73);
            this.chkLstSettings.Name = "chkLstSettings";
            this.chkLstSettings.Size = new System.Drawing.Size(150, 79);
            this.chkLstSettings.TabIndex = 22;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.txtParamSet);
            this.groupBox4.Controls.Add(this.btnParamGet);
            this.groupBox4.Controls.Add(this.btnParamSet);
            this.groupBox4.Controls.Add(this.txtParamGet);
            this.groupBox4.Location = new System.Drawing.Point(13, 123);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(146, 76);
            this.groupBox4.TabIndex = 18;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Get/Set Parameter";
            // 
            // txtParamGet
            // 
            this.txtParamGet.Location = new System.Drawing.Point(6, 19);
            this.txtParamGet.Name = "txtParamGet";
            this.txtParamGet.Size = new System.Drawing.Size(60, 20);
            this.txtParamGet.TabIndex = 10;
            // 
            // btnParamSet
            // 
            this.btnParamSet.Location = new System.Drawing.Point(84, 45);
            this.btnParamSet.Name = "btnParamSet";
            this.btnParamSet.Size = new System.Drawing.Size(47, 23);
            this.btnParamSet.TabIndex = 23;
            this.btnParamSet.Text = "Set";
            this.btnParamSet.UseVisualStyleBackColor = true;
            // 
            // btnParamGet
            // 
            this.btnParamGet.Location = new System.Drawing.Point(12, 45);
            this.btnParamGet.Name = "btnParamGet";
            this.btnParamGet.Size = new System.Drawing.Size(47, 23);
            this.btnParamGet.TabIndex = 24;
            this.btnParamGet.Text = "Get";
            this.btnParamGet.UseVisualStyleBackColor = true;
            // 
            // txtParamSet
            // 
            this.txtParamSet.Location = new System.Drawing.Point(76, 19);
            this.txtParamSet.Name = "txtParamSet";
            this.txtParamSet.Size = new System.Drawing.Size(64, 20);
            this.txtParamSet.TabIndex = 25;
            // 
            // lblServoOffset
            // 
            this.lblServoOffset.AutoSize = true;
            this.lblServoOffset.Location = new System.Drawing.Point(108, 158);
            this.lblServoOffset.Name = "lblServoOffset";
            this.lblServoOffset.Size = new System.Drawing.Size(13, 13);
            this.lblServoOffset.TabIndex = 16;
            this.lblServoOffset.Text = "..";
            // 
            // label28
            // 
            this.label28.AutoSize = true;
            this.label28.Location = new System.Drawing.Point(7, 156);
            this.label28.Name = "label28";
            this.label28.Size = new System.Drawing.Size(66, 13);
            this.label28.TabIndex = 15;
            this.label28.Text = "Servo Offset";
            // 
            // txtServoOffset
            // 
            this.txtServoOffset.Location = new System.Drawing.Point(111, 155);
            this.txtServoOffset.Name = "txtServoOffset";
            this.txtServoOffset.Size = new System.Drawing.Size(37, 20);
            this.txtServoOffset.TabIndex = 17;
            this.txtServoOffset.Text = "0";
            this.txtServoOffset.Visible = false;
            // 
            // btnServoOffset
            // 
            this.btnServoOffset.Location = new System.Drawing.Point(154, 153);
            this.btnServoOffset.Name = "btnServoOffset";
            this.btnServoOffset.Size = new System.Drawing.Size(47, 23);
            this.btnServoOffset.TabIndex = 26;
            this.btnServoOffset.Text = "Set";
            this.btnServoOffset.UseVisualStyleBackColor = true;
            this.btnServoOffset.Visible = false;
            // 
            // lblResistanceStdLevel
            // 
            this.lblResistanceStdLevel.AutoSize = true;
            this.lblResistanceStdLevel.Font = new System.Drawing.Font("Microsoft Sans Serif", 36F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lblResistanceStdLevel.Location = new System.Drawing.Point(57, 16);
            this.lblResistanceStdLevel.Name = "lblResistanceStdLevel";
            this.lblResistanceStdLevel.Size = new System.Drawing.Size(51, 55);
            this.lblResistanceStdLevel.TabIndex = 27;
            this.lblResistanceStdLevel.Text = "0";
            this.lblResistanceStdLevel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // frmIrtGui
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(784, 561);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox6);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.txtLog);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MaximumSize = new System.Drawing.Size(800, 600);
            this.MinimumSize = new System.Drawing.Size(800, 600);
            this.Name = "frmIrtGui";
            this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
            this.Text = "E-Motion Test Tool";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.pnlResistanceStd.ResumeLayout(false);
            this.pnlErg.ResumeLayout(false);
            this.pnlErg.PerformLayout();
            this.pnlResistanceSim.ResumeLayout(false);
            this.pnlResistanceSim.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txtLog;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lblEmrFirmwareRev;
        private System.Windows.Forms.Label lblEmrSerialNo;
        private System.Windows.Forms.TextBox txtEmrDeviceId;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label lblEmrModel;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label lblEmrHardwareRev;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label lblRefPwrManuf;
        private System.Windows.Forms.TextBox txtRefPwrDeviceId;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label lblRefPwrType;
        private System.Windows.Forms.Label lblRefPwrModel;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.Button btnEmrSearch;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Button btnRefPwrSearch;
        private System.Windows.Forms.Label lblEmrBattVolt;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Label lblEmrBattTime;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.TextBox txtSlope;
        private System.Windows.Forms.TextBox txtOffset;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label label18;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Button btnCalibrationSet;
        private System.Windows.Forms.Label lblChargeStatus;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Panel pnlResistanceStd;
        private System.Windows.Forms.Button btnResistanceSet;
        private System.Windows.Forms.ComboBox cmbResistanceMode;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.Panel pnlErg;
        private System.Windows.Forms.TextBox txtResistanceErgWatts;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Label label26;
        private System.Windows.Forms.TextBox txtTotalWeight;
        private System.Windows.Forms.Panel pnlResistanceSim;
        private System.Windows.Forms.TextBox textBox4;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.Label label24;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.CheckedListBox chkLstSettings;
        private System.Windows.Forms.Button btnSettingsSet;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.TextBox txtWheelSizeMm;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.TextBox txtParamSet;
        private System.Windows.Forms.Button btnParamGet;
        private System.Windows.Forms.Button btnParamSet;
        private System.Windows.Forms.TextBox txtParamGet;
        private System.Windows.Forms.TextBox txtServoOffset;
        private System.Windows.Forms.Label lblServoOffset;
        private System.Windows.Forms.Label label28;
        private System.Windows.Forms.Button btnServoOffset;
        private System.Windows.Forms.Label lblResistanceStdLevel;
    }
}

