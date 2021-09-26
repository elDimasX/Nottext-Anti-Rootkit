
#include "filesIRP.h"

/// <summary>
/// Escreve em um arquivo
/// </summary>
/// <param name="ArquivoParaEscrever"></param>
/// <param name="Mensagem"></param>
/// <param name="Mask"></param>
/// <returns></returns>
NTSTATUS EscreverNoArquivo(_In_ PUNICODE_STRING ArquivoParaEscrever, _In_ PUNICODE_STRING Mensagem, _In_ ACCESS_MASK Mask)
{
    // Se tiver com IRQL alto, pare
    if (KeGetCurrentIrql() != PASSIVE_LEVEL)
    {
        return STATUS_UNSUCCESSFUL;
    }

    ULONG i;

    // Repetição até 10
    for (i = 0; i < 10; i++)
    {
        Dormir();

        // Se não estiver alertando outro evento
        if (EstaAlertando == FALSE)
        {
            // Pare
            break;
        }
    }

    // Ok, estamos alertando um evento
    EstaAlertando = TRUE;

    // Nome do arquivo
    UNICODE_STRING ArquivoUnicode;

    // Atributos
    OBJECT_ATTRIBUTES Atributos;

    // Inicie o UNICODE
    RtlInitUnicodeString(&ArquivoUnicode, ArquivoParaEscrever);

    // Inicie os atributos
    InitializeObjectAttributes(
        &Atributos,
        &ArquivoUnicode,
        OBJ_KERNEL_HANDLE |
        OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
    );

    // Alça do arquivo
    NTSTATUS Status;
    HANDLE Alca;

    // IO
    IO_STATUS_BLOCK ioSbp;

    // Abra o arquivo, ou crie se não existir
    Status = ZwCreateFile(
        &Alca,
        Mask | SYNCHRONIZE,
        &Atributos,
        &ioSbp,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        0,
        FILE_OPEN_IF,
        FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE,
        NULL,
        0
    );

    // Se conseguir
    if (NT_SUCCESS(Status))
    {
        // Buffer
        CHAR MensagemBuffer[BUFFER_SIZE];
        size_t tb;

        // RtlStringCbPrintfW e RtlStringCbPrintfA é para garantir
        // que eles não gravem além do final do buffer.
        Status = RtlStringCbPrintfA(
            MensagemBuffer,
            sizeof(MensagemBuffer),
            Mensagem,
            0x0
        );

        // Se conseguir
        if (NT_SUCCESS(Status))
        {
            // Os RtlStringCbLengthW e RtlStringCbLengthA funções
            // determinar o comprimento, em bytes, de uma cadeia fornecida. 
            Status = RtlStringCbLengthA(
                MensagemBuffer, // Buffer
                sizeof(MensagemBuffer), // Tamanho dela
                &tb // TB
            );

            // Se obter sucesso
            if (NT_SUCCESS(Status))
            {
                // Escreva no arquivo
                Status = ZwWriteFile(
                    Alca,
                    NULL,
                    NULL,
                    NULL,

                    &ioSbp, // IO

                    // Buffer
                    MensagemBuffer,
                    tb,
                    NULL,
                    NULL
                );
            }
        }

        // Terminamos, feche o arquivo
        Status = ZwClose(
            Alca
        );
    }

    // Terminamos de alertar, deixe outra operação pendente terminar
    EstaAlertando = FALSE;

    return Status;
}

