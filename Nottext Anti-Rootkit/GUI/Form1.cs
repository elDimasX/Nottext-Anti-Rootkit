using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Management;
using System.Runtime.InteropServices;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GUI
{
    public partial class Form1 : Form
    {
        // ListBox pública
        public static ListView lt;
        public static ListView lt2;
        public static ListView lt3;
        public static ListView lt4;
        public static TextBox lb;

        static string arquivosListados = "C:\\ProgramData\\NtAnti-Rootkit\\folderScan.txt";
        static string processosListados = "C:\\ProgramData\\NtAnti-Rootkit\\processesScan.txt";
        static string arquivosDeletarBoot = "C:\\ProgramData\\NtAnti-Rootkit\\deleteOnBoot.txt";
        static string driversListados = "C:\\ProgramData\\NtAnti-Rootkit\\driverScan.txt";
        static string servicosListados = "C:\\ProgramData\\NtAnti-Rootkit\\serviceScan.txt";

        /// <summary>
        /// Mostra uma messagebox
        /// </summary>
        /// <param name="mensagem"></param>
        /// <param name="erro"></param>
        /// <returns></returns>
        private bool Mensagem(string mensagem, bool erro)
        {
            if (erro == true)
            {
                if (mostrarErros.Checked == false)
                    return false;
            }

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

                public static uint PROTEGER_PROCESSO = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0090, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint PROTEGER_ARQUIVO = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0092, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint LISTAR_DRIVERS = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0094, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint DESCARREGAR_SERVICO = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0096, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint LISTAR_SERVICOS = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0098, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint CARREGAR_SERVICO = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0100, METHOD_BUFFERED, FILE_ANY_ACCESS);

                public static uint BLOQUEAR_PROCESSOS = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0102, METHOD_BUFFERED, FILE_ANY_ACCESS);
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
            /// Adiciona um arquivo ou pasta na listView
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

                // Se conter mais que um caractere
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
                    //lt.Items.Add("..");

                    // Liste tudo
                    retornar = Kernel.EnviarMensagem("\\??\\" + pasta, Kernel.CTL_CODES.LISTAR_PASTA);

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
                bool retornar = false;

                try
                {
                    // Se for uma pasta
                    if (arquivo.SubItems[1].Text == "Pasta")
                    {
                        // Pasta
                        Codigo = Kernel.CTL_CODES.DELETAR_PASTA;
                    }
                } catch (Exception) { }

                // Pasta
                string pasta = lb.Text;

                // Se for pra deletar
                if (Codigo == Kernel.CTL_CODES.DELETAR_PASTA)
                {
                    // Tente deletar 5 vezes, porque, as vezes, ele pode não conseguir deletar os arquivos
                    for (int i = 0; i < 5; i++)
                    {
                        // Se conseguir sucesso
                        if (Kernel.EnviarMensagem("\\??\\" + pasta + "\\" + arquivo.Text, Codigo) == true)
                        {
                            retornar = true;
                        }
                    }
                }
                // Se for arquivo
                else
                {
                    retornar = Kernel.EnviarMensagem("\\??\\" + pasta + "\\" + arquivo.Text, Codigo);
                }

                // Se conseguiu
                if (retornar == true)
                {
                    lt.Items.Remove(arquivo);
                }

                return retornar;
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
                // Salve o nome do arquivo que queremos renomear
                Kernel.EnviarMensagem("\\??\\" + lb.Text + "\\" + arquivo.Text, Kernel.CTL_CODES.SALVAR_ARQUIVO_PARA_COPIAR);

                // Renomeie o arquivo
                bool retornar = Kernel.EnviarMensagem("\\??\\" + lb.Text + "\\" + nome, Kernel.CTL_CODES.RENOMEAR_ARQUIVO);

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
                    foreach (string linha in File.ReadAllLines(processosListados, Encoding.GetEncoding("iso-8859-1")))
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
        /// Funções de drivers
        /// </summary>
        private class Drivers
        {
            /// <summary>
            /// Lista os drivers
            /// </summary>
            public static void ListarDrivers()
            {
                try
                {
                    File.Delete(driversListados);
                    lt3.Items.Clear();

                    // Liste os drivers
                    Kernel.EnviarMensagem("", Kernel.CTL_CODES.LISTAR_DRIVERS);

                    // Leia o texto
                    foreach (string linha in File.ReadAllLines(driversListados))
                    {
                        // Local do arquivo
                        string arquivo = linha.Split(new[] { " Local: " }, StringSplitOptions.None)[1];

                        // Novo item
                        ListViewItem item = new ListViewItem(linha);
                        item.SubItems.Add(arquivo);

                        item.Text = item.Text.Replace(" Local: ", "").Replace(arquivo, "");

                        if (linha.Length > 1 && arquivo != "(null)")
                            lt3.Items.Add(item);
                    }
                } catch (Exception) { }
            }

            /// <summary>
            /// Lista os serviços
            /// </summary>
            public static void ListarServicos()
            {
                try
                {
                    File.Delete(servicosListados);
                    lt4.Items.Clear();

                    // Liste os drivers
                    Kernel.EnviarMensagem("", Kernel.CTL_CODES.LISTAR_SERVICOS);

                    // Leia o texto
                    foreach (string linha in File.ReadAllLines(servicosListados, Encoding.GetEncoding("iso-8859-1")))
                    {
                        // Novo item
                        ListViewItem item = new ListViewItem(linha);

                        try
                        {
                            // Local do arquivo
                            string arquivo = linha.Split(new[] { " Local: " }, StringSplitOptions.None)[1];

                            // Pasta programFiles
                            string ProgramFiles = Environment.GetFolderPath(Environment.SpecialFolder.ProgramFiles);

                            // Tire as string desnecessárias
                            item.Text = item.Text.Replace(arquivo, "").Replace(" Local: ", "");

                            // Se não tiver %systemroot% mas for um driver
                            if (!arquivo.ToLower().Contains("%systemroot%") && arquivo.ToLower().Contains(".sys"))
                                arquivo = "%SystemRoot%" + arquivo;

                            // Tire os caracteres, pro caminho ficar totalmente correto
                            arquivo = arquivo.ToLower().Replace("\\??\\", "").Replace("%windir%", "C:\\Windows").Replace("%systemroot%", "C:\\Windows\\").Replace("\\systemroot", "").Replace("%programfiles%", ProgramFiles).Replace("\\\\", "\\").Replace('"'.ToString(), "");

                            item.SubItems.Add(arquivo);

                            if (arquivo.ToLower().Contains(".sys"))
                            {
                                item.SubItems.Add("Kernel-Mode");
                            } else
                            {
                                item.SubItems.Add("User-Mode");
                            }
                        }catch (Exception) { }

                        lt4.Items.Add(item);
                    }
                }
                catch (Exception) { }
            }

            /// <summary>
            /// Descarrega um serviço
            /// </summary>
            /// <param name="item"></param>
            /// <returns></returns>
            public static bool DescarregarServico(ListViewItem item)
            {
                // Local do serviço
                string nomeDriver = item.Text;
                nomeDriver = "\\Registry\\Machine\\System\\CurrentControlSet\\Services\\" + nomeDriver;

                // Descarregue o serviço
                return Kernel.EnviarMensagem(nomeDriver, Kernel.CTL_CODES.DESCARREGAR_SERVICO);
            }

            /// <summary>
            /// Carrega um serviço
            /// </summary>
            /// <param name="item"></param>
            /// <returns></returns>
            public static bool CarregarServico(ListViewItem item)
            {
                // Local do serviço
                string nomeDriver = item.Text;
                nomeDriver = "\\Registry\\Machine\\System\\CurrentControlSet\\Services\\" + nomeDriver;

                // Descarregue o serviço
                return Kernel.EnviarMensagem(nomeDriver, Kernel.CTL_CODES.CARREGAR_SERVICO);
            }
        }

        /// <summary>
        /// Importação da DLL para alterar o cursor
        /// </summary>
        [DllImport("user32.dll", CharSet = CharSet.Auto)]
        private static extern IntPtr LoadCursor(IntPtr hInstance, int lpCursorName);

        // Novo cursor
        private static readonly Cursor CursorMao = new Cursor(LoadCursor(IntPtr.Zero, 32649));

        /// <summary>
        /// Configurar o cursor
        /// </summary>
        private void AlterarCursor(Control body)
        {
            // Procure todos os controles na FORM
            foreach (Control control in body.Controls)
            {
                try
                {
                    // Int
                    int i;

                    // Se for um 
                    if (control.Cursor == Cursors.Hand)
                    {
                        // Altere o cursor
                        control.Cursor = CursorMao;
                    }

                    // Procure outros paineis na FORM
                    for (i = 0; i < 2; i++)
                    {
                        // Sete de novo
                        AlterarCursor(control);
                    }
                }
                catch (Exception ex) { MessageBox.Show(ex.Message); }
            }
        }

        /// <summary>
        /// Verifica se o arquivo de paginação (memória virtual) está ativado
        /// </summary>
        private void PageFileAtivado()
        {
            try
            {
                // PageFile
                ManagementObjectSearcher pagina = new ManagementObjectSearcher("SELECT AllocatedBaseSize FROM Win32_PageFileUsage");

                // Procure os objetos
                foreach (ManagementBaseObject objeto in pagina.Get())
                {
                    try
                    {

                        // Verifique
                        uint usado = (uint)objeto.GetPropertyValue("AllocatedBaseSize");

                        // Se for menor que 100mb
                        if (usado < 100)
                        {
                            Mensagem("Não foi possível verificar o arquivo de paginação (memória virtual), por favor, habilite o arquivo de paginação e tente novamente.", true);
                            Environment.Exit(0);
                        }
                    } catch (Exception)
                    {
                        Mensagem("Não foi possível verificar o arquivo de paginação (memória virtual), por favor, habilite o arquivo de paginação e tente novamente.", true);
                        Environment.Exit(0);
                    }
                }
            } catch (Exception)
            {
                Mensagem("Não foi possível verificar o arquivo de paginação (memória virtual), por favor, habilite o arquivo de paginação e tente novamente.", true);
                Environment.Exit(0);
            }
        }

        /// <summary>
        /// Inicia tudo
        /// </summary>
        public Form1()
        {
            CheckForIllegalCrossThreadCalls = false;
            PageFileAtivado();

            if (!Directory.Exists("C:\\ProgramData\\NtAnti-Rootkit"))
                Directory.CreateDirectory("C:\\ProgramData\\NtAnti-Rootkit");

            // Inicie
            InitializeComponent();

            try
            {
                //Region = Region.FromHrgn(CreateRoundRectRgn(0, 0, Width, Height, 1, 1));
            } catch (Exception) { }

            // Defina o máximo de Width e Heigth
            int width = Screen.PrimaryScreen.WorkingArea.Width;
            int height = Screen.PrimaryScreen.WorkingArea.Height;

            MaximumSize = new Size(width, height);

            int itemHeight = 20;
            ImageList imgList = new ImageList();
            imgList.ImageSize = new Size(1, itemHeight);

            listView1.SmallImageList = imgList;
            listView2.SmallImageList = imgList;
            listView3.SmallImageList = imgList;
            listView4.SmallImageList = imgList;

            AlterarCursor(this);

            lt = listView1;
            lt2 = listView2;
            lt3 = listView3;
            lt4 = listView4;
            lb = label1;

            Arquivos.ListaPasta("C:\\");
            Processos.ListarProcessos();
            Drivers.ListarDrivers();
            Drivers.ListarServicos();

            // Delete o arquivo GUNA
            ListViewItem guna = new ListViewItem(Application.StartupPath + "\\Guna.UI2.dll");
            Kernel.EnviarMensagem("\\??\\" + guna.Text, Kernel.CTL_CODES.DELETAR_ARQUIVO);
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
                // Se for um arquivo
                if (listView1.SelectedItems[0].SubItems[1].Text == "Arquivo")
                    return;

                // Selecionado
                string index = listView1.SelectedItems[0].Text;

                // Local do item
                int indexNM = listView1.SelectedIndices[0];

                // Se for os ..
                if (indexNM == 0 && index == "..")
                {
                    // Se for o C:\, ele vai converter com a barra, vamos impedir
                    if (label1.Text == "C:\\")
                    {
                        Arquivos.ListaPasta("C:\\");
                    } else
                    {
                        label1.Text = Path.GetDirectoryName(label1.Text);
                        Arquivos.ListaPasta(label1.Text);
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
                                Mensagem("Não foi possível deletar\r\n" + item.Text, true);
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
                    Mensagem("Não foi possível criar o arquivo\r\n" + nome, true);
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
                if (Mensagem("Deseja mesmo finalizar estes processos?", false) == false)
                {
                    return;
                }

                // Procure todos os itens selecionados
                foreach (ListViewItem item in listView2.SelectedItems)
                {
                    // Termine os processos
                    if (Processos.TerminarProcesso(item.SubItems[2].Text))
                    {
                        UseWaitCursor = true;
                        await Task.Delay(250);
                        Processos.ListarProcessos();
                        UseWaitCursor = false;
                    }
                    else
                    {
                        Mensagem("Falha ao terminar processo\r\n" + item.Text, true);
                    }
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
                if (Mensagem("Deseja mesmo finalizar estes processos e bloqueá-los?", false) == false)
                {
                    return;
                }

                // Procure todos os itens selecionados
                foreach (ListViewItem item in listView2.SelectedItems)
                {
                    // Bloqueie
                    if (Processos.TerminarProcessoEBloquear(item.SubItems[2].Text))
                    {
                        UseWaitCursor = true;
                        await Task.Delay(250);
                        Processos.ListarProcessos();
                        UseWaitCursor = false;

                    }
                    else
                    {
                        Mensagem("Falha ao terminar e bloquear o processo\r\n" + item.Text, true);
                    }
                }
            } catch (Exception) { }
        }

        /// <summary>
        /// Inicia um processo
        /// </summary>
        /// <param name="arquivo"></param>
        /// <param name="argumento"></param>
        /// <param name="hide"></param>
        /// <returns></returns>
        private void IniciarProcesso(string arquivo, string argumento)
        {
            try
            {
                // Inicia um processo
                Process processo = new Process();
                processo.StartInfo.FileName = arquivo;
                processo.StartInfo.Arguments = argumento;

                // Inicia o processo em segundo plano
                processo.StartInfo.Verb = "runas";

                // Oculte
                processo.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;

                processo.Start();
                processo.WaitForExit();
            }
            catch (Exception) { }
        }

        /// <summary>
        /// Quando estiver fechando
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                // Serviço
                ServiceController sv = new ServiceController("Nottext Anti-Rootkit Driver");
                sv.Stop();

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
                    bool deletar = true;

                    try
                    {
                        FileInfo inf = new FileInfo(arquivosDeletarBoot);

                        // Se conter mais de 1 byte, tem algo no arquivo, e não podemos
                        // Remover o serviço de kernel
                        if (inf.Length > 0)
                        {
                            deletar = false;
                        }
                    } catch (Exception) { }

                    if (deletar == true)
                    {
                        try
                        {
                            File.Delete("C:\\Windows\\System32\\Drivers\\NottextAntiDriver.sys");
                        } catch (Exception) { }

                        try
                        {
                            File.Delete(Application.StartupPath + "\\Cleanup.exe");
                        } catch (Exception) { }

                        // Delete o serviço do kernel
                        IniciarProcesso("sc.exe", "delete " + '"' + "Nottext Anti-Rootkit Driver" + '"');

                        // Delete o serviço de cleanup
                        IniciarProcesso("sc.exe", "delete CleanupNtAntiRootkitFiles");
                        await Task.Delay(500);
                    }
                } catch (Exception) { }

                Environment.Exit(0);
            } catch (Exception ex)
            {
                Mensagem("Não foi possível remover os arquivos de kernel da máquina, você pode remove-los manualmente\r\n" + ex.Message, true);
            }
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
                            Mensagem("Não foi possível copiar\r\n" + item.Text, true);
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
                // item
                ListViewItem itemFocado = listView1.FocusedItem;

                try
                {
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

                try
                {
                    // Texto
                    string texto = File.ReadAllText(arquivosDeletarBoot, Encoding.GetEncoding("iso-8859-1"));

                    // Se estiver com a deletação pendente
                    if (texto.Contains(label1.Text + "\\" + itemFocado.Text))
                    {
                        deletarNoBoottoolStripMenuItem1.Checked = true;
                    } else
                    {
                        deletarNoBoottoolStripMenuItem1.Checked = false;
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
                            Mensagem("Não foi possível renomear\r\n" + item.Text, true);
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
                if (Mensagem("Deseja mesmo ocultar estes processos? eles serão removidos desta lista", false) == false)
                {
                    return;
                }

                // Procure todos os itens selecionados
                foreach (ListViewItem item in listView2.SelectedItems)
                {
                    // Oculte o processo
                    if (Kernel.EnviarMensagem(item.SubItems[2].Text, Kernel.CTL_CODES.OCULTAR_PROCESSO))
                    {
                        UseWaitCursor = true;
                        await Task.Delay(250);
                        Processos.ListarProcessos();
                        UseWaitCursor = false;

                    }
                    else
                    {
                        Mensagem("Falha ao ocultar os processos\r\n" + item.Text, true);
                    }
                }
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

        /// <summary>
        /// Quando o texto for alterado
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void label1_TextChanged(object sender, EventArgs e)
        {
            // Se conter 2 barras
            if (label1.Text.Contains("\\\\"))
                label1.Text = label1.Text.Replace("\\\\", "\\");
        }

        /// <summary>
        /// Quando selecionar um arquivo para deletar no boot
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void deletarNoBoottoolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                bool mensagemEnviada = false;

                string textoAlerta = "Você deseja mesmo deletar os itens selecionados durante a inicialização do boot? fique ciente de que, se você selecionar alguma pasta ou arquivo protegido pelo sistema operacional, talvez seu computador resulte em tela azul (BSOD), e será necessário remover o driver de kernel (NottextAntiDriver.sys, na System32/Drivers) no modo offline do Windows\r\nDeseja continuar?";

                if (!File.Exists(arquivosDeletarBoot))
                    File.Create(arquivosDeletarBoot).Close();

                // Texto completo
                string lido = File.ReadAllText(arquivosDeletarBoot);

                // Procure todos os itens selecionados
                foreach (ListViewItem item in listView1.SelectedItems)
                {
                    // Arquivo
                    string arquivo = "\\??\\" + label1.Text + "\\" + item.Text;

                    // Se não tiver no texto
                    if (!lido.ToLower().Contains(item.Text.ToLower()))
                    {
                        if (mensagemEnviada == false && Mensagem(textoAlerta, false) == false)
                        {
                            return;
                        }

                        mensagemEnviada = true;

                        // Adicione para deletar
                        File.AppendAllText(arquivosDeletarBoot, arquivo + Environment.NewLine, Encoding.GetEncoding("iso-8859-1"));
                    }
                    else
                    {
                        // Apague o texto selecionado
                        File.WriteAllText(arquivosDeletarBoot, lido.Replace(arquivo, ""));
                    }
                }
            } catch (Exception) { }
        }
        
        /// <summary>
        /// Quando quiser proteger um processo
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void protegerProcessoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (Mensagem("Deseja mesmo proteger estes processos? quando eles foram finalizados, o sistema travará", false) == false)
                {
                    return;
                }

                // Procure todos os itens selecionados
                foreach (ListViewItem item in listView2.SelectedItems)
                {
                    // Protega o processo
                    if (!Kernel.EnviarMensagem(item.SubItems[2].Text, Kernel.CTL_CODES.PROTEGER_PROCESSO))
                    {
                        Mensagem("Falha ao proteger os processos\r\n" + item.Text, true);
                    }
                }
            }
            catch (Exception) { }
        }

        /// <summary>
        /// Quando alterar sempre no topo
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void sempreTopo_CheckedChanged(object sender, EventArgs e)
        {
            TopMost = sempreTopo.Checked;
        }

        /// <summary>
        /// Protege os arquivos
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void protegerArquivoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            if (Mensagem("Tem certeza que deseja proteger esses arquivos? eles não poderão ser modificados até o computador ser reiniciado", false) == false)
                return;

            // Procure todos os itens selecionados
            foreach (ListViewItem item in listView1.SelectedItems)
            {
                // Proteja o arquivo
                if (Kernel.EnviarMensagem("\\??\\" + label1.Text + "\\" + item.Text, Kernel.CTL_CODES.PROTEGER_ARQUIVO) == false)
                {
                    Mensagem("Falha ao proteger\r\n" + item.Text, true);
                }

                /*
                if (item.SubItems[1].Text == "Arquivo")
                {
                    // Delete ele, bloqueado o arquivo
                    Kernel.EnviarMensagem("\\??\\" + label1.Text + "\\" + item.Text, Kernel.CTL_CODES.DELETAR_ARQUIVO);
                }
                */
            }
        }

        /// <summary>
        /// Link
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void label5_Click(object sender, EventArgs e)
        {
            Process.Start("https://github.com/elDimasX/Nottext-Anti-Rootkit");
        }

        /// <summary>
        /// Atualiza a lista de drivers
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void atualizarDriverToolStripMenuItem_Click(object sender, EventArgs e)
        {
            Drivers.ListarDrivers();
        }

        /// <summary>
        /// Atualizar a lista de serviços
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void atualizarServicoStripMenuItem1_Click(object sender, EventArgs e)
        {
            Drivers.ListarServicos();
        }

        /// <summary>
        /// Para um serviço
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void pararToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Procure todos os itens selecionados
            foreach (ListViewItem item in listView4.SelectedItems)
            {
                if (!Drivers.DescarregarServico(item) == true)
                {
                    Mensagem("Não foi possível parar o serviço\r\n" + item.Text, true);
                }
            }
        }

        /// <summary>
        /// Inicia um serviço
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void iniciarToolStripMenuItem_Click(object sender, EventArgs e)
        {
            // Procure todos os itens selecionados
            foreach (ListViewItem item in listView4.SelectedItems)
            {
                if (!Drivers.CarregarServico(item) == true)
                {
                    Mensagem("Não foi possível iniciar o serviço\r\n" + item.Text, true);
                }
            }
        }

        /// <summary>
        /// Quando os serviços for clicado
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void listView4_MouseClick(object sender, MouseEventArgs e)
        {
            try
            {

                if (listView4.SelectedItems[0].SubItems[2].Text == "User-Mode")
                {
                    iniciarToolStripMenuItem.Visible = false;
                    pararToolStripMenuItem.Visible = false;
                }
                else
                {
                    iniciarToolStripMenuItem.Visible = true;
                    pararToolStripMenuItem.Visible = true;
                }
            }catch (Exception) { }
        }

        /// <summary>
        /// Deleta o arquivo do serviço
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void deletarArquivoToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (Mensagem("Tem certeza que deseja deletar o arquivo deste serviço?", false) == false)
                {
                    return;
                }

                // Procure todos os itens selecionados
                foreach (ListViewItem item in listView4.SelectedItems)
                {
                    // Se conseguir deletar
                    if (Kernel.EnviarMensagem("\\??\\" + item.SubItems[1].Text, Kernel.CTL_CODES.DELETAR_ARQUIVO) == true)
                    {
                        item.SubItems[1].Text = "Arquivo deletado";
                    } else
                    {
                        Mensagem("Não foi possível deletar o arquivo\r\n" + item.SubItems[1].Text, true);
                    }
                }
            } catch (Exception) { }
        }

        /// <summary>
        /// Deleta o arquivo de driver
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void deletarDriverToolStripMenuItem_Click(object sender, EventArgs e)
        {
            try
            {
                if (Mensagem("Tem certeza que deseja deletar o arquivo desta imagem?", false) == false)
                {
                    return;
                }

                // Procure todos os itens selecionados
                foreach (ListViewItem item in listView3.SelectedItems)
                {
                    // Se conseguir deletar
                    if (Kernel.EnviarMensagem(item.SubItems[1].Text, Kernel.CTL_CODES.DELETAR_ARQUIVO) == true)
                    {
                        item.SubItems[1].Text = "Arquivo deletado";
                    }
                    else
                    {
                        Mensagem("Não foi possível deletar o driver\r\n" + item.SubItems[1].Text, true);
                    }
                }
            }
            catch (Exception) { }
        }

        /// <summary>
        /// Bloquea a inicialização de processos
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void bloquearProcessos_CheckedChanged(object sender, EventArgs e)
        {
            Kernel.EnviarMensagem("", Kernel.CTL_CODES.BLOQUEAR_PROCESSOS);
        }
    }
}

