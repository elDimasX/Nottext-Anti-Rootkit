
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
NTSTATUS ListarPasta(_In_ PUNICODE_STRING Pasta, _In_ BOOLEAN Deletar)
{
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
                                    "Arquivo : %S\r\n",
                                    NomeChar
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
                                // Procure as outras pastas, e delete o arquivo
                                ListarPasta(NomeCompleto, TRUE);

                                // Delete a pasta
                                //DeletarArquivo(NomeCompleto);
                            }

                            // Se não
                            else {
                                DeletarArquivo(NomeCompleto);
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
    if (Deletar == TRUE)
    {
        Status = DeletarArquivo(Pasta);
    }

    return Status;
}

/// <summary>
/// Completa uma função IRP
/// </summary>
/// <param name="DeviceObject"></param>
/// <param name="Irp"></param>
/// <param name="Context"></param>
/// <returns></returns>
NTSTATUS CompletarFuncaoIRP(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context)
{
    PKEVENT Event = (PKEVENT)Context;

    if (Event)
        KeSetEvent(Event, 0, 0);

    return STATUS_MORE_PROCESSING_REQUIRED;
}

/// <summary>
/// Completa a rotina de remover o somente-leitura
/// Obtive do Nottext File Remove
/// </summary>
NTSTATUS CompletarAtributo(PDEVICE_OBJECT DeviceObject, PIRP Irp, PVOID Context)
{
    // Configure o STATUS
    *Irp->UserIosb = Irp->IoStatus;

    // Se houver algum evento
    if (Irp->UserEvent)
    {
        // Sete o evento
        KeSetEvent(Irp->UserEvent, IO_NO_INCREMENT, 0);
    }

    // Libere o IRP
    IoFreeIrp(Irp);

    // Status
    return STATUS_MORE_PROCESSING_REQUIRED;
}

/// <summary>
/// Seta atributos para remover os 'somente-leitura'
/// Obtive esse do Nottext File Remove
/// </summary>
/// <param name="FileObject"></param>
/// <param name="IoStatusBlock"></param>
/// <param name="FileInformation"></param>
/// <param name="FileInformationLength"></param>
/// <param name="FileInformationClass"></param>
/// <returns></returns>
NTSTATUS IrpSetarAtributos(PFILE_OBJECT FileObject, PIO_STATUS_BLOCK IoStatusBlock, PVOID FileInformation, ULONG FileInformationLength, FILE_INFORMATION_CLASS FileInformationClass
)
{
    // Status
    NTSTATUS Status = STATUS_SUCCESS;

    // Objeto
    PDEVICE_OBJECT DeviceObject;

    // IRP
    PIRP Irp;

    // Evento
    KEVENT SycEvent;

    // IRP
    PIO_STACK_LOCATION irpSp;

    if (
        // Verifique se os valores estao nulos
        FileObject == NULL ||
        IoStatusBlock == NULL ||
        FileInformation == NULL ||
        FileInformationLength <= 0
        )
    {
        // Valores nulo, nao podemos prosseguir
        return STATUS_INVALID_PARAMETER;
    }

    // Pegue o dispositivo
    DeviceObject = IoGetRelatedDeviceObject(FileObject);

    // Se os valores estiverem nulos
    if (DeviceObject == NULL || DeviceObject->StackSize <= 0)
    {
        // Falha
        return STATUS_UNSUCCESSFUL;
    }

    // Aloque o IRP
    Irp = IoAllocateIrp(DeviceObject->StackSize, TRUE);

    // Se nao conseguir
    if (Irp == NULL)
    {
        // Libere o objeto
        //ObDereferenceObject(FileObject);

        // Falha
        return STATUS_UNSUCCESSFUL;
    }

    // Inicie o evento
    KeInitializeEvent(&SycEvent, SynchronizationEvent, FALSE);

    // Altere os valores do IRP
    Irp->AssociatedIrp.SystemBuffer = FileInformation;
    Irp->UserEvent = &SycEvent;
    Irp->UserIosb = IoStatusBlock;
    Irp->Tail.Overlay.OriginalFileObject = FileObject;
    Irp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
    Irp->RequestorMode = KernelMode;

    // Aloque e configure tudo
    irpSp = IoGetNextIrpStackLocation(Irp);
    irpSp->MajorFunction = IRP_MJ_SET_INFORMATION;
    irpSp->DeviceObject = DeviceObject;
    irpSp->FileObject = FileObject;

    // Substitua os atributos se existir
    irpSp->Parameters.SetFile.ReplaceIfExists = TRUE;
    irpSp->Parameters.SetFile.Length = FileInformationLength;
    irpSp->Parameters.SetFile.FileInformationClass = FileInformationClass;
    irpSp->Parameters.SetFile.FileObject = FileObject;

    // Complete a rotina
    IoSetCompletionRoutine(Irp, CompletarAtributo, NULL, TRUE, TRUE, TRUE);

    // Chame a funcao
    Status = IoCallDriver(DeviceObject, Irp);

    // Se estiver pendente
    if (Status == STATUS_PENDING)
    {
        // Espere a operacao
        KeWaitForSingleObject(&SycEvent, Executive, KernelMode, TRUE, NULL);
    }

    // Status
    Status = IoStatusBlock->Status;
    //ObDereferenceObject(pFileObject);

    // Retorne o status
    return Status;

}

