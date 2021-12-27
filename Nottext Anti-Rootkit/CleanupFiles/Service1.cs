using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.ServiceProcess;
using System.Threading.Tasks;

namespace CleanupFiles
{
    public partial class Service1 : ServiceBase
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
        /// Inicia o serviço
        /// </summary>
        public Service1()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Quando for iniciado
        /// </summary>
        /// <param name="args"></param>
        protected override void OnStart(string[] args)
        {
            // Primeiro, apague este serviço
            IniciarProcesso("sc.exe", "delete CleanupNtAntiRootkitFiles").Wait();

            // Apague o drivers
            IniciarProcesso("sc.exe", "delete " + '"' + "Nottext Anti-Rootkit Driver" + '"').Wait();
            IniciarProcesso("sc.exe", "stop " + '"' + "Nottext Anti-Rootkit Driver" + '"').Wait();

            try
            {
                File.Delete("C:\\Windows\\System32\\Drivers\\NottextAntiDriver.sys");
            } catch (Exception) { }

            try
            {
                Directory.Delete(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location) + "\\Drivers", true);
            }
            catch (Exception) { }

            try
            {
                File.Delete(Path.GetDirectoryName(Assembly.GetEntryAssembly().Location) + "\\KernelProcessList.exe");
            }
            catch (Exception) { }

            try
            {
                string local = '"' + Assembly.GetExecutingAssembly().Location + '"';
                string texto = "timeout 2\r\ndel /f " + local + "\r\ndel /f C:\\cleanup.bat";

                File.WriteAllText("C:\\cleanup.bat", texto);
                Process.Start("C:\\cleanup.bat");
            } catch (Exception) { }

            Environment.Exit(0);
        }

        /// <summary>
        /// Quando parar
        /// </summary>
        protected override void OnStop()
        {
        }
    }
}
