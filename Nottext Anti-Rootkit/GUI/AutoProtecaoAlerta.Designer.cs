
namespace GUI
{
    partial class AutoProtecaoAlerta
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AutoProtecaoAlerta));
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.realizarAcao = new System.Windows.Forms.Button();
            this.terminarProcesso = new System.Windows.Forms.CheckBox();
            this.bloquearProcesso = new System.Windows.Forms.CheckBox();
            this.deletarArquivo = new System.Windows.Forms.CheckBox();
            this.resumirProcesso = new System.Windows.Forms.CheckBox();
            this.label3 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(369, 45);
            this.label1.TabIndex = 0;
            this.label1.Text = "Um processo tentou finalizar o Nottext Anti-Rootkit.\r\nO processo foi suspenso, qu" +
    "eremos que escolha abaixo, o que deseja\r\nfazer com este processo:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 67);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(122, 30);
            this.label2.TabIndex = 1;
            this.label2.Text = "PID:\r\nArquivo do processo: ";
            // 
            // realizarAcao
            // 
            this.realizarAcao.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.realizarAcao.Location = new System.Drawing.Point(15, 228);
            this.realizarAcao.Name = "realizarAcao";
            this.realizarAcao.Size = new System.Drawing.Size(119, 22);
            this.realizarAcao.TabIndex = 5;
            this.realizarAcao.Text = "REALIZAR AÇÃO";
            this.realizarAcao.UseVisualStyleBackColor = true;
            this.realizarAcao.Click += new System.EventHandler(this.realizarAcao_Click);
            // 
            // terminarProcesso
            // 
            this.terminarProcesso.AutoSize = true;
            this.terminarProcesso.Location = new System.Drawing.Point(25, 127);
            this.terminarProcesso.Name = "terminarProcesso";
            this.terminarProcesso.Size = new System.Drawing.Size(124, 19);
            this.terminarProcesso.TabIndex = 6;
            this.terminarProcesso.Text = "Terminar processo";
            this.terminarProcesso.UseVisualStyleBackColor = true;
            this.terminarProcesso.CheckedChanged += new System.EventHandler(this.terminarProcesso_CheckedChanged);
            // 
            // bloquearProcesso
            // 
            this.bloquearProcesso.AutoSize = true;
            this.bloquearProcesso.Location = new System.Drawing.Point(155, 127);
            this.bloquearProcesso.Name = "bloquearProcesso";
            this.bloquearProcesso.Size = new System.Drawing.Size(123, 19);
            this.bloquearProcesso.TabIndex = 7;
            this.bloquearProcesso.Text = "Bloquear processo";
            this.bloquearProcesso.UseVisualStyleBackColor = true;
            this.bloquearProcesso.CheckedChanged += new System.EventHandler(this.bloquearProcesso_CheckedChanged);
            // 
            // deletarArquivo
            // 
            this.deletarArquivo.AutoSize = true;
            this.deletarArquivo.Location = new System.Drawing.Point(284, 127);
            this.deletarArquivo.Name = "deletarArquivo";
            this.deletarArquivo.Size = new System.Drawing.Size(106, 19);
            this.deletarArquivo.TabIndex = 8;
            this.deletarArquivo.Text = "Deletar arquivo";
            this.deletarArquivo.UseVisualStyleBackColor = true;
            // 
            // resumirProcesso
            // 
            this.resumirProcesso.AutoSize = true;
            this.resumirProcesso.Checked = true;
            this.resumirProcesso.CheckState = System.Windows.Forms.CheckState.Checked;
            this.resumirProcesso.Location = new System.Drawing.Point(25, 173);
            this.resumirProcesso.Name = "resumirProcesso";
            this.resumirProcesso.Size = new System.Drawing.Size(119, 19);
            this.resumirProcesso.TabIndex = 9;
            this.resumirProcesso.Text = "Resumir processo";
            this.resumirProcesso.UseVisualStyleBackColor = true;
            this.resumirProcesso.CheckedChanged += new System.EventHandler(this.resumirProcesso_CheckedChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Leelawadee UI", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(140, 227);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(174, 26);
            this.label3.TabIndex = 10;
            this.label3.Text = "Se não selecionar nada\r\no processo continuará suspenso";
            // 
            // AutoProtecaoAlerta
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(419, 262);
            this.ControlBox = false;
            this.Controls.Add(this.label3);
            this.Controls.Add(this.resumirProcesso);
            this.Controls.Add(this.deletarArquivo);
            this.Controls.Add(this.bloquearProcesso);
            this.Controls.Add(this.terminarProcesso);
            this.Controls.Add(this.realizarAcao);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Font = new System.Drawing.Font("Leelawadee UI", 9F);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "AutoProtecaoAlerta";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Alerta";
            this.TopMost = true;
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.AutoProtecaoAlerta_FormClosing);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button realizarAcao;
        public System.Windows.Forms.CheckBox terminarProcesso;
        public System.Windows.Forms.CheckBox bloquearProcesso;
        public System.Windows.Forms.CheckBox deletarArquivo;
        public System.Windows.Forms.CheckBox resumirProcesso;
        private System.Windows.Forms.Label label3;
    }
}