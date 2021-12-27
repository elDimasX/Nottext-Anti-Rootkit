

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
/// Protege um processo contra finalização
/// </summary>
/// <param name="ProcessoNome"></param>
VOID ProtegerProcesso(_In_ PUNICODE_STRING ProcessoNome)
{
    // Adicione para ser protegido
}

/// <summary>
/// Estruturas para suspender ou resumir processos
/// Código obtido aqui: https://www.unknowncheats.me/forum/c-and-c-/363566-suspend-process-ring0.html
/// </summary>
typedef NTSTATUS(NTAPI* pPsSuspendProcess)(_Out_ PEPROCESS Processo);
typedef NTSTATUS(NTAPI* pPsResumeProcess)(_Out_ PEPROCESS Processo);

/// <summary>
/// Declaração
/// </summary>
pPsSuspendProcess fSuspenderProcesso;
pPsResumeProcess fResumirProcesso;

/// <summary>
/// Retorna um endereço de chamada
/// </summary>
/// <param name="RoutineName"></param>
/// <returns></returns>
/// ReturnSystemRoutineAddress
PVOID RetornarEnderecoDeRotinaDoSistema(_In_ WCHAR* NomeRotina)
{
    // Rotina
    UNICODE_STRING Rotina = { 0 };

    // Se for null
    if (!NomeRotina)
    {
        // Retorne 0
        return 0;
    }

    // Inicie o Unicode
    RtlInitUnicodeString(&Rotina, NomeRotina);

    return MmGetSystemRoutineAddress(&Rotina);
}

