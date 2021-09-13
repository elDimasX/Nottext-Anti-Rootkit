using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.ServiceProcess;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace GUI
{
    static class Program
    {
        /// <summary>
        /// Inicia um processo
        /// </summary>
        /// <param name="arquivo"></param>
        /// <param name="argumento"></param>
        /// <param name="hide"></param>
        /// <returns></returns>
        public static async Task IniciarProcesso(string arquivo, string argumento)
        {
            try
            {
                // Inicia um processo
                Process processo = new Process();
                processo.StartInfo.FileName = arquivo;
                processo.StartInfo.Arguments = argumento;

                await Task.Delay(1); // Intervalo

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
        /// Carrega o driver
        /// </summary>
        private static async Task CarregarDriver()
        {
            try
            {
                // Pastas dos drivers
                string pastaDrivers = Application.StartupPath + "\\Drivers\\";

                try
                {
                    Directory.Delete(pastaDrivers, true);
                } catch (Exception) { }

                try
                {
                    File.Delete(Application.StartupPath + "\\KernelProcessList.exe");
                } catch (Exception) { }

                // Crie a pasta
                Directory.CreateDirectory(pastaDrivers);

                // 64 bits
                if (Environment.Is64BitProcess)
                {
                    // Salve o driver de x86
                    File.WriteAllBytes(pastaDrivers + "NottextAntiDriver.sys", Properties.Resources.NottextAntiDriverX64);
                }
                else
                {
                    // Salve o driver de x86
                    File.WriteAllBytes(pastaDrivers + "NottextAntiDriver.sys", Properties.Resources.NottextAntiDriverX86);
                }

                // Salve o programa que atualiza os processos
                File.WriteAllBytes(Application.StartupPath + "\\KernelProcessList.exe", Properties.Resources.KernelProcessList);

                // Salve o CAT
                File.WriteAllBytes(pastaDrivers + "deleteonboot.cat", Properties.Resources.deleteonboot);

                // Salve o INF
                File.WriteAllBytes(pastaDrivers + "NottextAntiDriver.inf", Properties.Resources.NottextAntiDriverInf);

                // Arquivo INF
                string inf = pastaDrivers + "NottextAntiDriver.inf";

                // Inicie o processo
                await IniciarProcesso("cmd.exe", "/c C:\\Windows\\System32\\InfDefaultInstall.exe " + '"' + inf + '"');

                // Serviço
                ServiceController kernel = new ServiceController("Nottext Anti-Rootkit Driver");

                try
                {
                    // Pare, se estiver sendo executado
                    kernel.Stop();
                }
                catch (Exception) { }

                kernel.Start();

            }
            catch (Exception ex)
            {
                if (ex.Message.Contains("Não é possível iniciar o serviço"))
                {
                    MessageBox.Show("Não foi possível extrair os arquivos necessários\r\nAtive o modo de teste do Windows para que seja possível continuar", "Nottext Anti-Rootkit", MessageBoxButtons.OK, MessageBoxIcon.Error);
                } else
                {
                    MessageBox.Show("Não foi possível extrair os arquivos necessários\r\n" + ex.Message, "Nottext Anti-Rootkit", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                
                Environment.Exit(0);
            }
        }

        /// <summary>
        /// Ponto de entrada principal para o aplicativo.
        /// </summary>
        [STAThread]
        static void Main()
        {
            //Application.EnableVisualStyles();
            //Application.SetCompatibleTextRenderingDefault(false);
            CarregarDriver().Wait();

            Application.Run(new Form1());
        }
    }
}
