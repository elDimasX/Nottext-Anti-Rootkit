

#include <fltKernel.h>
#include <ntstrsafe.h>

// Desativa os alertas de variaveis não iniciadas
#pragma warning (disable: 4703)

// Obtém o nome do Nottext, para que possa proteger depois
PUNICODE_STRING NottextProcesso;

// ZwQuerySystemInformation
NTSTATUS NTAPI ZwQuerySystemInformation(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

PDRIVER_OBJECT ObjetoDriverGlobal = NULL;

// LOG para enviar ao user-mode
#define ARQUIVOS_LOG L"\\??\\C:\\ProgramData\\NtAnti-Rootkit\\folderScan.txt"
#define PROCESSOS_LOG L"\\??\\C:\\ProgramData\\NtAnti-Rootkit\\processesScan.txt"
#define ARQUIVO_DELETAR_BOOT L"\\??\\C:\\ProgramData\\NtAnti-Rootkit\\deleteOnBoot.txt"
#define DRIVERS_LOG L"\\??\\C:\\ProgramData\\NtAnti-Rootkit\\driverScan.txt"
#define SERVICOS_LOG L"\\??\\C:\\ProgramData\\NtAnti-Rootkit\\serviceScan.txt"
#define PROCESSO_MALICIOSO L"\\??\\C:\\ProgramData\\NtAnti-Rootkit\\maliciousProcess.txt"

// Nome do dispositivo
UNICODE_STRING DispositivoNome = RTL_CONSTANT_STRING(L"\\Device\\NtAntiRtDriver"), SysNome = RTL_CONSTANT_STRING(L"\\??\\NtAntiRtDriver");

// Dispositivo global, usado para criar os links simbolicos para a comunicação
PDEVICE_OBJECT DispositivoGlobal;

///
/// CTL_CODES
/// 
#define LISTAR_PASTA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0059, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define LISTAR_PROCESSOS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0064, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define DELETAR_ARQUIVO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0068, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define CRIAR_ARQUIVO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0070, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define TERMINAR_PROCESSO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0072, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define TERMINAR_PROCESSO_E_BLOQUEAR CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0074, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define DELETAR_PASTA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0076, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define SALVAR_ARQUIVO_PARA_COPIAR CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0078, METHOD_BUFFERED, FILE_ANY_ACCESS)
PVOID NomeBackupCopiar;

#define COPIAR_ARQUIVO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0080, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define RENOMEAR_ARQUIVO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0082, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define OCULTAR_PROCESSO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0084, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define DESLIGAR_COMPUTADOR CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0086, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define REINICIAR_COMPUTADOR CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0088, METHOD_BUFFERED, FILE_ANY_ACCESS)

//#define PROTEGER_PROCESSO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0090, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PROTEGER_ARQUIVO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0092, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define LISTAR_DRIVERS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0094, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define DESCARREGAR_SERVICO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0096, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define LISTAR_SERVICOS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0098, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define CARREGAR_SERVICO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0100, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define BLOQUEAR_PROCESSOS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x0102, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define AUTO_DEFESA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x104, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define SUSPENDER_PROCESSO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x106, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define RESUMIR_PROCESSO CTL_CODE(FILE_DEVICE_UNKNOWN, 0x108, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define PROTEGER_NOTTEXT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x110, METHOD_BUFFERED, FILE_ANY_ACCESS)
BOOLEAN NegarProcessos = FALSE;

/// <summary>
/// Inicia o driver
/// </summary>
/// <param name="ObjetoDriver"></param>
/// <param name="RegistroLocal"></param>
/// <returns></returns>
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT ObjetoDriver, _In_ PUNICODE_STRING RegistroLocal);

/// <summary>
/// Verifica se é uma pasta ou arquivo
/// </summary>
/// <param name="dwAttributes"></param>
/// <returns></returns>
BOOLEAN EPasta(unsigned long dwAttributes);

/// <summary>
/// Lista todos os arquivos dentro de uma pasta
/// </summary>
/// <param name="Pasta"></param>
/// <returns></returns>
NTSTATUS ListarPasta(_In_ PUNICODE_STRING Pasta, _In_ BOOLEAN Deletar, _In_ BOOLEAN PrimeiraPasta);

