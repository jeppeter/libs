using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Net;

namespace VirtualKDSetup
{
    public partial class VirtualBoxIntegrationForm : Form
    {
        VirtualBoxClient _Client = new VirtualBoxClient();

        public VirtualBoxIntegrationForm()
        {
            InitializeComponent();

            label2.Text = textBox1.Text = _Client.Version;
            if (_Client.Is64Bit)
            {
                label2.Text += " (64-bit)";
                comboBox1.SelectedIndex = 1;
            }
            else
            {
                label2.Text += " (32-bit)";
                comboBox1.SelectedIndex = 0;
            }

            label4.Text = _Client.VirtualBoxPath;
            UpdateInstallButtons();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            textBox1.Visible = comboBox1.Visible = true;
            button1.Visible = label2.Visible = false;
            label1.Text = "Forced VirtualBox version:";
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == DialogResult.OK)
            {
                label4.Text = Path.GetDirectoryName(openFileDialog1.FileName);
                label3.Text = "Forced VirtualBox path:";

                _Client = new VirtualBoxClient(label4.Text);
                UpdateInstallButtons();
            }
        }

        string FileSourceToString(VirtualBoxClient.FileSource src)
        {
            switch(src)
            {
                case VirtualBoxClient.FileSource.Missing:
                    return "not found";
                case VirtualBoxClient.FileSource.VirtualBox:
                    return "original VirtualBox";
                case VirtualBoxClient.FileSource.VirtualKD:
                    return "patched VirtualKD";
                default:
                    return "unknown version";
            }
        }

        void UpdateInstallButtons()
        {
            label10.Text = FileSourceToString(_Client.VBoxDD);
            label12.Text = FileSourceToString(_Client.VBoxDD0);
            switch (_Client.State)
            {
                case VirtualBoxClient.IntegrationState.NotInstalled:
                    lblWarning.Visible = false;
                    btnAuto.Visible = true;
                    comboBox2.SelectedIndex = 0;
                    break;
                case VirtualBoxClient.IntegrationState.Successful:
                    lblWarning.Text = "VirtualKD is already integrated into VirtualBox. Re-integrating it might break VirtualBox directory and require reinstalling it. If you know what you're doing, press \"integrate manually\".";
                    lblWarning.Visible = true;
                    btnAuto.Visible = false;
                    comboBox2.SelectedIndex = 1;
                    break;
                case VirtualBoxClient.IntegrationState.VBoxReinstallRequired:
                    lblWarning.Text = "Seems like VirtualBox DLLs are corrupt or have been replaced incorrectly. It is recommended to reinstall VirtualBox. You can also try to fix it manually by pressing \"integrate manually\".";
                    lblWarning.Visible = true;
                    btnAuto.Visible = false;
                    comboBox2.SelectedIndex = -1;
                    break;
                case VirtualBoxClient.IntegrationState.Unknown:
                    lblWarning.Text = "Seems like VirtualBox DLLs are corrupt or have been replaced incorrectly. You can also try to fix it manually by pressing \"integrate manually\".";
                    lblWarning.Visible = true;
                    btnAuto.Visible = false;
                    comboBox2.SelectedIndex = -1;
                    break;
            }

        }

        private void button3_Click(object sender, EventArgs e)
        {
            panel1.Visible = true;
            button3.Visible = false;
            button2.Visible = false;

            btnAuto.Visible = true;
            btnAuto.Text = "Integrate VirtualKD with VirtualBox";
            lblWarning.Visible = false;
        }

        private void btnAuto_Click(object sender, EventArgs e)
        {
            if (comboBox2.SelectedIndex == -1)
            {
                MessageBox.Show("Please select an installation method", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (_Client.VirtualBoxPath == null)
            {
                MessageBox.Show("Please select VirtualBox path", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (textBox1.Text == "")
            {
                MessageBox.Show("Please specify VirtualBox version", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            WebClient clt = new WebClient();
            byte[] binaryFile = null;
            btnAuto.Enabled = false;
            try
            {
                if (!checkBox1.Checked)
                {
                    string URL = clt.DownloadString(string.Format("http://virtualkd.sysprogs.org/cgi-bin/getbin.pl?ver={0}&platform={1}", textBox1.Text, comboBox1.Text));
                    if (URL.StartsWith("http://"))
                        binaryFile = DownloadProgressForm.DownloadToArray(URL, "Downloading precompiled VboxDD.dll", null);
                }
            }
            catch (System.Exception)
            {
            }
            btnAuto.Enabled = true;

            if (binaryFile == null)
            {
                if (!checkBox1.Checked)
                    if (MessageBox.Show("VirtualKD setup was not able to download the precompiled VBoxDD.dll for your version of VirtualBox. Do you want to build it from source code?", "Error", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != DialogResult.Yes)
                        return;

                binaryFile = VBoxBuildForm.FetchAndBuild(textBox1.Text, (comboBox1.SelectedIndex == 1));
                if (binaryFile == null)
                    return;
            }

            string dd = _Client.VirtualBoxPath + @"\VBoxDD.dll";
            string dd0 = _Client.VirtualBoxPath + @"\VBoxDD0.dll";
            try
            {
                if (comboBox2.SelectedIndex == 0)
                {
                    if (!File.Exists(dd))
                        throw new FileNotFoundException("VBoxDD.dll is missing. Nothing to rename!");
                    if (File.Exists(dd0))
                        File.Delete(dd0);
                    File.Move(dd, dd0);
                }
                else
                {
                    if (File.Exists(dd))
                        File.Delete(dd);
                }

                using (FileStream fs = File.Create(dd))
                    fs.Write(binaryFile, 0, binaryFile.Length);

                MessageBox.Show("Integration successful", "Information", MessageBoxButtons.OK, MessageBoxIcon.Information);
                DialogResult = DialogResult.OK;
            }
            catch (System.Exception ex)
            {
                MessageBox.Show(ex.Message, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            
        }
    }
}
