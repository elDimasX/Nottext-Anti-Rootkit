
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
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.listView1 = new System.Windows.Forms.ListView();
            this.columnHeader1 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader2 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader6 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.menuArquivos = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.deleteFileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.criarArquivoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.copiarArquivoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.renomearToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.atualizarToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.irParaUmaPastaToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.label1 = new System.Windows.Forms.Label();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.listView2 = new System.Windows.Forms.ListView();
            this.columnHeader3 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader4 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.columnHeader5 = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.menuProcessos = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.finalizarProcessoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.finalizarBloquearToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ocultarProcessoToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.listarProcessosStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.reiniciar = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.desligar = new System.Windows.Forms.Button();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.linkLabel1 = new System.Windows.Forms.LinkLabel();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.tabControl1.SuspendLayout();
            this.tabPage1.SuspendLayout();
            this.menuArquivos.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.menuProcessos.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Location = new System.Drawing.Point(0, 0);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(817, 470);
            this.tabControl1.TabIndex = 0;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.listView1);
            this.tabPage1.Controls.Add(this.label1);
            this.tabPage1.Location = new System.Drawing.Point(4, 23);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(809, 443);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Arquivos";
            this.tabPage1.UseVisualStyleBackColor = true;
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
            this.listView1.FullRowSelect = true;
            this.listView1.GridLines = true;
            this.listView1.HideSelection = false;
            this.listView1.Location = new System.Drawing.Point(0, 0);
            this.listView1.Margin = new System.Windows.Forms.Padding(2);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(813, 418);
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
            // menuArquivos
            // 
            this.menuArquivos.Font = new System.Drawing.Font("Consolas", 9F);
            this.menuArquivos.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.deleteFileToolStripMenuItem,
            this.criarArquivoToolStripMenuItem,
            this.copiarArquivoToolStripMenuItem,
            this.renomearToolStripMenuItem,
            this.toolStripSeparator1,
            this.atualizarToolStripMenuItem,
            this.irParaUmaPastaToolStripMenuItem});
            this.menuArquivos.Name = "menuArquivos";
            this.menuArquivos.Size = new System.Drawing.Size(194, 142);
            // 
            // deleteFileToolStripMenuItem
            // 
            this.deleteFileToolStripMenuItem.Name = "deleteFileToolStripMenuItem";
            this.deleteFileToolStripMenuItem.Size = new System.Drawing.Size(193, 22);
            this.deleteFileToolStripMenuItem.Text = "Deletar";
            this.deleteFileToolStripMenuItem.Click += new System.EventHandler(this.deleteFileToolStripMenuItem_Click);
            // 
            // criarArquivoToolStripMenuItem
            // 
            this.criarArquivoToolStripMenuItem.Name = "criarArquivoToolStripMenuItem";
            this.criarArquivoToolStripMenuItem.Size = new System.Drawing.Size(193, 22);
            this.criarArquivoToolStripMenuItem.Text = "Criar arquivo";
            this.criarArquivoToolStripMenuItem.Click += new System.EventHandler(this.criarArquivoToolStripMenuItem_Click);
            // 
            // copiarArquivoToolStripMenuItem
            // 
            this.copiarArquivoToolStripMenuItem.Name = "copiarArquivoToolStripMenuItem";
            this.copiarArquivoToolStripMenuItem.Size = new System.Drawing.Size(193, 22);
            this.copiarArquivoToolStripMenuItem.Text = "Copiar arquivo";
            this.copiarArquivoToolStripMenuItem.Click += new System.EventHandler(this.copiarArquivoToolStripMenuItem_Click);
            // 
            // renomearToolStripMenuItem
            // 
            this.renomearToolStripMenuItem.Name = "renomearToolStripMenuItem";
            this.renomearToolStripMenuItem.Size = new System.Drawing.Size(193, 22);
            this.renomearToolStripMenuItem.Text = "Renomear";
            this.renomearToolStripMenuItem.Click += new System.EventHandler(this.renomearToolStripMenuItem_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(190, 6);
            // 
            // atualizarToolStripMenuItem
            // 
            this.atualizarToolStripMenuItem.Name = "atualizarToolStripMenuItem";
            this.atualizarToolStripMenuItem.Size = new System.Drawing.Size(193, 22);
            this.atualizarToolStripMenuItem.Text = "Atualizar";
            this.atualizarToolStripMenuItem.Click += new System.EventHandler(this.atualizarToolStripMenuItem_Click);
            // 
            // irParaUmaPastaToolStripMenuItem
            // 
            this.irParaUmaPastaToolStripMenuItem.Name = "irParaUmaPastaToolStripMenuItem";
            this.irParaUmaPastaToolStripMenuItem.Size = new System.Drawing.Size(193, 22);
            this.irParaUmaPastaToolStripMenuItem.Text = "Ir para uma pasta";
            this.irParaUmaPastaToolStripMenuItem.Click += new System.EventHandler(this.irParaUmaPastaToolStripMenuItem_Click);
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(9, 424);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(21, 14);
            this.label1.TabIndex = 1;
            this.label1.Text = "C:";
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.listView2);
            this.tabPage2.Location = new System.Drawing.Point(4, 23);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(809, 443);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Processos";
            this.tabPage2.UseVisualStyleBackColor = true;
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
            this.listView2.FullRowSelect = true;
            this.listView2.GridLines = true;
            this.listView2.HideSelection = false;
            this.listView2.Location = new System.Drawing.Point(0, 0);
            this.listView2.Margin = new System.Windows.Forms.Padding(2);
            this.listView2.Name = "listView2";
            this.listView2.Size = new System.Drawing.Size(813, 421);
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
            // menuProcessos
            // 
            this.menuProcessos.Font = new System.Drawing.Font("Consolas", 9F);
            this.menuProcessos.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.finalizarProcessoToolStripMenuItem,
            this.finalizarBloquearToolStripMenuItem,
            this.ocultarProcessoToolStripMenuItem,
            this.toolStripSeparator2,
            this.listarProcessosStripMenuItem});
            this.menuProcessos.Name = "menuArquivos";
            this.menuProcessos.Size = new System.Drawing.Size(215, 98);
            // 
            // finalizarProcessoToolStripMenuItem
            // 
            this.finalizarProcessoToolStripMenuItem.Name = "finalizarProcessoToolStripMenuItem";
            this.finalizarProcessoToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
            this.finalizarProcessoToolStripMenuItem.Text = "Finalizar processo";
            this.finalizarProcessoToolStripMenuItem.Click += new System.EventHandler(this.finalizarProcessoToolStripMenuItem_Click);
            // 
            // finalizarBloquearToolStripMenuItem
            // 
            this.finalizarBloquearToolStripMenuItem.Name = "finalizarBloquearToolStripMenuItem";
            this.finalizarBloquearToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
            this.finalizarBloquearToolStripMenuItem.Text = "Finalizar && Bloquear";
            this.finalizarBloquearToolStripMenuItem.Click += new System.EventHandler(this.finalizarBloquearToolStripMenuItem_Click);
            // 
            // ocultarProcessoToolStripMenuItem
            // 
            this.ocultarProcessoToolStripMenuItem.Name = "ocultarProcessoToolStripMenuItem";
            this.ocultarProcessoToolStripMenuItem.Size = new System.Drawing.Size(214, 22);
            this.ocultarProcessoToolStripMenuItem.Text = "Ocultar processo";
            this.ocultarProcessoToolStripMenuItem.Click += new System.EventHandler(this.ocultarProcessoToolStripMenuItem_Click);
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(211, 6);
            // 
            // listarProcessosStripMenuItem
            // 
            this.listarProcessosStripMenuItem.Name = "listarProcessosStripMenuItem";
            this.listarProcessosStripMenuItem.Size = new System.Drawing.Size(214, 22);
            this.listarProcessosStripMenuItem.Text = "Atualizar";
            this.listarProcessosStripMenuItem.Click += new System.EventHandler(this.listarProcessosStripMenuItem_Click);
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.reiniciar);
            this.tabPage3.Controls.Add(this.label2);
            this.tabPage3.Controls.Add(this.desligar);
            this.tabPage3.Location = new System.Drawing.Point(4, 23);
            this.tabPage3.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.tabPage3.Size = new System.Drawing.Size(809, 443);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "Configurações";
            this.tabPage3.UseVisualStyleBackColor = true;
            // 
            // reiniciar
            // 
            this.reiniciar.Location = new System.Drawing.Point(111, 41);
            this.reiniciar.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.reiniciar.Name = "reiniciar";
            this.reiniciar.Size = new System.Drawing.Size(81, 21);
            this.reiniciar.TabIndex = 2;
            this.reiniciar.Text = "REINICIAR";
            this.reiniciar.UseVisualStyleBackColor = true;
            this.reiniciar.Click += new System.EventHandler(this.reiniciar_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(7, 18);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(168, 14);
            this.label2.TabIndex = 1;
            this.label2.Text = "Opções de desligamento:";
            // 
            // desligar
            // 
            this.desligar.Location = new System.Drawing.Point(18, 41);
            this.desligar.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.desligar.Name = "desligar";
            this.desligar.Size = new System.Drawing.Size(81, 21);
            this.desligar.TabIndex = 0;
            this.desligar.Text = "DESLIGAR";
            this.desligar.UseVisualStyleBackColor = true;
            this.desligar.Click += new System.EventHandler(this.desligar_Click);
            // 
            // tabPage4
            // 
            this.tabPage4.Controls.Add(this.linkLabel1);
            this.tabPage4.Controls.Add(this.label4);
            this.tabPage4.Controls.Add(this.label3);
            this.tabPage4.Location = new System.Drawing.Point(4, 23);
            this.tabPage4.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.tabPage4.Size = new System.Drawing.Size(809, 443);
            this.tabPage4.TabIndex = 3;
            this.tabPage4.Text = "Sobre";
            this.tabPage4.UseVisualStyleBackColor = true;
            // 
            // linkLabel1
            // 
            this.linkLabel1.AutoSize = true;
            this.linkLabel1.Location = new System.Drawing.Point(60, 83);
            this.linkLabel1.Name = "linkLabel1";
            this.linkLabel1.Size = new System.Drawing.Size(343, 14);
            this.linkLabel1.TabIndex = 2;
            this.linkLabel1.TabStop = true;
            this.linkLabel1.Text = "https://github.com/elDimasX/Nottext-Anti-Rootkit";
            this.linkLabel1.VisitedLinkColor = System.Drawing.Color.Blue;
            this.linkLabel1.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabel1_LinkClicked);
            // 
            // label4
            // 
            this.label4.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label4.Location = new System.Drawing.Point(7, 13);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(793, 166);
            this.label4.TabIndex = 1;
            this.label4.Text = "Nottext Anti-Rootkit \r\n________________________\r\n\r\nVersão do aplicativo: 1.0.0.1\r" +
    "\nDesenvolvido em: C# e C\r\nGitHub: ";
            // 
            // label3
            // 
            this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label3.Location = new System.Drawing.Point(7, 407);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(796, 39);
            this.label3.TabIndex = 0;
            this.label3.Text = "Nottext Anti-Rootkit  © 2021 Nottext Security - Todos os direitos reservados. \r\nD" +
    "esenvolvido por Dimas Pereira";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 14F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(816, 470);
            this.Controls.Add(this.tabControl1);
            this.Font = new System.Drawing.Font("Consolas", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Nottext Anti-Rootkit";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.tabControl1.ResumeLayout(false);
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.menuArquivos.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.menuProcessos.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
            this.tabPage3.PerformLayout();
            this.tabPage4.ResumeLayout(false);
            this.tabPage4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.ContextMenuStrip menuArquivos;
        private System.Windows.Forms.ToolStripMenuItem deleteFileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem criarArquivoToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem atualizarToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip menuProcessos;
        private System.Windows.Forms.ToolStripMenuItem listarProcessosStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem finalizarProcessoToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem finalizarBloquearToolStripMenuItem;
        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.ListView listView2;
        private System.Windows.Forms.ColumnHeader columnHeader3;
        private System.Windows.Forms.ColumnHeader columnHeader4;
        private System.Windows.Forms.ColumnHeader columnHeader5;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ToolStripMenuItem copiarArquivoToolStripMenuItem;
        private System.Windows.Forms.ColumnHeader columnHeader6;
        private System.Windows.Forms.ToolStripMenuItem renomearToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem ocultarProcessoToolStripMenuItem;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button desligar;
        private System.Windows.Forms.Button reiniciar;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.LinkLabel linkLabel1;
        private System.Windows.Forms.ToolStripMenuItem irParaUmaPastaToolStripMenuItem;
    }
}

