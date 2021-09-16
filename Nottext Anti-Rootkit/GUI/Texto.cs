using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GUI
{
    public partial class Texto : Form
    {
        string cp;

        /// <summary>
        /// Inicia tudo
        /// </summary>
        /// <param name="mensagem"></param>
        public Texto(string mensagem, string alocar, string textoBox)
        {
            InitializeComponent();
            label1.Text = mensagem;

            textBox1.Text = textoBox;
            cp = alocar;
        }

        /// <summary>
        /// Retorna o texto
        /// </summary>
        /// <returns></returns>
        public string RetornarTexto()
        {
            return cp;
        }

        /// <summary>
        /// OK
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OK_Click(object sender, EventArgs e)
        {
            cp = textBox1.Text;
            Close();
        }
    }
}
