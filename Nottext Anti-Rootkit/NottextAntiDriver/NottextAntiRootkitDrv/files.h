
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

    IO_STATUS_BLOCK Io;
    PFILE_OBJECT ObjetoArquivo = NULL;

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
        PUNICODE_STRING MensagemBuffer = (PUNICODE_STRING)ExAllocatePoolWithTag(PagedPool, 800, 'bffw');

        if (MensagemBuffer)
        {
            // Copie para o Buffer
            sprintf(MensagemBuffer,
                "%s",
                Mensagem
            );

            // Escreva no arquivo
            Status = ZwWriteFile(
                Alca,
                NULL,
                NULL,
                NULL,

                &ioSbp, // IO

                // Buffer
                MensagemBuffer,
                strlen(MensagemBuffer),
                NULL,
                NULL
            );

            // Terminamos, feche o arquivo
            ZwClose(Alca);

            ExFreePoolWithTag(MensagemBuffer, 'bffw');
        }
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
        }
    }
}
BOOLEAN EPasta(unsigned long dwAttributes)
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

    // IO
    IO_STATUS_BLOCK Io;
    PFILE_OBJECT ObjetoArquivo;

    // Tente abrir a pasta com IRP direto e permissão de exclusão
    NTSTATUS Status = AbrirArquivoIRP(UnicodePasta, FILE_LIST_DIRECTORY | DELETE, &Io, &ObjetoArquivo, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE);

    // Se falhar
    if (!NT_SUCCESS(Status))
        Status = AbrirArquivoIRP(UnicodePasta, FILE_LIST_DIRECTORY, &Io, &ObjetoArquivo, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE);

    // Se conseguir
    if (NT_SUCCESS(Status))
    {
        // Tamanho
        ULONG Tamanho = (2 * 4096 + sizeof(FILE_BOTH_DIR_INFORMATION)) * 0x2000;

        // Pasta
        PFILE_BOTH_DIR_INFORMATION PastaInformacao = ExAllocatePoolWithTag(PagedPool, Tamanho, 'fldr');
        PFILE_BOTH_DIR_INFORMATION pBeginAddr = PastaInformacao;

        // Se conseguir alocar
        if (PastaInformacao)
        {
            // Obtenha os arquivos da pasta
            Status = ListarPastaIRP(ObjetoArquivo, &Io, PastaInformacao, Tamanho, FileBothDirectoryInformation, NULL);
            ULONG Contador = 0;

            // Se conseguir
            if (NT_SUCCESS(Status))
            {
                // Repetição
                while (TRUE)
                {
                    if (!_stricmp(PastaInformacao->FileName, "."))
                        Contador++;

                    // Se o contador for 1, não encontramos nenhum ponto
                    // Se o contador for maior que 1, achamos um pontos, e já o excluimos da lista
                    if (Contador == 0 || Contador > 1)
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
                                    if (EPasta(PastaInformacao->FileAttributes))
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
                                    if (EPasta(PastaInformacao->FileAttributes))
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

                    // Se terminar
                    if (PastaInformacao->NextEntryOffset == 0)
                    {
                        break;
                    }
                    // Vá para o próximo
                    PastaInformacao = (PFILE_BOTH_DIR_INFORMATION)((PUCHAR)PastaInformacao + PastaInformacao->NextEntryOffset);
                }
            }

            ExFreePoolWithTag(pBeginAddr, 'fldr');
        }

        ObDereferenceObject(ObjetoArquivo);
    }

    // Se for para deletar
    if (Deletar == TRUE && PrimeiraPasta == TRUE)
    {
        Status = DeletarArquivo(Pasta, TRUE);
    }

    // Termine
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


