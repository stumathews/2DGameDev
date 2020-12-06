using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ContentManager
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
           
            
        }

        private void button1_Click(object sender, EventArgs e)
        {
            var file = openFileDialog1.ShowDialog();
            dataSet1.ReadXml(openFileDialog1.FileName);
            dataGridView1.DataSource = dataSet1.Tables[0];
        }
    }
}
