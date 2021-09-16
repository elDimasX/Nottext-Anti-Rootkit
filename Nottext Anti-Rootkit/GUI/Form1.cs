using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security.Principal;
using System.ServiceProcess;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GUI
{
    public partial class Form1 : Form
    {
        // ListBox pública
        public static ListView lt;
        public static ListView lt2;
        public static Label lb;

        static string arquivosListados = "C:\\ProgramData\\folderScan.txt";
        static string processosListados = "C:\\ProgramData\\processesScan.txt";

        /// <summary>
        /// Mostra uma messagebox
        /// </summary>
        /// <param name="mensagem"></param>
        /// <param name="erro"></param>
        /// <returns></returns>
        private bool Mensagem(string mensagem, bool erro)
        {
            bool retornar = true;
            var icone = MessageBoxIcon.Information;

            if (erro == true)
            {
                icone = MessageBoxIcon.Error;
            }

            // Se for um a pergunta
            if (mensagem.Contains("?"))
            {
                // Verifique se ele clicou no OK
                retornar = MessageBox.Show(mensagem, "Nottext Anti-Rootkit", MessageBoxButtons.OKCancel, icone) == DialogResult.OK;
            } else
            {
                // Mostre uma mensagem
                MessageBox.Show(mensagem, "Nottext Anti-Rootkit", MessageBoxButtons.OK, icone);
            }

            return retornar;
        }

        /// <summary>
        /// Funções de kernel
        /// </summary>
        private class Kernel
        {
            /// Importação da DLL KERNEL32.DLL CreateFile
            /// </summary>
            /// <returns></returns>
            [DllImport("Kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
            public static extern int CreateFile(
                String lpFileName, // Nome da porta
                int dwDesiredAccess, // Acesso
                int dwShareMode, // Compartilhamento
                IntPtr lpSecurityAttributes, // Security
                int dwCreationDisposition, // Disposition
                int dwFlagsAndAttributes, // Atributos
                int hTemplateFile // Arquivo
            );

            /// <summary>
            /// Envia uma mensagem
            /// </summary>
            [DllImport("kernel32.dll", SetLastError = true)]
            public static extern int DeviceIoControl(
                IntPtr hDevice,
                uint dwIoControlCode,
                StringBuilder lpInBuffer,
                int nInBufferSize,
                StringBuilder lpOutBuffer,
                Int32 nOutBufferSize,
                ref Int32 lpBytesReturned,
                IntPtr lpOverlapped
            );

            /// <summary>
            /// Fechar o dispositivo
            /// </summary>
            [DllImport("kernel32", SetLastError = true)]
            public static extern bool CloseHandle(
                IntPtr handle // O que fechar
            );

            // Definições, necessárias
            public static uint FILE_DEVICE_UNKNOWN = 0x00000022;
            public static uint FILE_ANY_ACCESS = 0;
            public static uint METHOD_BUFFERED = 0;
            public static int GENERIC_WRITE = 0x40000000;
            public static int GENERIC_READ = unchecked((int)0x80000000);
            public static int FILE_SHARE_READ = 1;
            public static int FILE_SHARE_WRITE = 2;
            public static int OPEN_EXISTING = 3;
            public static int IOCTL_DISK_GET_DRIVE_LAYOUT_EX = unchecked((int)0x00070050);
            public static int ERROR_INSUFFICIENT_BUFFER = 122;

            /// <summary>
            /// CTL_CODE, para que o driver conheça nossa necessidade
            /// </summary>
            private static uint CTL_CODE(uint DeviceType, uint Function, uint Method, uint Access)
            {
                return ((DeviceType << 16) | (Access << 14) | (Function << 2) | Method);
            }

            /// <summary>
            /// Todos os códigos CTL
            /// </summary>
            public static class CTL_CODES
            {
                // Avisa para o driver que queremos deletar um arquivo
                public static uint LISTAR_PASTA = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0059, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint LISTAR_PROCESSOS = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0064, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint DELETAR_ARQUIVO = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0068, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint CRIAR_ARQUIVO = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0070, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint TERMINAR_PROCESSO = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0072, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint TERMINAR_PROCESSO_E_BLOQUEAR = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0074, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint DELETAR_PASTA= CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0076, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint SALVAR_ARQUIVO_PARA_COPIAR = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0078, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint COPIAR_ARQUIVO = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0080, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint RENOMEAR_ARQUIVO = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0082, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint OCULTAR_PROCESSO = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0084, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint DESLIGAR_COMPUTADOR = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0086, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint REINICIAR_COMPUTADOR = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0088, METHOD_BUFFERED, FILE_ANY_ACCESS);
            }

            /// <summary>
            /// Envia uma mensagem ao nosso driver
            /// </summary>
            /// <param name="mensagem"></param>
            /// <param name="Codigo"></param>
            public static bool EnviarMensagem(string mensagem, uint Ctl)
            {
                bool StatusRetornar = false;

                try
                {
                    // Nosso dispositivo de comunicação com o driver
                    IntPtr dispositivo = (IntPtr)CreateFile(
                        "\\\\.\\NtAntiRtDriver",
                        GENERIC_READ | GENERIC_WRITE,
                        FILE_SHARE_READ,
                        IntPtr.Zero,
                        OPEN_EXISTING,
                        0,
                        0
                    );

                    // Novos bytes
                    int uCnt = 10;

                    // Mensagem para o kernel
                    StringBuilder enviarAoKernel = new StringBuilder(mensagem);

                    // Mensagem para receber do kernel
                    StringBuilder receberDoKernel = new StringBuilder();

                    // Envie a mensagem
                    DeviceIoControl(

                        // Dispositivo
                        dispositivo,

                        // Nosso código CTL
                        Ctl,

                        // Mensagem para enviar
                        enviarAoKernel,

                        // + 5, para não enviar lixo ao kernel
                        enviarAoKernel.Length + 5,

                        // Receber o kernel
                        receberDoKernel,
                        1,
                        ref uCnt,
                        IntPtr.Zero
                    );

                    // Se for um sucesso
                    if (receberDoKernel.ToString() == "success!")
                    {
                        // Sucesso
                        StatusRetornar = true;
                    }

                    // Feche o dispositivo
                    CloseHandle(dispositivo);
                }
                catch (Exception) { }

                // Status
                return StatusRetornar;
            }

        }

        /// <summary>
        /// Funções de arquivos
        /// </summary>
        private class Arquivos
        {
            /// <summary>
            /// Converte bytes para uma string corretamente
            /// </summary>
            /// <param name="byteCount"></param>
            /// <returns></returns>
            static string BytesParaString(long bytesContar)
            {
                // Os tamanhos
                string[] tipos = { "B", "KB", "MB", "GB", "TB", "PB", "EB" };

                // Se for 0
                if (bytesContar == 0)
                    return "0" + tipos[0];

                // Converta
                long bytes = Math.Abs(bytesContar);

                // Converter para int
                int lugar = Convert.ToInt32(Math.Floor(Math.Log(bytes, 1024)));

                // Math
                double num = Math.Round(bytes / Math.Pow(1024, lugar), 1);

                // Sign
                return (Math.Sign(bytesContar) * num).ToString() + tipos[lugar];
            }

            /// <summary>
            /// Adiciona um arquivo ou pasta na listBox
            /// </summary>
            /// <param name="arquivo"></param>
            public static void AdicionarArquivoOuPasta(string arquivo)
            {
                string tipo = "";

                // Se for arquivo
                if (arquivo.Contains("Arquivo : "))
                {
                    tipo = "Arquivo";
                }
                // Se for pasta
                else
                {
                    tipo = "Pasta";
                }

                // Tamanho
                string tamanho = "";
                string tamanhoConvertido = "";

                // Se for um arquivo
                if (tipo == "Arquivo")
                {
                    // Obtenha o resultado depois da palavra Tamanho: 
                    tamanho = arquivo.Split(new[] { "Tamanho: " }, StringSplitOptions.None)[1];

                    // Tamanho, coonvertido corretamente
                    tamanhoConvertido = BytesParaString(Int32.Parse(tamanho));
                }

                // Cor
                Color cor = Color.Black;

                // Nome do arquivo original
                string arquivoOriginal = arquivo.Replace("\\??\\", "")
                    .Replace("Arquivo : ", "")
                    .Replace("Pasta : ", "")
                    .Replace(" : Tamanho: ", "");

                // Novo item
                ListViewItem item = new ListViewItem(arquivoOriginal);

                // Se for maior
                if (tamanho.Length > 0)
                    item.Text = item.Text.Replace(tamanho, "");

                 // Se não conseguimos listar
                if (!File.Exists(lb.Text + "\\"+ item.Text))
                {
                    if (!Directory.Exists(lb.Text + "\\" + item.Text))
                        cor = Color.Red;
                }

                // Cor
                item.ForeColor = cor;

                // Adicione o tipo
                item.SubItems.Add(tipo);
                item.SubItems.Add(tamanhoConvertido);

                // Adicione
                lt.Items.Add(item);
            }

            /// <summary>
            /// Lista uma pasta
            /// </summary>
            /// <param name="pasta"></param>
            public static bool ListaPasta(string pasta)
            {
                try
                {
                    File.Delete(arquivosListados);
                } catch (Exception) { }

                bool retornar = false;

                try
                {
                    // Limpe tudo
                    lt.Items.Clear();
                    lt.Items.Add("..");

                    // Se for C:
                    if (lb.Text.ToLower() == "c:")
                    {
                        retornar = Kernel.EnviarMensagem("\\??\\C:\\", Kernel.CTL_CODES.LISTAR_PASTA);
                    } else
                    {
                        // Liste tudo
                        retornar = Kernel.EnviarMensagem("\\??\\" + pasta, Kernel.CTL_CODES.LISTAR_PASTA);
                    }

                    // Procure todos os arquivos
                    foreach (string arquivo in File.ReadAllLines(arquivosListados, Encoding.GetEncoding("iso-8859-1")))
                    {
                        AdicionarArquivoOuPasta(arquivo);
                    }
                } catch (Exception)
                {
                }

                return retornar;
            }

            /// <summary>
            /// Cria um arquivo
            /// </summary>
            /// <param name="arquivo"></param>
            /// <returns></returns>
            public static bool CriarArquivo(string arquivo)
            {
                bool retornar = Kernel.EnviarMensagem("\\??\\" + arquivo, Kernel.CTL_CODES.CRIAR_ARQUIVO);

                if (retornar == true)
                {
                    try
                    {
                        File.Delete(arquivosListados);
                    } catch (Exception) { }

                    // Liste a pasta de novo
                    ListaPasta(lb.Text);
                }

                return retornar;
            }

            /// <summary>
            /// Deleta um arquivo
            /// </summary>
            /// <param name="arquivo"></param>
            /// <returns></returns>
            public static bool DeletarArquivo(ListViewItem arquivo)
            {
                // Deletar arquivo
                uint Codigo = Kernel.CTL_CODES.DELETAR_ARQUIVO;

                // Se for uma pasta
                if (arquivo.SubItems[1].Text == "Pasta")
                {
                    // Pasta
                    Codigo = Kernel.CTL_CODES.DELETAR_PASTA;
                }

                // Pasta
                string pasta = lb.Text;

                if (Kernel.EnviarMensagem("\\??\\" + pasta + "\\" + arquivo.Text, Codigo) == true)
                {
                    lt.Items.Remove(arquivo);

                    return true;
                }

                return false;
            }

            /// <summary>
            /// Copia um arquivo
            /// </summary>
            /// <param name="arquivo"></param>
            /// <param name="local"></param>
            /// <returns></returns>
            public static bool CopiarArquivo(ListViewItem arquivo, string local)
            {
                bool retornar = false;

                // Salve o nome do arquivo que queremos copiar
                Kernel.EnviarMensagem("\\??\\" + lb.Text + "\\" + arquivo.Text, Kernel.CTL_CODES.SALVAR_ARQUIVO_PARA_COPIAR);

                // Copie o arquivo
                retornar = Kernel.EnviarMensagem("\\??\\" + local, Kernel.CTL_CODES.COPIAR_ARQUIVO);

                return retornar;
            }

            /// <summary>
            /// Renomeia um arquivo
            /// </summary>
            /// <param name="arquivo"></param>
            /// <param name="nome"></param>
            /// <returns></returns>
            public static bool RenomearArquivo(ListViewItem arquivo, string nome)
            {
                bool retornar = false;

                // Salve o nome do arquivo que queremos renomear
                Kernel.EnviarMensagem("\\??\\" + lb.Text + "\\" + arquivo.Text, Kernel.CTL_CODES.SALVAR_ARQUIVO_PARA_COPIAR);

                // Renomeie o arquivo
                retornar = Kernel.EnviarMensagem("\\??\\" + lb.Text + "\\" + nome, Kernel.CTL_CODES.RENOMEAR_ARQUIVO);

                return retornar;
            }
        }

        /// <summary>
        /// Funções de processos
        /// </summary>
        private class Processos
        {
            /// <summary>
            /// Lista os processos na máquina
            /// </summary>
            public static void ListarProcessos()
            {
                try
                {
                    try
                    {
                        File.Delete(processosListados);
                    } catch (Exception) { }

                    lt2.Items.Clear();

                    // Inicie o processo
                    Process pp = new Process();
                    pp.StartInfo.FileName = Application.StartupPath + "\\KernelProcessList.exe";
                    pp.StartInfo.Arguments = "";
                    pp.StartInfo.UseShellExecute = false;
                    pp.Start();
                    pp.WaitForExit();

                    // Leia linha por linha
                    foreach (string linha in File.ReadAllLines(processosListados))
                    {
                        // PID
                        string pid = linha.Split(new[] { "PID: " }, StringSplitOptions.None)[1];

                        // Threads
                        string threads = linha.Split(new[] { "Threads: " }, StringSplitOptions.None)[1];

                        // Remova o nome threads e PID
                        threads = threads.Replace("Threads: ", "").Replace(" : PID: " + pid, "");

                        // Processo
                        string processo = linha;

                        // Remover depois disso
                        int index = processo.LastIndexOf(": Threads: ");

                        // Remova-o
                        processo = processo.Substring(0, index);

                        // Nome
                        ListViewItem item = new ListViewItem(processo);
                        item.SubItems.Add(threads);
                        item.SubItems.Add(pid);

                        lt2.Items.Add(item);
                    }
                } catch (Exception) { }
            }

            /// <summary>
            /// Termina um processo
            /// </summary>
            /// <param name="pid"></param>
            /// <returns></returns>
            public static bool TerminarProcesso(string pid)
            {
                // Falhou
                return Kernel.EnviarMensagem(pid, Kernel.CTL_CODES.TERMINAR_PROCESSO);
            }

            /// <summary>
            /// Termina um processo e bloqueia o arquivo
            /// </summary>
            /// <param name="pid"></param>
            /// <returns></returns>
            public static bool TerminarProcessoEBloquear(string pid)
            {
                // Falhou
                return Kernel.EnviarMensagem(pid, Kernel.CTL_CODES.TERMINAR_PROCESSO_E_BLOQUEAR);
            }
        }

        /// <summary>
        /// Inicia tudo
        /// </summary>
        public Form1()
        {
            CheckForIllegalCrossThreadCalls = false;

            // Inicie
            InitializeComponent();

            lt = listView1;
            lt2 = listView2;
            lb = label1;

            Arquivos.ListaPasta("C:\\");
            Processos.ListarProcessos();
        }

        /// <summary>
        /// Dois cliques em algum item do ListBox
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void listView1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            try
            {
                // Selecionado
                string index = listView1.SelectedItems[0].Text;

                // Local do item
                int indexNM = listView1.SelectedIndices[0];

                // Se for os ..
                if (indexNM == 0)
                {
                    if (label1.Text == "C:" || label1.Text == "C:\\")
                    {
                        Arquivos.ListaPasta("C:\\");
                        label1.Text = "C:";
                    } else
                    {
                        // Se for o C:\, ele vai converter com a barra, vamos impedir
                        if (Path.GetDirectoryName(label1.Text) == "C:\\")
                        {
                            label1.Text = "C:";
                            Arquivos.ListaPasta("C:\\");
                        } else
                        {
                            label1.Text = Path.GetDirectoryName(label1.Text);
                            Arquivos.ListaPasta(label1.Text);
                        }

                        
                    }
                }
                else
                {
                    label1.Text = label1.Text + "\\" + index;
                    Arquivos.ListaPasta(label1.Text);
                }
            } catch (Exception) { }
        }

        /// <summary>
        /// Deleta um arquivo
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void deleteFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                // Confirme
                if (Mensagem("Tem certeza que deseja deletar os itens selecionados?", false) == true)
                {
                    // Lista
                    List<ListViewItem> items = new List<ListViewItem>();

                    foreach (ListViewItem item in listView1.SelectedItems)
                    {
                        // Adicione
                        items.Add(item);
                    }

                    // Procure
                    foreach (ListViewItem item in items)
                    {
                        try
                        {
                            // Delete o arquivo
                            if (Arquivos.DeletarArquivo(item) == false)
                            {
                                // Mostre a mensagem
                                Mensagem("Não foi possível deletar", true);
                            }
                        } catch (Exception) { }
                    }

                    items.Clear();
                }
            } catch (Exception) { }
        }

        /// <summary>
        /// Quando criar um arquivo
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void criarArquivoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                string nome = "";
                Texto texto = new Texto("Digite o nome do arquivo: ", nome, "");
                texto.ShowDialog();

                // Nova string
                nome = texto.RetornarTexto();

                if (nome == "")
                {
                    return;
                }

                // Se não cosneguir criar um arquivo
                if (Arquivos.CriarArquivo(label1.Text + "\\" + nome) == false)
                {
                    Mensagem("Não foi possível criar o arquivo", true);
                }
            } catch (Exception) { }
            
        }

        /// <summary>
        /// Atualiza a pasta
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void atualizarToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Se não conter o \
            if (!label1.Text.Contains("\\"))
            {
                Arquivos.ListaPasta(label1.Text + "\\");

                // Pare
                return;
            }

            Arquivos.ListaPasta(label1.Text);
        }
        
        /// <summary>
        /// Quando listar os processos
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void listarProcessosStripMenuItem_Click(object sender, EventArgs e)
        {
            Processos.ListarProcessos();
        }

        /// <summary>
        /// Finaliza um processo
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void finalizarProcessoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {

                // Obtenha somente o PID
                string item = listView2.SelectedItems[0].SubItems[2].Text;

                if (Mensagem("Deseja mesmo finalizar o processo?", false) == false)
                {
                    return;
                }

                if (Processos.TerminarProcesso(item))
                {
                    UseWaitCursor = true;
                    await Task.Delay(250);
                    Processos.ListarProcessos();
                    UseWaitCursor = false;
                }
                else
                {
                    Mensagem("Falha ao terminar processo", true);
                }
            } catch (Exception) { }
        }

        /// <summary>
        /// Finalizar e bloquear
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void finalizarBloquearToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {

                // Obtenha somente o ID
                string item = listView2.SelectedItems[0].SubItems[2].Text;

                if (Mensagem("Deseja mesmo finalizar o processo e bloqueá-lo?", false) == false)
                {
                    return;
                }

                // Bloqueie
                if (Processos.TerminarProcessoEBloquear(item))
                {
                    UseWaitCursor = true;
                    await Task.Delay(250);
                    Processos.ListarProcessos();
                    UseWaitCursor = false;

                }
                else
                {
                    Mensagem("Falha ao terminar e bloquear o processo", true);
                }
            } catch (Exception) { }
        }

        /// <summary>
        /// Quando estiver fechando
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            // Delete o serviço
            Process pp = new Process();
            pp.StartInfo.FileName = "sc.exe";
            pp.StartInfo.Arguments = "delete " + '"' + "Nottext Anti-Rootkit Driver" + '"';
            pp.StartInfo.UseShellExecute = false;
            pp.StartInfo.CreateNoWindow = true;
            pp.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            pp.Start();
            pp.WaitForExit();

            try
            {
                // Delete a pasta
                Directory.Delete(Application.StartupPath + "\\Drivers", true);
            } catch (Exception) { }

            try
            {
                // Arquivo de listagem
                File.Delete(Application.StartupPath + "\\KernelProcessList.exe");
            } catch (Exception) { }

            try
            {
                // Serviço
                ServiceController sv = new ServiceController("Nottext Anti-Rootkit Driver");

                try
                {
                    sv.Stop();
                } catch (Exception) { }

                try
                {
                    File.Delete("C:\\Windows\\System32\\Drivers\\NottextAntiDriver.sys");
                } catch (Exception) { }

                Environment.Exit(0);
            }
            catch (Exception) { }
        }

        /// <summary>
        /// Copia um arquivo
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void copiarArquivoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Lista
            List<ListViewItem> items = new List<ListViewItem>();

            foreach (ListViewItem item in listView1.SelectedItems)
            {
                // Adicione
                items.Add(item);
            }

            // Procure
            foreach (ListViewItem item in items)
            {
                try
                {
                    string local = "";

                    Texto txt = new Texto("Digite o local do arquivo para copiar: " + item.Text, local, label1.Text + "\\" + item.Text);

                    txt.ShowDialog();

                    // Obtenha o texto
                    local = txt.RetornarTexto();

                    // Se não for vázio
                    if (local != "")
                    {
                        // Delete o arquivo
                        if (Arquivos.CopiarArquivo(item, local) == false)
                        {
                            // Mostre a mensagem
                            Mensagem("Não foi possível copiar", true);
                        }
                    }
                } catch (Exception) { }
            }

            items.Clear();
            Arquivos.ListaPasta(label1.Text);
        }

        /// <summary>
        /// Quando clicar no item
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void listView1_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                try
                {
                    // item
                    ListViewItem itemFocado = listView1.FocusedItem;

                    // Se conter
                    if (itemFocado.SubItems[1].Text == "Pasta")
                    {
                        copiarArquivoToolStripMenuItem.Visible = false;
                    }
                    else
                    {
                        copiarArquivoToolStripMenuItem.Visible = true;
                    }
                } catch (Exception) { }
            }
        }

        /// <summary>
        /// Quando clicar em renomear
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void renomearToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Lista
            List<ListViewItem> items = new List<ListViewItem>();

            foreach (ListViewItem item in listView1.SelectedItems)
            {
                // Adicione
                items.Add(item);
            }

            // Procure
            foreach (ListViewItem item in items)
            {
                try
                {
                    // Novo nome
                    string novoNome = "";

                    Texto txt = new Texto("Digite o novo nome do arquivo: " + item.Text, novoNome, item.Text);

                    txt.ShowDialog();

                    // Obtenha o texto
                    novoNome = txt.RetornarTexto();

                    // Se não for vázio
                    if (novoNome != "")
                    {
                        // Delete o arquivo
                        if (Arquivos.RenomearArquivo(item, novoNome) == false)
                        {
                            // Mostre a mensagem
                            Mensagem("Não foi possível renomear", true);
                        }
                    }
                }
                catch (Exception) { }
            }

            items.Clear();
            Arquivos.ListaPasta(label1.Text);
        }

        /// <summary>
        /// Quando ocultar um processo
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void ocultarProcessoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                // Obtenha somente o PID
                ListViewItem item = listView2.SelectedItems[0];

                if (Mensagem("Deseja mesmo ocultar o processo? ele será removido desta lista", false) == false)
                {
                    return;
                }

                // Oculte o processo
                Kernel.EnviarMensagem(item.SubItems[2].Text, Kernel.CTL_CODES.OCULTAR_PROCESSO);

                UseWaitCursor = true;
                await Task.Delay(250);
                Processos.ListarProcessos();
                UseWaitCursor = false;
            } catch (Exception) { }
        }

        /// <summary>
        /// Quando clicar em desligar
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void desligar_Click(object sender, EventArgs e)
        {
            // Se confirmar
            if (Mensagem("Tem certeza? seu computador será desligado agora", false) == true)
            {
                Kernel.EnviarMensagem("", Kernel.CTL_CODES.DESLIGAR_COMPUTADOR);
                Close();
            }
        }

        /// <summary>
        /// Reinicia o computador
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void reiniciar_Click(object sender, EventArgs e)
        {
            // Se confirmar
            if (Mensagem("Tem certeza? seu computador será reiniciado agora", false) == true)
            {
                Kernel.EnviarMensagem("", Kernel.CTL_CODES.REINICIAR_COMPUTADOR);
                Close();
            }
        }

        /// <summary>
        /// Link
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Process.Start("https://github.com/elDimasX/Nottext-Anti-Rootkit");
        }

        /// <summary>
        /// Vai para uma pasta localizada
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void irParaUmaPastaToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                string local = "";

                Texto txt = new Texto("Digite o local que deseja ir (cuidado): ", local, "");
                txt.ShowDialog();
                local = txt.RetornarTexto();

                if (!String.IsNullOrEmpty(local))
                {
                    // Altere a label
                    label1.Text = local;

                    // Liste
                    Arquivos.ListaPasta(local);
                }
            } catch (Exception) { }
        }
    }
}