/// <summary>
/// Copia um arquivo
/// </summary>
/// <param name="Arquivo"></param>
/// <returns></returns>
NTSTATUS CopiarArquivo(_In_ PUNICODE_STRING Arquivo, _In_ PUNICODE_STRING ArquivoParaCopiar)
{
    // Tudo para os 2 arquivos
    HANDLE Alca = NULL;
    HANDLE AlcaCopiar = NULL;

    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    IO_STATUS_BLOCK IoW;
    IO_STATUS_BLOCK IoR;

    IO_STATUS_BLOCK Io;

    OBJECT_ATTRIBUTES Atributos;
    OBJECT_ATTRIBUTES AtributosCopiar;

    ANSI_STRING AsCopiar;
    ANSI_STRING As;

    UNICODE_STRING Un;
    UNICODE_STRING UnCopiar;

    // Converta para UNICODE o arquivo para copiar
    RtlInitAnsiString(&AsCopiar, ArquivoParaCopiar);
    RtlAnsiStringToUnicodeString(&UnCopiar, &AsCopiar, TRUE);

    if (UnCopiar.Buffer == NULL)
    {
        goto sair;
    }

    // Inicie os atributos do arquivo para copiar
    InitializeObjectAttributes(&AtributosCopiar, &UnCopiar, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    // Converta para UNICODE o arquivo
    RtlInitAnsiString(&As, Arquivo);
    RtlAnsiStringToUnicodeString(&Un, &As, TRUE);

    if (Un.Buffer == NULL)
    {
        goto sair;
    }

    // Inicie os atributos do arquivo para copiar
    InitializeObjectAttributes(&Atributos, &Un, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    // Outras variaveis
    PVOID buffer = NULL;
    ULONG Tamanho = 4096;
    LARGE_INTEGER Offset = { 0 };

    // Aloque espaço
    buffer = ExAllocatePoolWithTag(PagedPool, Tamanho, 'file');

    // Se conseguir alocar
    if (buffer)
    {
        // Tente abrir o arquivo que vai ser copiado
        Status = ZwOpenFile(&AlcaCopiar, GENERIC_READ, &AtributosCopiar, &IoR, FILE_SHARE_READ, FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);

        // Se conseguir abrir o arquivo que vamos ler
        if (NT_SUCCESS(Status))
        {
            // Abra o arquivo que vamos escrever
            Status = ZwCreateFile(&Alca, GENERIC_READ | GENERIC_WRITE, &Atributos, &IoW, NULL, FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE, NULL, 0);

            if (NT_SUCCESS(Status))
            {
                while (1)
                {
                    // Tamanho
                    Tamanho = 4096;

                    // Leia o arquivo original, que vamos copiar
                    Status = ZwReadFile(AlcaCopiar, NULL, NULL, NULL, &Io, buffer, Tamanho, &Offset, NULL);

                    // Se falhar
                    if (!NT_SUCCESS(Status))
                    {
                        // Se for o fim do arquivo
                        if (Status == STATUS_END_OF_FILE)
                        {
                            Status = STATUS_SUCCESS;
                            break;
                        }
                        else
                        {
                            // Saia
                            break;
                        }
                    }

                    // Tamanhho
                    Tamanho = Io.Information;

                    // Escreva no arquivo que vai ser criado
                    Status = ZwWriteFile(Alca, NULL, NULL, NULL, &Io, buffer, Tamanho, &Offset, NULL);

                    // Se falhar
                    if (!NT_SUCCESS(Status))
                    {
                        // Saia
                        break;
                    }

                    Offset.QuadPart += Tamanho;
                }
            }
        }
    }

    // Se o buffer não for NULL
    if (buffer != NULL)
        ExFreePoolWithTag(buffer, 'file');

    goto sair;
sair:

    // Se for NULL
    if (Un.Buffer != NULL)
        RtlFreeUnicodeString(&Un);

    // Se for NULL
    if (UnCopiar.Buffer != NULL)
        RtlFreeUnicodeString(&UnCopiar);

    if (Alca != NULL)
        ZwClose(Alca);

    if (AlcaCopiar != NULL)
        ZwClose(AlcaCopiar);

    return Status;
}

/// <summary>
/// Renomear um arquivo
/// </summary>
/// <param name="Arquivo"></param>
/// <returns></returns>
NTSTATUS RenomearArquivo(_In_ PUNICODE_STRING Arquivo)
{
    // AS e UNICODE
    ANSI_STRING As;
    UNICODE_STRING Un;

    // ANSI
    RtlInitAnsiString(&As, NomeBackupCopiar);
    RtlAnsiStringToUnicodeString(&Un, &As, TRUE);

    // Se não alocar
    if (Un.Buffer == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }

    // AS e UNICODE para o novo nome
    ANSI_STRING AsNovo;
    UNICODE_STRING UnNovo;

    // ANSI
    RtlInitAnsiString(&AsNovo, Arquivo);
    RtlAnsiStringToUnicodeString(&UnNovo, &AsNovo, TRUE);

    // Se não alocar
    if (UnNovo.Buffer == NULL)
    {
        goto sair;
    }

    // Atributos
    OBJECT_ATTRIBUTES Atributos;

    // Informação para renomear
    PFILE_RENAME_INFORMATION Renomear = NULL;

    // Inicie os atributos
    InitializeObjectAttributes(&Atributos, &Un, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    // Alça
    HANDLE Alca = NULL;
    IO_STATUS_BLOCK Io;
    NTSTATUS Status;

    // Abra o arquivo como pasta
    Status = ZwOpenFile(
        &Alca,
        DELETE | SYNCHRONIZE | FILE_LIST_DIRECTORY,
        &Atributos,
        &Io,
        FILE_SHARE_READ,
        FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
    );

    if (!NT_SUCCESS(Status))
    {
        // Abra o arquivo como arquivo
        Status = ZwOpenFile(
            &Alca,
            DELETE | SYNCHRONIZE,
            &Atributos,
            &Io,
            FILE_SHARE_READ,
            FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
        );
    }

    if (NT_SUCCESS(Status))
    {
        // Aloque espaço
        Renomear = ExAllocatePoolWithTag(PagedPool, sizeof(FILE_RENAME_INFORMATION) + UnNovo.MaximumLength, 'file');

        // Se não conseguir alocar
        if (!Renomear)
        {
            goto sair;
        }

        // Zerar o buffer
        RtlZeroMemory(Renomear, sizeof(FILE_RENAME_INFORMATION) + UnNovo.MaximumLength);
        Renomear->FileNameLength = UnNovo.Length;

        // Copie
        wcscpy(Renomear->FileName, UnNovo.Buffer);
        Renomear->ReplaceIfExists = FALSE;
        Renomear->RootDirectory = NULL;

        // Renomeie
        Status = ZwSetInformationFile(Alca, &Io, Renomear, sizeof(FILE_RENAME_INFORMATION) + UnNovo.MaximumLength, FileRenameInformation);
    }

    goto sair;

sair:

    if (Un.Buffer != NULL)
        RtlFreeUnicodeString(&Un);

    if (UnNovo.Buffer != NULL)
        RtlFreeUnicodeString(&UnNovo);

    if (Alca != NULL)
        ZwClose(Alca);

    if (Renomear != NULL)
        ExFreePoolWithTag(Renomear, 'file');

    return Status;
}

// Definição
#define CleanupRoutine(JEWS,DID,WTC) ZwClose(JEWS);
sMemFree(DID, WTC);

/// <summary>
/// Link onde peguei o código: http://www.rohitab.com/discuss/topic/33415-tell-files-and-directories-apart-in-zwquerydirectoryfile/
/// </summary>
/// <param name="size"></param>
/// <param name="tag"></param>
/// <returns></returns>
void* MemAlloc(size_t size, const unsigned long tag)
{
    void* MemBlock = NULL;
    if (KeGetCurrentIrql() <= APC_LEVEL && tag != 0)
    {
        MemBlock = ExAllocatePoolWithTag(PagedPool, size, tag);
        if (MemBlock != NULL)
            RtlZeroMemory(MemBlock, size);
    }
    return MemBlock;
}
void MemFree(void* Buffer, const unsigned long tag)
{
    if (KeGetCurrentIrql() <= APC_LEVEL && tag != 0 && Buffer != NULL)
    {
        try
        {
            ExFreePoolWithTag(Buffer, tag);
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            DbgPrint("Invalid Memory\r\n");
        }
    }
}
BOOLEAN IsFileDirectory(unsigned long dwAttributes)
{
    return (BOOLEAN)((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
}

/// <summary>
/// Lista todos os arquivos em um diretorio
/// </summary>
/// <param name="szFileDirectoryName"></param>
/// <returns></returns>
NTSTATUS ListarPasta(_In_ PUNICODE_STRING Pasta, _In_ BOOLEAN Deletar, _In_ BOOLEAN PrimeiraPasta)
{
    PEPROCESS eproc = IoGetCurrentProcess();
    KeAttachProcess(eproc);

    // Resultado do scaneamento
    PUCHAR Buffer = NULL;

    // Tamanho do Blob a ser preenchido durante a varredura
    ULONG TamanhoBuffer = 0;

    // Rara saber se estamos reiniciando a varredura de diretório
    BOOLEAN Primeiro = TRUE;

    // Bloco de status para indicar o status final de uma solicitação de I / O
    IO_STATUS_BLOCK IoStatusBlock = { 0 };

    // ANSI e UNICODE
    ANSI_STRING AnsiPasta;
    UNICODE_STRING UnicodePasta;

    // Pasta
    RtlInitAnsiString(&AnsiPasta, Pasta);
    RtlAnsiStringToUnicodeString(&UnicodePasta, &AnsiPasta, TRUE);

    // Se não preencher
    if (UnicodePasta.Buffer == NULL)
    {
        // Falha
        return STATUS_UNSUCCESSFUL;
    }

    // Estrutura esperada por ZwOpenFile
    OBJECT_ATTRIBUTES Atributos;

    // Manipula para o diretório a ser verificado com ZwQueryDirectoryFile
    HANDLE Alca;

    // Status
    NTSTATUS Status;

    // rResultado estruturado do dir scan
    FILE_BOTH_DIR_INFORMATION* PastaInformacao;

    // Inicie os atributos
    InitializeObjectAttributes(
        &Atributos,
        &UnicodePasta,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,

        // aberto absoluto, nenhum identificador de diretório relativo
        NULL,

        // nenhum descritor de segurança necessário
        NULL
    );

    // Abra a pasta
    Status = ZwOpenFile(
        &Alca,
        FILE_LIST_DIRECTORY,
        &Atributos,
        &IoStatusBlock,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT
    );

    // Se não conseguir
    if (!NT_SUCCESS(Status))
    {
        RtlFreeUnicodeString(&UnicodePasta);
        return Status;
    }

    // Tamanho do buffer
    TamanhoBuffer = 64 * 1024;

    // Aloque
    Buffer = (PUCHAR)MemAlloc(TamanhoBuffer, 'KICK');
    if (Buffer == NULL)
    {
        RtlFreeUnicodeString(&UnicodePasta);
        return Status;
    }

    // Iniciamos uma nova varredura
    Primeiro = TRUE;

    while (1)
    {
        // Obtenha as informações
        Status = ZwQueryDirectoryFile(
            Alca,
            NULL,
            NULL,
            NULL,
            &IoStatusBlock,
            Buffer,
            TamanhoBuffer,
            FileBothDirectoryInformation,
            FALSE,
            NULL,
            Primeiro
        );

        // Se não tiver mais arquivos
        if (Status == STATUS_NO_MORE_FILES || Status == STATUS_NO_SUCH_FILE)
        {
            break;
        }

        // Se falhar
        if (!NT_SUCCESS(Status))
        {
            // Saia
            break;
        }

        // Vamos continuar o escaneamento
        Primeiro = FALSE;

        // Informação
        PastaInformacao = (FILE_BOTH_DIR_INFORMATION*)Buffer;

        // Itera por meio de dados coletados e nomes de exibição
        while (1)
        {
            // Se for uma pasta, e não for um ponto
            if ((PastaInformacao->FileName)[0] != L'.')
            {
                // Nome CHAR
                UCHAR* NomeChar = MemAlloc(PastaInformacao->FileNameLength * sizeof(WCHAR), 'KIKE');

                // Se conseguir
                if (NomeChar)
                {
                    // Copie
                    RtlCopyMemory(NomeChar, &PastaInformacao->FileName[0], PastaInformacao->FileNameLength);

                    // Nome do arquivo
                    PUNICODE_STRING NomeCompleto = MemAlloc(300, 'KIKE');

                    // Se foi alocado
                    if (NomeCompleto)
                    {
                        // Se não for para deletar
                        if (Deletar == FALSE)
                        {
                            // Se for um arquivo
                            if (IsFileDirectory(PastaInformacao->FileAttributes))
                            {
                                sprintf(
                                    NomeCompleto,
                                    "Pasta : %S\r\n",
                                    NomeChar
                                );
                            }
                            else {
                                sprintf(
                                    NomeCompleto,
                                    "Arquivo : %S : Tamanho: %d\r\n",
                                    NomeChar,
                                    PastaInformacao->EndOfFile.QuadPart
                                );
                            }

                            EscreverNoArquivo(ARQUIVOS_LOG, NomeCompleto, FILE_APPEND_DATA);
                        }

                        // Se for para deletar
                        else {

                            // Converta o nome
                            sprintf(
                                NomeCompleto,
                                "%s\\%S",
                                Pasta,
                                NomeChar
                            );

                            // Se for uma pasta
                            if (IsFileDirectory(PastaInformacao->FileAttributes))
                            {
                                // Procure as outras pastas, e delete os arquivos
                                // Mas não é a pasta principal
                                ListarPasta(NomeCompleto, TRUE, FALSE);

                                // Delete a pasta
                                DeletarArquivo(NomeCompleto, TRUE);
                            }

                            // Se não
                            else {
                                DeletarArquivo(NomeCompleto, FALSE);
                            }
                        }

                        MemFree(NomeCompleto, 'KIKE');
                    }

                    MemFree(NomeChar, 'KIKE');
                }
            }

            // Se for 0
            if (PastaInformacao->NextEntryOffset == 0)
            {
                break;
            }
            else
            {
                // Altere
                PastaInformacao = (FILE_BOTH_DIR_INFORMATION*)(((PUCHAR)PastaInformacao) + PastaInformacao->NextEntryOffset);
            }
        }
    }

    // Se a alça estiver aberta
    if (Alca != NULL)
    {
        ZwClose(Alca);
    }

    // Se não for NULL
    if (UnicodePasta.Buffer != NULL)
    {
        RtlFreeUnicodeString(&UnicodePasta);
    }

    // Se for para deletar
    if (Deletar == TRUE && PrimeiraPasta == TRUE)
    {
        Status = DeletarArquivo(Pasta, TRUE);
    }

    KeDetachProcess();

    return Status;
}

/// <summary>
/// Completa a rotina de remover o somente-leitura
/// </summary>
NTSTATUS CompletarAtributo(_In_ PDEVICE_OBJECT ObjetoDispositivo, _In_ PIRP Irp, _In_ PVOID Contexto OPTIONAL)
{
    // Configure o STATUS
    *Irp->UserIosb = Irp->IoStatus;

    // Se tiver o evento
    if (Irp->UserEvent)
        KeSetEvent(Irp->UserEvent, IO_NO_INCREMENT, 0);
    
    // Se tiver um buffer
    if (Irp->MdlAddress)
    {
        // Libere-o
        IoFreeMdl(Irp->MdlAddress);
        Irp->MdlAddress = NULL;
    }

    // Libere o IRP
    IoFreeIrp(Irp);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

/// <summary>
/// Limpa os atributos
/// </summary>
/// <param name="FileHandle"></param>
/// <returns></returns>
BOOLEAN IrpSetarAtributos(PFILE_OBJECT ObjetoArquivo)
{
    // Arquivo
    NTSTATUS Status = STATUS_SUCCESS;

    // Device
    PDEVICE_OBJECT DispositivoObjetivo, BaseDispositivo;
    PIRP Irp;

    // Evento
    KEVENT evento;
    FILE_BASIC_INFORMATION Informacao;
    IO_STATUS_BLOCK ioStatus;
    PIO_STACK_LOCATION irpSp;

    // Obtenha o dsispotivo
    DispositivoObjetivo = IoGetRelatedDeviceObject(ObjetoArquivo);

    BaseDispositivo = IoGetDeviceAttachmentBaseRef(DispositivoObjetivo);

    Irp = IoAllocateIrp(BaseDispositivo->StackSize, TRUE);

    if (Irp == NULL)
    {
        return FALSE;
    }

    // Inicie o evento
    KeInitializeEvent(&evento, SynchronizationEvent, FALSE);

    // Copie as informações
    memset(&Informacao, 0, sizeof(FILE_BASIC_INFORMATION));

    // Arquivo normal
    Informacao.FileAttributes = FILE_ATTRIBUTE_NORMAL;
    Irp->AssociatedIrp.SystemBuffer = &Informacao;
    Irp->UserEvent = &evento;
    Irp->UserIosb = &ioStatus;
    Irp->Tail.Overlay.OriginalFileObject = ObjetoArquivo;
    Irp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
    Irp->RequestorMode = KernelMode;

    // Configure o Irp
    irpSp = IoGetNextIrpStackLocation(Irp);
    irpSp->MajorFunction = IRP_MJ_SET_INFORMATION;
    irpSp->DeviceObject = BaseDispositivo;
    irpSp->FileObject = ObjetoArquivo;
    irpSp->Parameters.SetFile.ReplaceIfExists = TRUE;
    irpSp->Parameters.SetFile.Length = sizeof(FILE_BASIC_INFORMATION);
    irpSp->Parameters.SetFile.FileInformationClass = FileBasicInformation;
    irpSp->Parameters.SetFile.FileObject = ObjetoArquivo;

    // Complete a rotina
    IoSetCompletionRoutine(
        Irp,
        CompletarAtributo,
        &evento,
        TRUE,
        TRUE,
        TRUE);

    // Chame o IRP
    IoCallDriver(BaseDispositivo, Irp);

    // Espere
    KeWaitForSingleObject(&evento, Executive, KernelMode, TRUE, NULL);

    return TRUE;
}

/// <summary>
/// Deleta um arquivo
/// </summary>
/// <param name="Arquivo"></param>
/// <returns></returns>
NTSTATUS DeletarArquivo(_In_ PUNICODE_STRING Arquivo, BOOLEAN ePasta)
{
    // ANSI e STRING
    UNICODE_STRING UnArquivo;
    ANSI_STRING AsArquivo;

    // Inicie
    RtlInitAnsiString(&AsArquivo, Arquivo);
    RtlAnsiStringToUnicodeString(&UnArquivo, &AsArquivo, TRUE);

    if (UnArquivo.Buffer == NULL)
    {
        return STATUS_UNSUCCESSFUL;
    }

    // Atributos
    OBJECT_ATTRIBUTES Atributos;

    // Inicie os atributos
    InitializeObjectAttributes(&Atributos, &UnArquivo, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

    HANDLE Alca;
    IO_STATUS_BLOCK Io;
    NTSTATUS Status = STATUS_UNSUCCESSFUL;

    __try {

        // Primeiro, tente deletar de forma simples
        Status = DeletarArquivoNormal(UnArquivo);

        // Se falhar
        if (!NT_SUCCESS(Status))
        {
            // Objeto
            PFILE_OBJECT ObjetoArquivo = NULL;

            // Abra o arquivo em IRP
            Status = AbrirArquivoIRP(UnArquivo, GENERIC_READ | DELETE, &Io, &ObjetoArquivo);

            // Se funcionar
            if (NT_SUCCESS(Status))
            {
                DbgPrint("Aberto");

                // Tire os atributos
                IrpSetarAtributos(ObjetoArquivo);

                //Status = ObOpenObjectByPointer(ObjetoArquivo, 0, NULL, DELETE, *IoFileObjectType, KernelMode, &Alca);

                Status = DeletarObjetoArquivo(ObjetoArquivo, ePasta);

                // Libere o objeto
                ObDereferenceObject(ObjetoArquivo);
            }
        }
    } __except (EXCEPTION_EXECUTE_HANDLER){ }

    RtlFreeUnicodeString(&UnArquivo);

    return Status;
}

/// <summary>
/// Cria um arquivo
/// </summary>
/// <param name="Arquivo"></param>
/// <returns></returns>
NTSTATUS CriarArquivo(_In_ PUNICODE_STRING Arquivo)
{
    ANSI_STRING PastaAnsi;
    UNICODE_STRING PastaUnicode;
    OBJECT_ATTRIBUTES Atributos;
    HANDLE Alca;
    IO_STATUS_BLOCK IoStatusBlock;

    // Inicie o ANSI e o UNICODE
    RtlInitAnsiString(&PastaAnsi, Arquivo);
    RtlAnsiStringToUnicodeString(&PastaUnicode, &PastaAnsi, TRUE);

    InitializeObjectAttributes(&Atributos, &PastaUnicode, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    // Cria o arquivo
    NTSTATUS Status = ZwCreateFile(
        &Alca,
        GENERIC_WRITE | GENERIC_READ,
        &Atributos,
        &IoStatusBlock,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        FILE_SHARE_READ,
        FILE_CREATE,
        FILE_NON_DIRECTORY_FILE,
        NULL,
        0
    );

    if (NT_SUCCESS(Status))
    {
        ZwClose(Alca);
    }

    if (PastaUnicode.Buffer != NULL)
    {
        RtlFreeUnicodeString(&PastaUnicode);
    }

    return Status;
}

