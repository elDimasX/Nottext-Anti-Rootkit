

#include <fltKernel.h>
#include <ntstrsafe.h>
#include "rootkitOperations.h"

// Desativa os alertas de variaveis não iniciadas
#pragma warning (disable: 4703)

// ZwQuerySystemInformation
NTSTATUS NTAPI ZwQuerySystemInformation(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

// LOG de pastas para escanear
#define ARQUIVOS_LOG L"\\??\\C:\\ProgramData\\folderScan.txt"
#define PROCESSOS_LOG L"\\??\\C:\\ProgramData\\processesScan.txt"
PVOID ARQUIVO_LIDO = "\\??\\C:\\ProgramData\\fileReaded.txt";

// Nome do dispositivo
UNICODE_STRING DispositivoNome = RTL_CONSTANT_STRING(L"\\Device\\NtAntiRtDriver"), SysNome = RTL_CONSTANT_STRING(L"\\??\\NtAntiRtDriver");

// Dispositivo global, usado para criar os links simbolicos para a comunicação
PDEVICE_OBJECT DispositivoGlobal;

///
/// CTL_CODES
/// 
// CTL para deletar lsitar uma pasta
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

/// <summary>
/// Inicia o driver
/// </summary>
/// <param name="DriverObject"></param>
/// <param name="RegistryPath"></param>
/// <returns></returns>
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath);

/// <summary>
/// Verifica se é uma pasta ou arquivo
/// </summary>
/// <param name="dwAttributes"></param>
/// <returns></returns>
BOOLEAN IsFileDirectory(unsigned long dwAttributes);

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
NTSTATUS DeletarArquivo(_In_ PUNICODE_STRING Arquivo);

/// <summary>
/// Completa uma função IRP
/// </summary>
/// <param name="DeviceObject"></param>
/// <param name="Irp"></param>
/// <param name="Context"></param>
/// <returns></returns>
NTSTATUS CompletarFuncaoIRP(_In_ PDEVICE_OBJECT ObjetoDispositivo, _In_ PIRP Irp, _In_ PVOID Context);

/// <summary>
/// Completa um atributo a um arquivo
/// </summary>
/// <param name="DeviceObject"></param>
/// <param name="Irp"></param>
/// <param name="Context"></param>
/// <returns></returns>
NTSTATUS CompletarAtributo(_In_ PDEVICE_OBJECT ObjetoDispositivo, _In_ PIRP Irp, _In_ PVOID Contexto);

/// <summary>
/// Cria um arquivo
/// </summary>
/// <param name="Arquivo"></param>
NTSTATUS CriarArquivo(_In_ PUNICODE_STRING Arquivo);

/// <summary>
/// Escreve um texto em algum  arquivo
/// </summary>
/// <param name="FileToLog"></param>
/// <param name="Message"></param>
/// <param name="Mask"></param>
/// <returns></returns>
NTSTATUS EscreverNoArquivo(_In_ PUNICODE_STRING ArquivoParaEscrever, _In_ PUNICODE_STRING Mensagem, _In_ ACCESS_MASK Mask);

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

#define BUFFER_SIZE 500

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
/// Lista todos os processos
/// </summary>
/// <returns></returns>
NTSTATUS ListarProcessos();

/// <summary>
/// Termina um processo
/// </summary>
/// <param name="ProcessID"></param>
/// <returns></returns>
NTSTATUS TerminarProcesso(_In_ ULONG ProcessID);

/// <summary>
/// Oculta um processo
/// </summary>
/// <param name="pid"></param>
/// <returns></returns>
VOID OcultarProcesso(_In_ UINT32 pid);

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

// Desliga o sistema
VOID DesligarSistema(){NtShutdownSystem(2);}
VOID ReiniciarSistema(){NtShutdownSystem(1);}

#pragma warning(disable : 4996)

/// <summary>
/// Força o PC a desligar
/// </summary>
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
			// Inicie
			ExInitializeWorkItem(ItemTrabalho, &DesligarSistema, NULL);
		}
		else {
			// Inicie
			ExInitializeWorkItem(ItemTrabalho, &ReiniciarSistema, NULL);
		}

		// Inicie
		ExQueueWorkItem(ItemTrabalho, DelayedWorkQueue);
	}
}

/// <summary>
/// Requisição criada
/// </summary>
/// <param name="Data"></param>
/// <param name="Objects"></param>
/// <param name="Context"></param>
/// <returns></returns>
FLT_PREOP_CALLBACK_STATUS IrpMjCriado(
	_Inout_ PFLT_CALLBACK_DATA  Data,
	_In_ PCFLT_RELATED_OBJECTS Objetos,
	_In_ PVOID* Contexto
);

/// <summary>
/// Função que descarrega o filtro
/// </summary>
NTSTATUS DescarregarDriver(
	_In_ FLT_FILTER_UNLOAD_FLAGS Descarregar
);
