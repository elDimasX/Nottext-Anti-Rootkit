

/*

    Estruturas

*/
typedef struct _SYSTEM_THREADS {
    LARGE_INTEGER  KernelTime;
    LARGE_INTEGER  UserTime;
    LARGE_INTEGER  CreateTime;
    ULONG          WaitTime;
    PVOID          StartAddress;
    CLIENT_ID      ClientId;
    KPRIORITY      Priority;
    KPRIORITY      BasePriority;
    ULONG          ContextSwitchCount;
    LONG           State;
    LONG           WaitReason;
} SYSTEM_THREADS, * PSYSTEM_THREADS;

typedef struct _SYSTEM_PROCESSES {
    ULONG            NextEntryDelta;
    ULONG            ThreadCount;
    ULONG            Reserved1[6];
    LARGE_INTEGER    CreateTime;
    LARGE_INTEGER    UserTime;
    LARGE_INTEGER    KernelTime;
    UNICODE_STRING   ProcessName;
    KPRIORITY        BasePriority;
    SIZE_T           ProcessId;
    SIZE_T           InheritedFromProcessId;
    ULONG            HandleCount;
    ULONG            Reserved2[2];
    VM_COUNTERS      VmCounters;
    IO_COUNTERS      IoCounters;
    SYSTEM_THREADS   Threads[1];
} SYSTEM_PROCESSES, * PSYSTEM_PROCESSES;

// Definicões
#define SystemProcessInformation 5
typedef unsigned char       BYTE;

// TAG
#define POOL_TAG 'enoN'

// Incluição
#include <stdlib.h>

/// <summary>
/// Lista todos os processos
/// </summary>
/// <returns></returns>
NTSTATUS ListarProcessos()
{
    NTSTATUS Status;
    ULONG Tamanho = 0;

    // Se conseguir obter a informação da alocação
    if (ZwQuerySystemInformation(SystemProcessInformation, NULL, 0, &Tamanho) == STATUS_INFO_LENGTH_MISMATCH) {

        // Tamanho
        if (Tamanho) {

            // Memoria
            PVOID Memoria = ExAllocatePoolWithTag(PagedPool, Tamanho, POOL_TAG);

            if (Memoria) {

                // Obtenha as informações novamente
                Status = ZwQuerySystemInformation(SystemProcessInformation, Memoria, Tamanho, &Tamanho);

                if (NT_SUCCESS(Status)) {

                    PSYSTEM_PROCESSES entradaProcesso = Memoria;

                    do {

                        // Nome
                        if (entradaProcesso->ProcessName.Length) {

                            // String
                            PCHAR NomeProcesso = ExAllocatePoolWithTag(PagedPool, 300, 'KIKE');

                            // Se conseguir
                            if (NomeProcesso)
                            {
                                // Converta
                                // 
                                // NomeProcesso = CHAR NomeProcesso[100]
                                //Status = RtlStringCbPrintfA(NomeProcesso, _countof(NomeProcesso), "%ws | %llu\r\n", processEntry->ProcessName.Buffer, processEntry->ProcessId);

                                sprintf(
                                    NomeProcesso,
                                    "%ws : Threads: %u : PID: %llu\r\n",
                                    entradaProcesso->ProcessName.Buffer,
                                    entradaProcesso->ThreadCount,
                                    entradaProcesso->ProcessId
                                );

                                // Escreva no arquivo
                                EscreverNoArquivo(PROCESSOS_LOG, NomeProcesso, FILE_APPEND_DATA);

                                ExFreePoolWithTag(NomeProcesso, 'KIKE');
                            }
                        }
                        
                        // Ache novos processos
                        entradaProcesso = (PSYSTEM_PROCESSES)((BYTE*)entradaProcesso + entradaProcesso->NextEntryDelta);
                    } while (entradaProcesso->NextEntryDelta);
                }

                // Libere
                ExFreePoolWithTag(Memoria, POOL_TAG);
            }
        }
    }

    // Status
    return Status;
}

