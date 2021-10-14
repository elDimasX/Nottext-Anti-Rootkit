using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GUI
{
    public partial class Texto : Form
    {
        string cp;

        /// <summary>
        /// Importadação da DLL para arrendodar e deixar a interface "feia"
        /// </summary>
        /// <param name="nLeftRect"></param>
        /// <param name="nTopRect"></param>
        /// <param name="nRightRect"></param>
        /// <param name="nBottomRect"></param>
        /// <param name="nWidthEllipse"></param>
        /// <param name="nHeightEllipse"></param>
        /// <returns></returns>
        [DllImport("Gdi32.dll", EntryPoint = "CreateRoundRectRgn")]
        private static extern IntPtr CreateRoundRectRgn
        (
            int nLeftRect,     // x-coordinate of upper-left corner
            int nTopRect,      // y-coordinate of upper-left corner
            int nRightRect,    // x-coordinate of lower-right corner
            int nBottomRect,   // y-coordinate of lower-right corner
            int nWidthEllipse, // height of ellipse
            int nHeightEllipse // width of ellipse
        );

        /// <summary>
        /// Inicia tudo
        /// </summary>
        /// <param name="mensagem"></param>
        public Texto(string mensagem, string alocar, string textoBox)
        {
            InitializeComponent();

            try
            {
                Region = Region.FromHrgn(CreateRoundRectRgn(0, 0, Width, Height, 1, 1));
            }
            catch (Exception) { }

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
