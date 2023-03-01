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
    public partial class AutoProtecaoAlerta : Form
    {
        bool permiteFechar = false;

        /// <summary>
        /// Carrega o form
        /// </summary>
        public AutoProtecaoAlerta(string pid, string arquivo)
        {
            InitializeComponent();
            label2.Text = "PID: "+ pid + "\r\nArquivo do processo: " + arquivo;
        }

        /// <summary>
        /// Quando estiver fechando
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void AutoProtecaoAlerta_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (permiteFechar == false)
                e.Cancel = true;
        }

        /// <summary>
        /// Botão de sair
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void realizarAcao_Click(object sender, EventArgs e)
        {
            permiteFechar = true;
            Close();
        }

        /// <summary>
        /// Resumir processo
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void resumirProcesso_CheckedChanged(object sender, EventArgs e)
        {
            if (resumirProcesso.Checked == true)
            {
                bloquearProcesso.Checked = false;
                terminarProcesso.Checked = false;
            }
        }

        /// <summary>
        /// Terminar processo alterar
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void terminarProcesso_CheckedChanged(object sender, EventArgs e)
        {
            if (terminarProcesso.Checked == true)
            {
                resumirProcesso.Checked = false;
            }
        }

        /// <summary>
        /// Bloquear processo
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void bloquearProcesso_CheckedChanged(object sender, EventArgs e)
        {
            if (bloquearProcesso.Checked == true)
            {
                resumirProcesso.Checked = false;
            }
        }
    }
}