/// <summary>
/// Configura a operação de suspender ou continuar processo
/// </summary>
/// <returns></returns>
NTSTATUS ConfigurarSuspensao()
{
    // Queremos configurar a suspenção do processo
    fSuspenderProcesso = (pPsSuspendProcess)RetornarEnderecoDeRotinaDoSistema(L"PsSuspendProcess");
    fResumirProcesso = (pPsResumeProcess)RetornarEnderecoDeRotinaDoSistema(L"PsResumeProcess");

    // Se falhar em obter as opereções de resumo e suspensão
    if (!fSuspenderProcesso || !fResumirProcesso)
    {
        // Falha
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // Sucesso
    return STATUS_SUCCESS;
}

/// <summary>
/// Suspende um processo
/// </summary>
/// <param name="Process"></param>
/// <returns></returns>
NTSTATUS SuspenderProcesso(_Out_ PEPROCESS Processo)
{
    // Se for null, significa que não conseguimos obter o essa função do sistema
    if (!fSuspenderProcesso)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // Retorne
    return fSuspenderProcesso(Processo);
}

/// <summary>
/// Suspende um processo
/// </summary>
/// <param name="Process"></param>
/// <returns></returns>
NTSTATUS ResumirProcesso(_Out_ PEPROCESS Processo)
{
    // Se for null, significa que não conseguimos obter o essa função do sistema
    if (!fResumirProcesso)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // Retorne
    return fResumirProcesso(Processo);
}

/// <summary>
/// Quando um processo estiver sendo finalizado, ele passa pra essa função
/// Com isso, podemos aplicar a auto-defesa
/// </summary>
/// <param name="Contexto"></param>
/// <param name="Informacao"></param>
/// <returns></returns>
OB_PREOP_CALLBACK_STATUS ProcessoMorto(_In_ PVOID Contexto, _In_ POB_PRE_OPERATION_INFORMATION Informacao)
{
    // Verifique se a auto-proteção está habilitada
    if (Autoprotecao == FALSE)
    {
        // Desabilita, pare
        return OB_PREOP_SUCCESS;
    }

    // O acesso que devemos retornar
    PACCESS_MASK AcessoRetornar = NULL;

    // Acesso do processo original
    ACCESS_MASK AcessoOriginal = 0;

    // PID do processo
    HANDLE Pid;

    // Verifique se o ObjectType foi PsProcessType
    if (Informacao->ObjectType == *PsProcessType)
    {
        // Se for o processo que esteja solicitando isso
        if (Informacao->Object == PsGetCurrentProcess())
        {
            // Permita que o processo se modifique
            return OB_PREOP_SUCCESS;
        }
    }

    // Se for o thread
    else if (Informacao->ObjectType == *PsThreadType)
    {
        // Obtenha o Processo pelo PID
        Pid = PsGetThreadProcessId(
            // PID
            (PETHREAD)Informacao->Object
        );

        // Se o thread for do nosso processo
        if (Informacao == PsGetCurrentProcessId())
        {
            // Permita
            return OB_PREOP_SUCCESS;
        }
    }

    // Se for uma operação feita pelo kernel
    if (Informacao->KernelHandle == 1)
    {
        // Permita
        return OB_PREOP_SUCCESS;
    }

    // Aloque espaço
    PUNICODE_STRING NomeProcesso = ExAllocatePoolWithTag(PagedPool, 600, 'tag');

    // Se não conseguir alocar
    if (NomeProcesso == NULL)
    {
        return OB_PREOP_SUCCESS;
    }

    // Converta UNICODE para PUNICODE para podermos comparar
    sprintf(NomeProcesso, "%wZ", (UNICODE_STRING*)LocalProcesso((PEPROCESS)Informacao->Object));

    // Verifique se o processo é protegido
    if (strstr(NomeProcesso, NottextProcesso))
    {
        // Acesso que devemos retornar, receba isso para alterar depois
        AcessoRetornar = &Informacao->Parameters->CreateHandleInformation.DesiredAccess;

        // Obtenha o acesso original
        AcessoOriginal = Informacao->Parameters->CreateHandleInformation.OriginalDesiredAccess;

        // Se for diferente de PROCESS_CREATE_PROCESS
        // Significa que um processo não está sendo aberto
        if ((AcessoOriginal & PROCESS_CREATE_PROCESS) != PROCESS_CREATE_PROCESS)
        {
            // Faça outras verificações
            if (
                // Processo está sendo terminado
                (AcessoOriginal & PROCESS_TERMINATE) == PROCESS_TERMINATE ||

                // Processo está sendo suspenso
                (AcessoOriginal & PROCESS_SUSPEND_RESUME) == PROCESS_SUSPEND_RESUME ||

                // Está alterando alguma informação do processo
                (AcessoOriginal & PROCESS_SET_INFORMATION) == PROCESS_SET_INFORMATION
                )
            {
                // Suspenda o processo
                SuspenderProcesso(PsGetCurrentProcess());

                // Aloque espaço
                PUNICODE_STRING NomeProcessoMalicioso = ExAllocatePoolWithTag(PagedPool, 600, 'tag');

                // Se conseguir alocar
                if (NomeProcessoMalicioso != NULL)
                {
                    // Converta UNICODE para PUNICODE para avisarmos ao user-mode
                    sprintf(NomeProcessoMalicioso, "PID: %u Arquivo: %wZ", PsGetCurrentProcessId(), (UNICODE_STRING*)LocalProcesso(PsGetCurrentProcess()));

                    // Escreva no arquivo
                    EscreverNoArquivo(PROCESSO_MALICIOSO, NomeProcessoMalicioso, GENERIC_WRITE);
                }

                // Retorne STATUS_ABANDONED (ACCESS_DENIED não funciona), que vai fazer com que a operação
                // Seja cancelada 
                *AcessoRetornar &= STATUS_ABANDONED;
            }
        }
    }

    // Libere
    ExFreePoolWithTag(NomeProcesso, 'tag');

    // Permita
    return OB_PREOP_SUCCESS;
}

/// <summary>
/// Instala a auto-proteção (impede que o processo seja encerrado)
/// </summary>
/// <returns></returns>
NTSTATUS InstalarAutoProtecao()
{
    // Definições
    OB_CALLBACK_REGISTRATION Registro;
    OB_OPERATION_REGISTRATION Operacao;

    // Tipo de operação
    Operacao.ObjectType = PsProcessType;
    Operacao.Operations = OB_OPERATION_HANDLE_CREATE; // Tudo que for registrado, é passado pro nosso driver

    // Configure a operação antes e depois
    Operacao.PreOperation = ProcessoMorto;
    Operacao.PostOperation = NULL;

    // Inicie a altitude da nossa chamada
    RtlInitUnicodeString(&Registro.Altitude, L"370001");

    // Versão
    Registro.Version = OB_FLT_REGISTRATION_VERSION;
    Registro.OperationRegistrationCount = 1;

    // Sem contexto
    Registro.RegistrationContext = NULL;

    // Registre o "Operacao", que configuramos acima
    Registro.OperationRegistration = &Operacao;

    // Tente registrar ele
    NTSTATUS Status = ObRegisterCallbacks(
        &Registro,
        &RegistroAutoProtecao
    );

    // Retorne o status
    return Status;
}

/// <summary>
/// Desinstala a auto-proteção
/// </summary>
/// <returns></returns>
NTSTATUS DesinstalarAutoProtecao()
{
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    // Se o RegistroAutoProtecao não for nulo, significa que a proteção foi instalada
    if (RegistroAutoProtecao != NULL)
    {
        // Remova ele
        ObUnRegisterCallbacks(RegistroAutoProtecao);
        
        // Se o Status for nulo depois de removermos ele, significa que houve sucesso
        if (RegistroAutoProtecao == NULL)
            Status = STATUS_SUCCESS;
    }

    return Status;
}

/// <summary>
/// Definições para finalizar um processo (um método mais agressivo)
/// </summary>
/// <param name="Process"></param>
/// <returns></returns>
PVOID PsGetProcessSectionBaseAddress(PEPROCESS Process);
NTSTATUS MmUnmapViewOfSection(PEPROCESS Process, PVOID BaseAddress); // Usado para desmapeia a imagem do processo

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

    // Se falhar ao terminar processo, mas sucesso ao abrir
    if (!NT_SUCCESS(Status))
    {
        // Pegue o processo a partir do PID
        Status = PsLookupProcessByProcessId(ProcessoPID, &Processo);

        // Se conseguir
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

/// <summary>
/// Quando um processo é iniciado
/// </summary>
/// <param name="Processo"></param>
/// <param name="Pid"></param>
/// <param name="Info"></param>
VOID NovoProcesso(_In_ PEPROCESS Processo, _In_ HANDLE Pid, _In_ PPS_CREATE_NOTIFY_INFO Info)
{
    if (Info)
    {
        if (NegarProcessos == TRUE)
            Info->CreationStatus = STATUS_ACCESS_DENIED;
        else
            Info->CreationStatus = STATUS_SUCCESS;
    }
}