/// <summary>
/// Deleta um arquivo
/// </summary>
/// <param name="Arquivo"></param>
/// <returns></returns>
NTSTATUS DeletarArquivo(_In_ PUNICODE_STRING Arquivo, BOOLEAN ePasta);

/// <summary>
/// Completa um atributo a um arquivo
/// </summary>
/// <param name="DeviceObject"></param>
/// <param name="Irp"></param>
/// <param name="Context"></param>
/// <returns></returns>
NTSTATUS CompletarAtributo(_In_ PDEVICE_OBJECT ObjetoDispositivo, _In_ PIRP Irp, _In_ PVOID Contexto OPTIONAL);

/// <summary>
/// Copia um arquivo
/// </summary>
/// <param name="Arquivo"></param>
/// <returns></returns>
NTSTATUS CopiarArquivo(_In_ PUNICODE_STRING Arquivo, _In_ PUNICODE_STRING ArquivoParaCopiar);

/// <summary>
/// Renomeia um arquivo
/// </summary>
/// <param name="Arquivo"></param>
/// <returns></returns>
NTSTATUS RenomearArquivo(_In_ PUNICODE_STRING Arquivo);

// Tempo
LARGE_INTEGER Tempo;
BOOLEAN EstaAlertando = FALSE;

/// <summary>
/// Dorme
/// </summary>
VOID Dormir(){

	// Espere
	KeDelayExecutionThread(
		KernelMode, // Kernel
		FALSE, // Não alertável
		&Tempo // Tempo, definimos ele em DriverEntry
	);
}

/// <summary>
/// Protege um processo contra finalização, se finalizar, o sistema travará
/// </summary>
/// <param name="ProcessoNome"></param>
NTSTATUS ProtegerProcesso(_In_ DWORD ProcessoNome, _In_ ULONG Proteger);

/// <summary>
/// Configura a operação de suspender ou continuar processo
/// </summary>
/// <returns></returns>
NTSTATUS ConfigurarSuspensao();

/// <summary>
/// Suspende um processo
/// </summary>
/// <param name="Process"></param>
/// <returns></returns>
NTSTATUS SuspenderProcesso(_Out_ PEPROCESS Processo);

/// <summary>
/// Suspende um processo
/// </summary>
/// <param name="Process"></param>
/// <returns></returns>
NTSTATUS ResumirProcesso(_Out_ PEPROCESS Processo);

/// <summary>
/// Retorna um endereço de chamada
/// </summary>
/// <param name="RoutineName"></param>
/// <returns></returns>
/// ReturnSystemRoutineAddress
PVOID RetornarEnderecoDeRotinaDoSistema(_In_ WCHAR* NomeRotina);

/// <summary>
/// Lista todos os processos
/// </summary>
/// <returns></returns>
NTSTATUS ListarProcessos();

/// <summary>
/// Termina um processo
/// </summary>
/// <param name="ProcessoPID"></param>
/// <returns></returns>
NTSTATUS TerminarProcesso(_In_ ULONG ProcessoPID);

/// <summary>
/// Oculta um processo
/// </summary>
/// <param name="pid"></param>
/// <returns></returns>
NTSTATUS OcultarProcesso(_In_ UINT32 pid);
NTSTATUS ProtectProcess(DWORD PID);

/// <summary>
/// Obtém o caminho de um processo
/// </summary>
/// <param name="Process"></param>
/// <returns></returns>
PUNICODE_STRING LocalProcesso(_In_ PEPROCESS Process);

