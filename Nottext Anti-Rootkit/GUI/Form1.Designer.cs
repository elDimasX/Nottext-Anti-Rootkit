
namespace GUI
{
    partial class Form1
    {
        /// <summary>
        /// Variável de designer necessária.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Limpar os recursos que estão sendo usados.
        /// </summary>
        /// <param name="disposing">true se for necessário descartar os recursos gerenciados; caso contrário, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Código gerado pelo Windows Form Designer

        /// <summary>
        /// Método necessário para suporte ao Designer - não modifique 
        /// o conteúdo deste método com o editor de código.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.menuArquivos = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deletarArquivosToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.deletarNoBoottoolStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.protegerArquivoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.criarArquivoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.copiarArquivoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.renomearToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.atualizarToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.irParaUmaPastaToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.menuProcessos = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.finalizarProcessoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.finalizarBloquearToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.protegerProcessoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ocultarProcessoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.listarProcessosStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.panel1 = new System.Windows.Forms.Panel();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage5 = new System.Windows.Forms.TabPage();
            this.listView3 = new System.Windows.Forms.ListView();
            this.columnHeader8 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader12 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.menuDrivers = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deletarDriverToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.atualizarDriverToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tabPage6 = new System.Windows.Forms.TabPage();
            this.listView4 = new System.Windows.Forms.ListView();
            this.columnHeader9 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader10 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader11 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.menuServicos = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deletarArquivoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.iniciarToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.pararToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.atualizarServicoStripMenuItem1 = new System.Windows.Forms.ToolStripMenuItem();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.label1 = new System.Windows.Forms.TextBox();
            this.listView1 = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader6 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.listView2 = new System.Windows.Forms.ListView();
            this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader4 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader5 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.bloquearProcessos = new System.Windows.Forms.CheckBox();
            this.mostrarErros = new System.Windows.Forms.CheckBox();
            this.desligar = new System.Windows.Forms.Button();
            this.sempreTopo = new System.Windows.Forms.CheckBox();
            this.reiniciar = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.arrastar = new Guna.UI2.WinForms.Guna2ResizeForm(this.components);
            this.columnHeader7 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.label6 = new System.Windows.Forms.Label();
            this.menuArquivos.SuspendLayout();
            this.menuProcessos.SuspendLayout();
            this.panel1.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage5.SuspendLayout();
            this.menuDrivers.SuspendLayout();
            this.tabPage6.SuspendLayout();
            this.menuServicos.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuArquivos
            // 
            this.menuArquivos.Font = new System.Drawing.Font("Leelawadee UI", 9F);
            this.menuArquivos.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deletarArquivosToolStripMenuItem,
            this.deletarNoBoottoolStripMenuItem1,
            this.protegerArquivoToolStripMenuItem,
            this.criarArquivoToolStripMenuItem,
            this.copiarArquivoToolStripMenuItem,
            this.renomearToolStripMenuItem,
            this.toolStripSeparator1,
            this.atualizarToolStripMenuItem,
            this.irParaUmaPastaToolStripMenuItem});
            this.menuArquivos.Name = "menuArquivos";
            this.menuArquivos.Size = new System.Drawing.Size(166, 186);
            // 
            // deletarArquivosToolStripMenuItem
            // 
            this.deletarArquivosToolStripMenuItem.ForeColor = System.Drawing.Color.Black;
            this.deletarArquivosToolStripMenuItem.Name = "deletarArquivosToolStripMenuItem";
            this.deletarArquivosToolStripMenuItem.Size = new System.Drawing.Size(165, 22);
            this.deletarArquivosToolStripMenuItem.Text = "Deletar";
            this.deletarArquivosToolStripMenuItem.Click += new System.EventHandler(this.deleteFileToolStripMenuItem_Click);
            // 
            // deletarNoBoottoolStripMenuItem1
            // 
            this.deletarNoBoottoolStripMenuItem1.ForeColor = System.Drawing.Color.Black;
            this.deletarNoBoottoolStripMenuItem1.Name = "deletarNoBoottoolStripMenuItem1";
            this.deletarNoBoottoolStripMenuItem1.Size = new System.Drawing.Size(165, 22);
            this.deletarNoBoottoolStripMenuItem1.Text = "Deletar no Boot";
            this.deletarNoBoottoolStripMenuItem1.Click += new System.EventHandler(this.deletarNoBoottoolStripMenuItem_Click);
            // 
            // protegerArquivoToolStripMenuItem
            // 
            this.protegerArquivoToolStripMenuItem.ForeColor = System.Drawing.Color.Black;
            this.protegerArquivoToolStripMenuItem.Name = "protegerArquivoToolStripMenuItem";
            this.protegerArquivoToolStripMenuItem.Size = new System.Drawing.Size(165, 22);
            this.protegerArquivoToolStripMenuItem.Text = "Proteger";
            this.protegerArquivoToolStripMenuItem.Click += new System.EventHandler(this.protegerArquivoToolStripMenuItem_Click);
            // 
            // criarArquivoToolStripMenuItem
            // 
            this.criarArquivoToolStripMenuItem.ForeColor = System.Drawing.Color.Black;
            this.criarArquivoToolStripMenuItem.Name = "criarArquivoToolStripMenuItem";
            this.criarArquivoToolStripMenuItem.Size = new System.Drawing.Size(165, 22);
            this.criarArquivoToolStripMenuItem.Text = "Criar arquivo";
            this.criarArquivoToolStripMenuItem.Click += new System.EventHandler(this.criarArquivoToolStripMenuItem_Click);
            // 
            // copiarArquivoToolStripMenuItem
            // 
            this.copiarArquivoToolStripMenuItem.ForeColor = System.Drawing.Color.Black;
            this.copiarArquivoToolStripMenuItem.Name = "copiarArquivoToolStripMenuItem";
            this.copiarArquivoToolStripMenuItem.Size = new System.Drawing.Size(165, 22);
            this.copiarArquivoToolStripMenuItem.Text = "Copiar arquivo";
            this.copiarArquivoToolStripMenuItem.Click += new System.EventHandler(this.copiarArquivoToolStripMenuItem_Click);
            // 
            // renomearToolStripMenuItem
            // 
            this.renomearToolStripMenuItem.ForeColor = System.Drawing.Color.Black;
            this.renomearToolStripMenuItem.Name = "renomearToolStripMenuItem";
            this.renomearToolStripMenuItem.Size = new System.Drawing.Size(165, 22);
            this.renomearToolStripMenuItem.Text = "Renomear";
            this.renomearToolStripMenuItem.Click += new System.EventHandler(this.renomearToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(162, 6);
            // 
            // atualizarToolStripMenuItem
            // 
            this.atualizarToolStripMenuItem.ForeColor = System.Drawing.Color.Black;
            this.atualizarToolStripMenuItem.Name = "atualizarToolStripMenuItem";
            this.atualizarToolStripMenuItem.Size = new System.Drawing.Size(165, 22);
            this.atualizarToolStripMenuItem.Text = "Atualizar";
            this.atualizarToolStripMenuItem.Click += new System.EventHandler(this.atualizarToolStripMenuItem_Click);
            // 
            // irParaUmaPastaToolStripMenuItem
            // 
            this.irParaUmaPastaToolStripMenuItem.ForeColor = System.Drawing.Color.Black;
            this.irParaUmaPastaToolStripMenuItem.Name = "irParaUmaPastaToolStripMenuItem";
            this.irParaUmaPastaToolStripMenuItem.Size = new System.Drawing.Size(165, 22);
            this.irParaUmaPastaToolStripMenuItem.Text = "Ir para uma pasta";
            this.irParaUmaPastaToolStripMenuItem.Click += new System.EventHandler(this.irParaUmaPastaToolStripMenuItem_Click);
            // 
            // menuProcessos
            // 
            this.menuProcessos.Font = new System.Drawing.Font("Leelawadee UI", 9F);
            this.menuProcessos.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.finalizarProcessoToolStripMenuItem,
            this.finalizarBloquearToolStripMenuItem,
            this.protegerProcessoToolStripMenuItem,
            this.ocultarProcessoToolStripMenuItem,
            this.toolStripSeparator2,
            this.listarProcessosStripMenuItem});
            this.menuProcessos.Name = "menuArquivos";
            this.menuProcessos.Size = new System.Drawing.Size(181, 120);
            // 
            // finalizarProcessoToolStripMenuItem
            // 
            this.finalizarProcessoToolStripMenuItem.ForeColor = System.Drawing.Color.Black;
            this.finalizarProcessoToolStripMenuItem.Name = "finalizarProcessoToolStripMenuItem";
            this.finalizarProcessoToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
            this.finalizarProcessoToolStripMenuItem.Text = "Finalizar processo";
            this.finalizarProcessoToolStripMenuItem.Click += new System.EventHandler(this.finalizarProcessoToolStripMenuItem_Click);
            // 
            // finalizarBloquearToolStripMenuItem
            // 
            this.finalizarBloquearToolStripMenuItem.ForeColor = System.Drawing.Color.Black;
            this.finalizarBloquearToolStripMenuItem.Name = "finalizarBloquearToolStripMenuItem";
            this.finalizarBloquearToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
            this.finalizarBloquearToolStripMenuItem.Text = "Finalizar && Bloquear";
            this.finalizarBloquearToolStripMenuItem.Click += new System.EventHandler(this.finalizarBloquearToolStripMenuItem_Click);
            // 
            // protegerProcessoToolStripMenuItem
            // 
            this.protegerProcessoToolStripMenuItem.ForeColor = System.Drawing.Color.Black;
            this.protegerProcessoToolStripMenuItem.Name = "protegerProcessoToolStripMenuItem";
            this.protegerProcessoToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
            this.protegerProcessoToolStripMenuItem.Text = "Proteger processo";
            this.protegerProcessoToolStripMenuItem.Click += new System.EventHandler(this.protegerProcessoToolStripMenuItem_Click);
            // 
            // ocultarProcessoToolStripMenuItem
            // 
            this.ocultarProcessoToolStripMenuItem.ForeColor = System.Drawing.Color.Black;
            this.ocultarProcessoToolStripMenuItem.Name = "ocultarProcessoToolStripMenuItem";
            this.ocultarProcessoToolStripMenuItem.Size = new System.Drawing.Size(180, 22);
            this.ocultarProcessoToolStripMenuItem.Text = "Ocultar processo";
            this.ocultarProcessoToolStripMenuItem.Click += new System.EventHandler(this.ocultarProcessoToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(177, 6);
            // 
            // listarProcessosStripMenuItem
            // 
            this.listarProcessosStripMenuItem.ForeColor = System.Drawing.Color.Black;
            this.listarProcessosStripMenuItem.Name = "listarProcessosStripMenuItem";
            this.listarProcessosStripMenuItem.Size = new System.Drawing.Size(180, 22);
            this.listarProcessosStripMenuItem.Text = "Atualizar";
            this.listarProcessosStripMenuItem.Click += new System.EventHandler(this.listarProcessosStripMenuItem_Click);
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.BackColor = System.Drawing.SystemColors.Control;
            this.panel1.Controls.Add(this.tabControl1);
            this.panel1.ForeColor = System.Drawing.Color.Black;
            this.panel1.Location = new System.Drawing.Point(3, 1);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(816, 555);
            this.panel1.TabIndex = 5;
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage5);
            this.tabControl1.Controls.Add(this.tabPage6);
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Location = new System.Drawing.Point(0, 3);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(816, 543);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage5
            // 
            this.tabPage5.Controls.Add(this.listView3);
            this.tabPage5.Location = new System.Drawing.Point(4, 24);
            this.tabPage5.Name = "tabPage5";
            this.tabPage5.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage5.Size = new System.Drawing.Size(808, 515);
            this.tabPage5.TabIndex = 4;
            this.tabPage5.Text = "Drivers";
            this.tabPage5.UseVisualStyleBackColor = true;
            // 
            // listView3
            // 
            this.listView3.Alignment = System.Windows.Forms.ListViewAlignment.Default;
            this.listView3.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listView3.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader8,
            this.columnHeader12});
            this.listView3.ContextMenuStrip = this.menuDrivers;
            this.listView3.ForeColor = System.Drawing.Color.Black;
            this.listView3.FullRowSelect = true;
            this.listView3.GridLines = true;
            this.listView3.HideSelection = false;
            this.listView3.Location = new System.Drawing.Point(0, 0);
            this.listView3.Name = "listView3";
            this.listView3.Size = new System.Drawing.Size(812, 510);
            this.listView3.Sorting = System.Windows.Forms.SortOrder.Ascending;
            this.listView3.TabIndex = 3;
            this.listView3.UseCompatibleStateImageBehavior = false;
            this.listView3.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader8
            // 
            this.columnHeader8.Text = "Imagem";
            this.columnHeader8.Width = 197;
            // 
            // columnHeader12
            // 
            this.columnHeader12.Text = "Local";
            this.columnHeader12.Width = 579;
            // 
            // menuDrivers
            // 
            this.menuDrivers.Font = new System.Drawing.Font("Leelawadee UI", 9F);
            this.menuDrivers.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deletarDriverToolStripMenuItem,
            this.toolStripSeparator4,
            this.atualizarDriverToolStripMenuItem});
            this.menuDrivers.Name = "menuArquivos";
            this.menuDrivers.Size = new System.Drawing.Size(145, 54);
            // 
            // deletarDriverToolStripMenuItem
            // 
            this.deletarDriverToolStripMenuItem.Name = "deletarDriverToolStripMenuItem";
            this.deletarDriverToolStripMenuItem.Size = new System.Drawing.Size(144, 22);
            this.deletarDriverToolStripMenuItem.Text = "Deletar driver";
            this.deletarDriverToolStripMenuItem.Click += new System.EventHandler(this.deletarDriverToolStripMenuItem_Click);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(141, 6);
            // 
            // atualizarDriverToolStripMenuItem
            // 
            this.atualizarDriverToolStripMenuItem.ForeColor = System.Drawing.Color.Black;
            this.atualizarDriverToolStripMenuItem.Name = "atualizarDriverToolStripMenuItem";
            this.atualizarDriverToolStripMenuItem.Size = new System.Drawing.Size(144, 22);
            this.atualizarDriverToolStripMenuItem.Text = "Atualizar";
            this.atualizarDriverToolStripMenuItem.Click += new System.EventHandler(this.atualizarDriverToolStripMenuItem_Click);
            // 
            // tabPage6
            // 
            this.tabPage6.BackColor = System.Drawing.Color.Transparent;
            this.tabPage6.Controls.Add(this.listView4);
            this.tabPage6.Location = new System.Drawing.Point(4, 24);
            this.tabPage6.Name = "tabPage6";
            this.tabPage6.Size = new System.Drawing.Size(808, 515);
            this.tabPage6.TabIndex = 0;
            this.tabPage6.Text = "Serviços";
            // 
            // listView4
            // 
            this.listView4.Alignment = System.Windows.Forms.ListViewAlignment.Default;
            this.listView4.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listView4.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader9,
            this.columnHeader10,
            this.columnHeader11});
            this.listView4.ContextMenuStrip = this.menuServicos;
            this.listView4.ForeColor = System.Drawing.Color.Black;
            this.listView4.FullRowSelect = true;
            this.listView4.GridLines = true;
            this.listView4.HideSelection = false;
            this.listView4.Location = new System.Drawing.Point(-3, 0);
            this.listView4.Name = "listView4";
            this.listView4.Size = new System.Drawing.Size(812, 513);
            this.listView4.Sorting = System.Windows.Forms.SortOrder.Ascending;
            this.listView4.TabIndex = 4;
            this.listView4.UseCompatibleStateImageBehavior = false;
            this.listView4.View = System.Windows.Forms.View.Details;
            this.listView4.MouseClick += new System.Windows.Forms.MouseEventHandler(this.listView4_MouseClick);
            // 
            // columnHeader9
            // 
            this.columnHeader9.Text = "Serviço";
            this.columnHeader9.Width = 233;
            // 
            // columnHeader10
            // 
            this.columnHeader10.Text = "Local";
            this.columnHeader10.Width = 441;
            // 
            // columnHeader11
            // 
            this.columnHeader11.Text = "Tipo";
            this.columnHeader11.Width = 97;
            // 
            // menuServicos
            // 
            this.menuServicos.Font = new System.Drawing.Font("Leelawadee UI", 9F);
            this.menuServicos.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deletarArquivoToolStripMenuItem,
            this.iniciarToolStripMenuItem,
            this.pararToolStripMenuItem,
            this.toolStripSeparator3,
            this.atualizarServicoStripMenuItem1});
            this.menuServicos.Name = "menuArquivos";
            this.menuServicos.Size = new System.Drawing.Size(155, 98);
            // 
            // deletarArquivoToolStripMenuItem
            // 
            this.deletarArquivoToolStripMenuItem.Name = "deletarArquivoToolStripMenuItem";
            this.deletarArquivoToolStripMenuItem.Size = new System.Drawing.Size(154, 22);
            this.deletarArquivoToolStripMenuItem.Text = "Deletar arquivo";
            this.deletarArquivoToolStripMenuItem.Click += new System.EventHandler(this.deletarArquivoToolStripMenuItem_Click);
            // 
            // iniciarToolStripMenuItem
            // 
            this.iniciarToolStripMenuItem.Name = "iniciarToolStripMenuItem";
            this.iniciarToolStripMenuItem.Size = new System.Drawing.Size(154, 22);
            this.iniciarToolStripMenuItem.Text = "Iniciar";
            this.iniciarToolStripMenuItem.Click += new System.EventHandler(this.iniciarToolStripMenuItem_Click);
            // 
            // pararToolStripMenuItem
            // 
            this.pararToolStripMenuItem.Name = "pararToolStripMenuItem";
            this.pararToolStripMenuItem.Size = new System.Drawing.Size(154, 22);
            this.pararToolStripMenuItem.Text = "Parar";
            this.pararToolStripMenuItem.Click += new System.EventHandler(this.pararToolStripMenuItem_Click);
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(151, 6);
            // 
            // atualizarServicoStripMenuItem1
            // 
            this.atualizarServicoStripMenuItem1.ForeColor = System.Drawing.Color.Black;
            this.atualizarServicoStripMenuItem1.Name = "atualizarServicoStripMenuItem1";
            this.atualizarServicoStripMenuItem1.Size = new System.Drawing.Size(154, 22);
            this.atualizarServicoStripMenuItem1.Text = "Atualizar";
            this.atualizarServicoStripMenuItem1.Click += new System.EventHandler(this.atualizarServicoStripMenuItem1_Click);
            // 
            // tabPage1
            // 
            this.tabPage1.BackColor = System.Drawing.SystemColors.Control;
            this.tabPage1.Controls.Add(this.label1);
            this.tabPage1.Controls.Add(this.listView1);
            this.tabPage1.Location = new System.Drawing.Point(4, 24);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(808, 515);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Arquivos";
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.ForeColor = System.Drawing.Color.Black;
            this.label1.Location = new System.Drawing.Point(3, 493);
            this.label1.MaxLength = 0;
            this.label1.Name = "label1";
            this.label1.ReadOnly = true;
            this.label1.Size = new System.Drawing.Size(799, 23);
            this.label1.TabIndex = 3;
            this.label1.Text = "C:\\";
            this.label1.TextChanged += new System.EventHandler(this.label1_TextChanged);
            // 
            // listView1
            // 
            this.listView1.Alignment = System.Windows.Forms.ListViewAlignment.Default;
            this.listView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2,
            this.columnHeader6});
            this.listView1.ContextMenuStrip = this.menuArquivos;
            this.listView1.ForeColor = System.Drawing.Color.Black;
            this.listView1.FullRowSelect = true;
            this.listView1.GridLines = true;
            this.listView1.HideSelection = false;
            this.listView1.Location = new System.Drawing.Point(0, 0);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(812, 487);
            this.listView1.TabIndex = 2;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            this.listView1.MouseClick += new System.Windows.Forms.MouseEventHandler(this.listView1_MouseClick);
            this.listView1.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.listView1_MouseDoubleClick);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Arquivo";
            this.columnHeader1.Width = 506;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "Tipo";
            this.columnHeader2.Width = 152;
            // 
            // columnHeader6
            // 
            this.columnHeader6.Text = "Tamanho";
            this.columnHeader6.Width = 125;
            // 
            // tabPage2
            // 
            this.tabPage2.BackColor = System.Drawing.Color.White;
            this.tabPage2.Controls.Add(this.listView2);
            this.tabPage2.Location = new System.Drawing.Point(4, 24);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(808, 515);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Processos";
            // 
            // listView2
            // 
            this.listView2.Alignment = System.Windows.Forms.ListViewAlignment.Default;
            this.listView2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listView2.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader3,
            this.columnHeader4,
            this.columnHeader5});
            this.listView2.ContextMenuStrip = this.menuProcessos;
            this.listView2.ForeColor = System.Drawing.Color.Black;
            this.listView2.FullRowSelect = true;
            this.listView2.GridLines = true;
            this.listView2.HideSelection = false;
            this.listView2.Location = new System.Drawing.Point(0, 0);
            this.listView2.Name = "listView2";
            this.listView2.Size = new System.Drawing.Size(812, 515);
            this.listView2.Sorting = System.Windows.Forms.SortOrder.Ascending;
            this.listView2.TabIndex = 3;
            this.listView2.UseCompatibleStateImageBehavior = false;
            this.listView2.View = System.Windows.Forms.View.Details;
            // 
            // columnHeader3
            // 
            this.columnHeader3.Text = "Processo";
            this.columnHeader3.Width = 315;
            // 
            // columnHeader4
            // 
            this.columnHeader4.Text = "Threads";
            this.columnHeader4.Width = 166;
            // 
            // columnHeader5
            // 
            this.columnHeader5.Text = "PID";
            this.columnHeader5.Width = 130;
            // 
            // tabPage3
            // 
            this.tabPage3.BackColor = System.Drawing.SystemColors.Control;
            this.tabPage3.Controls.Add(this.label6);
            this.tabPage3.Controls.Add(this.bloquearProcessos);
            this.tabPage3.Controls.Add(this.mostrarErros);
            this.tabPage3.Controls.Add(this.desligar);
            this.tabPage3.Controls.Add(this.sempreTopo);
            this.tabPage3.Controls.Add(this.reiniciar);
            this.tabPage3.Controls.Add(this.label2);
            this.tabPage3.Location = new System.Drawing.Point(4, 24);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage3.Size = new System.Drawing.Size(808, 515);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "Configurações";
            // 
            // bloquearProcessos
            // 
            this.bloquearProcessos.AutoSize = true;
            this.bloquearProcessos.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.bloquearProcessos.Location = new System.Drawing.Point(236, 123);
            this.bloquearProcessos.Name = "bloquearProcessos";
            this.bloquearProcessos.Size = new System.Drawing.Size(211, 19);
            this.bloquearProcessos.TabIndex = 6;
            this.bloquearProcessos.Text = "Bloquear inicialização de processos";
            this.bloquearProcessos.UseVisualStyleBackColor = true;
            this.bloquearProcessos.CheckedChanged += new System.EventHandler(this.bloquearProcessos_CheckedChanged);
            // 
            // mostrarErros
            // 
            this.mostrarErros.AutoSize = true;
            this.mostrarErros.Checked = true;
            this.mostrarErros.CheckState = System.Windows.Forms.CheckState.Checked;
            this.mostrarErros.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.mostrarErros.Location = new System.Drawing.Point(134, 123);
            this.mostrarErros.Name = "mostrarErros";
            this.mostrarErros.Size = new System.Drawing.Size(96, 19);
            this.mostrarErros.TabIndex = 5;
            this.mostrarErros.Text = "Mostrar erros";
            this.mostrarErros.UseVisualStyleBackColor = true;
            // 
            // desligar
            // 
            this.desligar.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.desligar.Location = new System.Drawing.Point(17, 49);
            this.desligar.Name = "desligar";
            this.desligar.Size = new System.Drawing.Size(87, 22);
            this.desligar.TabIndex = 4;
            this.desligar.Text = "DESLIGAR";
            this.desligar.UseVisualStyleBackColor = true;
            this.desligar.Click += new System.EventHandler(this.desligar_Click);
            // 
            // sempreTopo
            // 
            this.sempreTopo.AutoSize = true;
            this.sempreTopo.Cursor = System.Windows.Forms.Cursors.Arrow;
            this.sempreTopo.Location = new System.Drawing.Point(17, 123);
            this.sempreTopo.Name = "sempreTopo";
            this.sempreTopo.Size = new System.Drawing.Size(111, 19);
            this.sempreTopo.TabIndex = 3;
            this.sempreTopo.Text = "Sempre no topo";
            this.sempreTopo.UseVisualStyleBackColor = true;
            this.sempreTopo.CheckedChanged += new System.EventHandler(this.sempreTopo_CheckedChanged);
            // 
            // reiniciar
            // 
            this.reiniciar.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.reiniciar.Location = new System.Drawing.Point(110, 49);
            this.reiniciar.Name = "reiniciar";
            this.reiniciar.Size = new System.Drawing.Size(87, 22);
            this.reiniciar.TabIndex = 2;
            this.reiniciar.Text = "REINICIAR";
            this.reiniciar.UseVisualStyleBackColor = true;
            this.reiniciar.Click += new System.EventHandler(this.reiniciar_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 22);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(141, 15);
            this.label2.TabIndex = 1;
            this.label2.Text = "Opções de desligamento:";
            // 
            // tabPage4
            // 
            this.tabPage4.BackColor = System.Drawing.Color.Transparent;
            this.tabPage4.Controls.Add(this.label5);
            this.tabPage4.Controls.Add(this.label4);
            this.tabPage4.Controls.Add(this.label3);
            this.tabPage4.Location = new System.Drawing.Point(4, 24);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage4.Size = new System.Drawing.Size(808, 515);
            this.tabPage4.TabIndex = 3;
            this.tabPage4.Text = "Sobre";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.BackColor = System.Drawing.Color.Transparent;
            this.label5.Cursor = System.Windows.Forms.Cursors.Hand;
            this.label5.Font = new System.Drawing.Font("Yu Gothic UI", 9F);
            this.label5.ForeColor = System.Drawing.Color.Blue;
            this.label5.Location = new System.Drawing.Point(52, 88);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(276, 15);
            this.label5.TabIndex = 3;
            this.label5.Text = "https://github.com/elDimasX/Nottext-Anti-Rootkit";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.label5.Click += new System.EventHandler(this.label5_Click);
            // 
            // label4
            // 
            this.label4.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label4.BackColor = System.Drawing.Color.Transparent;
            this.label4.Location = new System.Drawing.Point(7, 14);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(794, 177);
            this.label4.TabIndex = 1;
            this.label4.Text = "Nottext Anti-Rootkit \r\n________________________\r\n\r\nVersão do aplicativo: 1.0.0.4\r" +
    "\nDesenvolvido em: C# e C\r\nGitHub: ";
            // 
            // label3
            // 
            this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label3.Location = new System.Drawing.Point(7, 457);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(795, 42);
            this.label3.TabIndex = 0;
            this.label3.Text = "Nottext Anti-Rootkit  © 2021 Nottext Security - Todos os direitos reservados. \r\nD" +
    "esenvolvido por Dimas Pereira";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // columnHeader7
            // 
            this.columnHeader7.DisplayIndex = 0;
            this.columnHeader7.Text = "Arquivo";
            this.columnHeader7.Width = 781;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(7, 96);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(83, 15);
            this.label6.TabIndex = 7;
            this.label6.Text = "Outras opções";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ActiveBorder;
            this.ClientSize = new System.Drawing.Size(822, 558);
            this.Controls.Add(this.panel1);
            this.Font = new System.Drawing.Font("Leelawadee UI", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(48)))), ((int)(((byte)(57)))), ((int)(((byte)(96)))));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(838, 597);
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Nottext Anti-Rootkit";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.menuArquivos.ResumeLayout(false);
            this.menuProcessos.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.tabControl1.ResumeLayout(false);
            this.tabPage5.ResumeLayout(false);
            this.menuDrivers.ResumeLayout(false);
            this.tabPage6.ResumeLayout(false);
            this.menuServicos.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
            this.tabPage3.PerformLayout();
            this.tabPage4.ResumeLayout(false);
            this.tabPage4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.ContextMenuStrip menuArquivos;
        private System.Windows.Forms.ToolStripMenuItem deletarArquivosToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem criarArquivoToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem atualizarToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip menuProcessos;
        private System.Windows.Forms.ToolStripMenuItem listarProcessosStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem finalizarProcessoToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem finalizarBloquearToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem copiarArquivoToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem renomearToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem ocultarProcessoToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem irParaUmaPastaToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem deletarNoBoottoolStripMenuItem1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.ToolStripMenuItem protegerProcessoToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem protegerArquivoToolStripMenuItem;
        private Guna.UI2.WinForms.Guna2ResizeForm arrastar;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TextBox label1;
        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ColumnHeader columnHeader6;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.ListView listView2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.ColumnHeader columnHeader4;
        private System.Windows.Forms.ColumnHeader columnHeader5;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.CheckBox sempreTopo;
        private System.Windows.Forms.Button reiniciar;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ContextMenuStrip menuDrivers;
        private System.Windows.Forms.ToolStripMenuItem atualizarDriverToolStripMenuItem;
        private System.Windows.Forms.TabPage tabPage5;
        private System.Windows.Forms.ListView listView3;
        private System.Windows.Forms.ColumnHeader columnHeader7;
        private System.Windows.Forms.TabPage tabPage6;
        private System.Windows.Forms.ListView listView4;
        private System.Windows.Forms.ColumnHeader columnHeader8;
        private System.Windows.Forms.ColumnHeader columnHeader9;
        private System.Windows.Forms.ContextMenuStrip menuServicos;
        private System.Windows.Forms.ToolStripMenuItem atualizarServicoStripMenuItem1;
        private System.Windows.Forms.ToolStripMenuItem pararToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripMenuItem iniciarToolStripMenuItem;
        private System.Windows.Forms.ColumnHeader columnHeader10;
        private System.Windows.Forms.ColumnHeader columnHeader11;
        private System.Windows.Forms.ToolStripMenuItem deletarArquivoToolStripMenuItem;
        private System.Windows.Forms.ColumnHeader columnHeader12;
        private System.Windows.Forms.ToolStripMenuItem deletarDriverToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.Button desligar;
        private System.Windows.Forms.CheckBox mostrarErros;
        private System.Windows.Forms.CheckBox bloquearProcessos;
        private System.Windows.Forms.Label label6;
    }
}