/// <summary>
/// Protege um processo, quando ele for finalizado, o sistema trava
/// </summary>
/// <param name="ProcessoPID"></param>
VOID ProtegerProcesso(_In_ ULONG ProcessoPID)
{
    // Objetos
    HANDLE AlcaProcesso;
    OBJECT_ATTRIBUTES Atributos;

    // ID
    CLIENT_ID ClienteID;

    // Inicie os atributos
    InitializeObjectAttributes(&Atributos, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

    // Sete o valor
    ClienteID.UniqueProcess = ProcessoPID;
    ClienteID.UniqueThread = 0;

    // Abra o processo
    NTSTATUS Status = ZwOpenProcess(&AlcaProcesso, PROCESS_ALL_ACCESS, &Atributos, &ClienteID);
}

PVOID PsGetProcessSectionBaseAddress(PEPROCESS Process);
NTSTATUS MmUnmapViewOfSection(PEPROCESS Process, PVOID BaseAddress); // Used to unmap process's executable image

/// <summary>
/// Termina um processo
/// </summary>
/// <param name="ProcessoPID"></param>
/// <returns></returns>
NTSTATUS TerminarProcesso(_In_ ULONG ProcessoPID)
{
    PEPROCESS Processo;


    // Status
    NTSTATUS Status = STATUS_SUCCESS;

    // Alça do processo
    HANDLE AlcaProcesso;
    OBJECT_ATTRIBUTES Atributos;

    // ID
    CLIENT_ID ClienteID;

    // Inicie os atributos
    InitializeObjectAttributes(&Atributos, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

    // Sete o valor
    ClienteID.UniqueProcess = ProcessoPID;
    ClienteID.UniqueThread = 0;

    // Abra o processo
    Status = ZwOpenProcess(&AlcaProcesso, PROCESS_ALL_ACCESS, &Atributos, &ClienteID);

    // Se falhar
    if (!NT_SUCCESS(Status))
    {
        // Pegue o processo
        Status = PsLookupProcessByProcessId(ProcessoPID, &Processo);

        if (NT_SUCCESS(Status))
        {
            // Termine o processo usando MmUnmapViewOfSection
            return MmUnmapViewOfSection(Processo, PsGetProcessSectionBaseAddress(Processo));
        }
    }

    // Termine o processo
    Status = ZwTerminateProcess(AlcaProcesso, -1);

    // Feche a alça
    ZwClose(AlcaProcesso);

    // Se falhar
    if (!NT_SUCCESS(Status))
    {
        // Pegue o processo
        Status = PsLookupProcessByProcessId(ProcessoPID, &Processo);

        if (NT_SUCCESS(Status))
        {
            // Termine o processo usando MmUnmapViewOfSection
            Status = MmUnmapViewOfSection(Processo, PsGetProcessSectionBaseAddress(Processo));
        }
    }

    // Retorne
    return Status;
}

// Usado para pegar o nome completo de um processo
NTSTATUS
PsReferenceProcessFilePointer(
    IN  PEPROCESS Process, // O processo
    OUT PVOID* OutFileObject // Arquivo
);

/// <summary>
/// Obtém um local de um processo
/// </summary>
/// <param name="Process"></param>
/// <returns></returns>
PUNICODE_STRING LocalProcesso(_In_ PEPROCESS Processo)
{

    // FileObject
    PFILE_OBJECT FileObject;

    // Onde salvaremos o nome do local do disco
    POBJECT_NAME_INFORMATION FileObjectInfo = NULL;

    // Se não conseguir salvar as informações do processo no PFILE_OBJECT
    if (!NT_SUCCESS(PsReferenceProcessFilePointer(Processo, &FileObject)))
    {
        // Não retorne nada
        return NULL;
    }

    // Se não conseguir obter o local do driver ("C:\") do arquivo
    if (!NT_SUCCESS(IoQueryFileDosDeviceName(FileObject, &FileObjectInfo)))
    {
        // Falhou, pare
        return NULL;
    }

    // Libere o objeto
    ObDereferenceObject(FileObject);

    // Retorne o nome do arquivo
    return &(FileObjectInfo->Name);
}