/// <summary>
/// IRP criado
/// </summary>
/// <param name="DeviceObject"></param>
/// <param name="Irp"></param>
/// <returns></returns>
NTSTATUS IRPCriado(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

/// <summary>
/// IRP recebido
/// </summary>
/// <param name="DeviceObject"></param>
/// <param name="Irp"></param>
/// <returns></returns>
NTSTATUS IRPRecebido(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

/// <summary>
/// IRP fechado
/// </summary>
/// <param name="DeviceObject"></param>
/// <param name="Irp"></param>
/// <returns></returns>
NTSTATUS IRPFechado(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

/*

	Estrutura para desligamento

*/
typedef enum _SHUTDOWN_ACTION {
	ShutdownNoReboot,
	ShutdownReboot,
	ShutdownPowerOff
} SHUTDOWN_ACTION;

NTSYSCALLAPI NTSTATUS NTAPI NtShutdownSystem(_In_ SHUTDOWN_ACTION Action);

// Desliga ou reinicia sistema
VOID DesligarSistema(){NtShutdownSystem(2);}
VOID ReiniciarSistema(){NtShutdownSystem(1);}

#pragma warning(disable : 4996)

// Desliga ou reinicia o PC
VOID DesligarOuReiniciarPC(_In_ BOOLEAN DesligarPC)
{
	// Novo work
	PWORK_QUEUE_ITEM ItemTrabalho = (PWORK_QUEUE_ITEM)ExAllocatePool(NonPagedPool, sizeof(WORK_QUEUE_ITEM));

	// Se conseguir alocar
	if (ItemTrabalho != NULL)
	{
		// Se for para desligar
		if (DesligarPC == TRUE)
		{
			// Desligue
			ExInitializeWorkItem(ItemTrabalho, &DesligarSistema, NULL);
		}
		else {
			// Reinicie
			ExInitializeWorkItem(ItemTrabalho, &ReiniciarSistema, NULL);
		}

		// Inicie
		ExQueueWorkItem(ItemTrabalho, DelayedWorkQueue);
	}
}

// Requisição de minifiltro criada
FLT_PREOP_CALLBACK_STATUS IrpMjCriado(
	_Inout_ PFLT_CALLBACK_DATA  Data,
	_In_ PCFLT_RELATED_OBJECTS Objetos,
	_In_ PVOID* Contexto
);

// Função que descarrega o filtro
NTSTATUS DescarregarDriver(
	_In_ FLT_FILTER_UNLOAD_FLAGS Descarregar
);

// Abre um arquivo com IRP
NTSTATUS AbrirArquivoIRP(_In_ UNICODE_STRING Arquivo, _In_ ACCESS_MASK Acesso, _In_ PIO_STATUS_BLOCK Io, _Out_ PFILE_OBJECT* ObjetoArquivo, USHORT CompartilharAcesso);

// Deleta um arquivo por FILE_OBJECT
NTSTATUS DeletarObjetoArquivo(_Out_ PFILE_OBJECT ObjetoArquivo, _In_ BOOLEAN ePasta);

// Lista todos os drivers em execução
VOID ListarDrivers();

// Lista todos os serviços instalados
VOID ListarServicos();

// Verifica se é pra iniciar ou parar um serviço
NTSTATUS CarregarOuDescarregarServico(_In_ PUNICODE_STRING NomeServico, _In_ BOOLEAN Parar);

// Retorna o local do arquivo de um seviço
BOOLEAN LocalArquivo(_In_ PUNICODE_STRING NomeServico, _Out_ PUNICODE_STRING RetornarValor);

// Quando um processo é iniciado
VOID NovoProcesso(_In_ PEPROCESS Processo, _In_ HANDLE Pid, _In_ PPS_CREATE_NOTIFY_INFO Info);

/*
	Operações para bloquear na auto-proteção de processos
*/
#define PROCESS_TERMINATE		(0x0001)		// Processo terminado
#define PROCESS_VM_READ			(0x0010)		// Lê algumas informações
#define PROCESS_VM_WRITE		(0x0020)		// Escreve no processo
#define PROCESS_VM_OPERATION	(0x0008)		// Realiza alguma operação no processo
#define PROCESS_SUSPEND_RESUME	(0x0800)		// Suspende ou resume
#define PROCESS_SET_INFORMATION (0x0200)		// Altera alguma informação do processo
#define PROCESS_SET_PORT		(0x0800)		// Altera a porta do processo
#define PROCESS_SET_SESSIONID	(0x0004)		// Altera o ID do processo
#define PROCESS_CREATE_PROCESS	(0x0080)		// Um processo foi criado

// Quando um processo é morto
OB_PREOP_CALLBACK_STATUS ProcessoMorto(_In_ PVOID Contexto, _In_ POB_PRE_OPERATION_INFORMATION Informacao);

// Registro para a auto-proteção (para que possamos remover ele depois)
PVOID RegistroAutoProtecao = NULL;

// Verifica se a auto-proteção está habilitada ou não
BOOLEAN Autoprotecao = TRUE;

// Instala a auto-proteção
NTSTATUS InstalarAutoProtecao();

// Desinstala a auto-proteção do processo
NTSTATUS DesinstalarAutoProtecao();
