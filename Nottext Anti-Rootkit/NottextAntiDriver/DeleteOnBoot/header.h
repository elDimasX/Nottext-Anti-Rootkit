

#include <fltKernel.h>
#include <ntstrsafe.h>


// ZwQuerySystemInformation
NTSTATUS NTAPI ZwQuerySystemInformation(ULONG SystemInformationClass, PVOID SystemInformation, ULONG SystemInformationLength, PULONG ReturnLength);

// LOG de pastas para escanear
#define ARQUIVOS_LOG L"\\??\\C:\\ProgramData\\folderScan.txt"
#define PROCESSOS_LOG L"\\??\\C:\\ProgramData\\processesScan.txt"

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

/// <summary>
/// Inicia o driver
/// </summary>
/// <param name="DriverObject"></param>
/// <param name="RegistryPath"></param>
/// <returns></returns>
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath);

/// <summary>
/// Descarrega o driver
/// </summary>
/// <param name="DriverObject"></param>
VOID Unload(_In_ PDRIVER_OBJECT DriverObject);

/// <summary>
/// Lista todos os arquivos dentro de uma pasta
/// </summary>
/// <param name="Pasta"></param>
/// <returns></returns>
NTSTATUS ListarPasta(_In_ PUNICODE_STRING Pasta, _In_ BOOLEAN Deletar);

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
NTSTATUS CompletarFuncaoIRP(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context);

/// <summary>
/// Completa um atributo a um arquivo
/// </summary>
/// <param name="DeviceObject"></param>
/// <param name="Irp"></param>
/// <param name="Context"></param>
/// <returns></returns>
NTSTATUS CompletarAtributo(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context);

/// <summary>
/// Seta atributos em um arquivo
/// </summary>
/// <param name="FileObject"></param>
/// <param name="IoStatusBlock"></param>
/// <param name="FileInformation"></param>
/// <param name="FileInformationLength"></param>
/// <param name="FileInformationClass"></param>
/// <returns></returns>
NTSTATUS IrpSetarAtributos(PFILE_OBJECT FileObject, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG FileInformationLength, FILE_INFORMATION_CLASS FileInformationClass
);

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

#define BUFFER_SIZE 3000

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