/// <summary>
/// Deleta um arquivo
/// </summary>
/// <param name="Arquivo"></param>
/// <returns></returns>
NTSTATUS DeletarArquivo(_In_ PUNICODE_STRING Arquivo)
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

    // Abra o arquivo
    NTSTATUS Status = ZwOpenFile(&Alca, GENERIC_READ, &Atributos, &Io, FILE_SHARE_DELETE, FILE_NON_DIRECTORY_FILE);

    // Se for sucesso
    if (!NT_SUCCESS(Status))
    {
        // Tenta abrir o arquivo com permissão de leitura
        Status = IoCreateFileSpecifyDeviceObjectHint(
            &Alca,
            FILE_LIST_DIRECTORY,
            &Atributos,
            &Io,
            NULL,
            FILE_ATTRIBUTE_NORMAL,
            FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
            FILE_OPEN,
            FILE_DIRECTORY_FILE,
            NULL,
            NULL,
            CreateFileTypeNone,
            NULL,
            IO_IGNORE_SHARE_ACCESS_CHECK | IO_IGNORE_READONLY_ATTRIBUTE,
            NULL
        );

        if (NT_SUCCESS(Status))
        {
            // Feche a alça
            ZwClose(Alca);
            Status = ZwDeleteFile(&Atributos);
        }

        // Libere o UNICODE
        RtlFreeUnicodeString(&UnArquivo);
        return Status;
    }

    // ObReferenceObjectByHandle
    PFILE_OBJECT ObjetoArquivo;
    PDEVICE_OBJECT DispositivoObjeto;
    Status = ObReferenceObjectByHandle(Alca, 0x10000, 0, 0, (PVOID*)&ObjetoArquivo, 0);

    // Se falhar
    if (!NT_SUCCESS(Status))
    {
        ZwClose(Alca);
        RtlFreeUnicodeString(&UnArquivo);
        return Status;
    }

    // Valor
    DispositivoObjeto = IoGetBaseFileSystemDeviceObject(ObjetoArquivo);

    if (DispositivoObjeto == NULL)
    {
        ZwClose(Alca);
        ObfDereferenceObject(ObjetoArquivo);
        RtlFreeUnicodeString(&UnArquivo);
        return;
    }

    // Deletar
    ObjetoArquivo->DeleteAccess = 1;
    ObjetoArquivo->SectionObjectPointer->ImageSectionObject = 0;

    // Informações para um arquivo
    FILE_BASIC_INFORMATION FileInformationAttribute;

    // Copie as informações para os atributos
    memset(&FileInformationAttribute, 0, sizeof(FILE_BASIC_INFORMATION));

    // Atributo de arquivo normal
    FileInformationAttribute.FileAttributes = FILE_ATTRIBUTE_NORMAL;

    // Remova os atributos, como o somente leitura
    Status = IrpSetarAtributos(
        ObjetoArquivo,
        &Io,
        &FileInformationAttribute,
        sizeof(FILE_BASIC_INFORMATION),
        FileBasicInformation
    );

    // IRP
    PIRP Irp;
    Irp = IoAllocateIrp(DispositivoObjeto->StackSize, 1);

    // Se não alocar
    if (Irp == NULL)
    {
        ZwClose(Alca);
        ObfDereferenceObject(ObjetoArquivo);
        RtlFreeUnicodeString(&UnArquivo);
        return;
    }

    // Evento
    KEVENT Evento;
    PIO_STACK_LOCATION LocalAtual;
    KeInitializeEvent(&Evento, SynchronizationEvent, 0);

    ULONG ValorDesconhecido = 1;

    Irp->AssociatedIrp.SystemBuffer = &ValorDesconhecido;
    Irp->UserEvent = &Evento;
    Irp->UserIosb = &Io;
    Irp->Tail.Overlay.OriginalFileObject = ObjetoArquivo;
    Irp->Tail.Overlay.Thread = KeGetCurrentThread();

    LocalAtual = Irp->Tail.Overlay.CurrentStackLocation;
    Irp->RequestorMode = KernelMode;
    --LocalAtual;
    LocalAtual->MajorFunction = 6;
    LocalAtual->DeviceObject = DispositivoObjeto;
    LocalAtual->FileObject = ObjetoArquivo;
    LocalAtual->Parameters.SetFile.FileInformationClass = 13;
    LocalAtual->Parameters.SetFile.FileObject = ObjetoArquivo;
    LocalAtual->Parameters.SetFile.Length = 1;

    // Completa a rotia
    LocalAtual->CompletionRoutine = CompletarFuncaoIRP;
    LocalAtual->Context = &Evento;
    LocalAtual->Control = 0xE0;

    // IoCallDriver
    Status = IoCallDriver(DispositivoObjeto, Irp);

    // Se falhar
    if (!NT_SUCCESS(Status))
    {
        ZwClose(Alca);
        ObfDereferenceObject(ObjetoArquivo);
        RtlFreeUnicodeString(&UnArquivo);
        return Status;
    }

    // Espere
    KeWaitForSingleObject(&Evento, 0, KernelMode, FALSE, NULL);

    ZwClose(Alca);
    ObfDereferenceObject(ObjetoArquivo);
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

