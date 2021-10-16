
/// <summary>
/// Deleta um arquivo por FILE_OBJECT
/// </summary>
/// <param name="ObjetoArquivo"></param>
/// <param name="ePasta"></param>
/// <returns></returns>
NTSTATUS DeletarObjetoArquivo(_Out_ PFILE_OBJECT ObjetoArquivo, _In_ BOOLEAN ePasta)
{
	PDEVICE_OBJECT ObjetoDispositivo, ObjetoBase;
	PIRP Irp;
	KEVENT Evento;
	FILE_DISPOSITION_INFORMATION Informacao;

	IO_STATUS_BLOCK ioStatus;
	PIO_STACK_LOCATION irpSp;

	ObjetoDispositivo = IoGetRelatedDeviceObject(ObjetoArquivo);

	// Obtenha a base
	ObjetoBase = IoGetDeviceAttachmentBaseRef(ObjetoDispositivo);

	// Aloque o IRP
	Irp = IoAllocateIrp(ObjetoBase->StackSize, TRUE);

	// Se falhar
	if (Irp == NULL) {
		return STATUS_UNSUCCESSFUL;
	}

	// Inicie o evento
	KeInitializeEvent(&Evento, SynchronizationEvent, FALSE);

	// Delete
	Informacao.DeleteFile = TRUE;

	// IRP
	Irp->AssociatedIrp.SystemBuffer = &Informacao;
	Irp->UserEvent = &Evento;
	Irp->UserIosb = &ioStatus;
	Irp->Tail.Overlay.OriginalFileObject = ObjetoArquivo;
	Irp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
	Irp->RequestorMode = KernelMode;

	// Configure
	irpSp = IoGetNextIrpStackLocation(Irp);
	irpSp->MajorFunction = IRP_MJ_SET_INFORMATION;
	irpSp->DeviceObject = ObjetoBase;
	irpSp->FileObject = ObjetoArquivo;
	irpSp->Parameters.SetFile.Length = sizeof(FILE_DISPOSITION_INFORMATION);
	irpSp->Parameters.SetFile.FileInformationClass = FileDispositionInformation;
	irpSp->Parameters.SetFile.FileObject = ObjetoArquivo;

	// Complete a rotina
	IoSetCompletionRoutine(
		Irp,
		CompletarAtributo,
		&Evento,
		TRUE,
		TRUE,
		TRUE);

	//PSECTION_OBJECT_POINTERS ObjetoPonteiro;
	//ObjetoPonteiro = ObjetoArquivo->SectionObjectPointer;

	// Se n�o for pasta
	if (ePasta == FALSE)
	{
		// Nenhum processo
		ObjetoArquivo->DeleteAccess = 1;
		ObjetoArquivo->SectionObjectPointer->ImageSectionObject = 0;
		ObjetoArquivo->SectionObjectPointer->DataSectionObject = 0;

		// A MmFlushImageSection libera a se��o da imagem para um arquivo. 
		MmFlushImageSection(
			ObjetoArquivo->SectionObjectPointer,
			MmFlushForDelete //  O arquivo est� sendo exclu�do. 
		);
	}

	// Chame o IRP
	NTSTATUS Status = IoCallDriver(ObjetoBase, Irp);

	// Espere
	KeWaitForSingleObject(&Evento, Executive, KernelMode, TRUE, NULL);

	return Status;
}

/// <summary>
/// Deleta o arquivo, enviando um IRP direto para drivers de baixo n�vel
/// Um n�vel mais simples de remo��o
/// </summary>
NTSTATUS DeletarArquivoNormal(_In_ UNICODE_STRING ArquivoUnicode)
{
    // Atributos
    OBJECT_ATTRIBUTES Atributos;

    // Objeto
    PFILE_OBJECT ObjetoArquivo = NULL;

    // Dispositivo
    PDEVICE_OBJECT ObjetoDispositivo;

    // Alca
    HANDLE Alca = NULL;

    // Status
    NTSTATUS Status;

    // IRP
    PIRP Irp;

    // CurrentLocation
    PIO_STACK_LOCATION CurrentLocation;

    // Evento
    KEVENT Evento;
    IO_STATUS_BLOCK	IoBlock;

    // Valor desconhecido
    ULONG ValorDesconhecido = 1;

    // Inicie os atributos
    InitializeObjectAttributes(
        &Atributos,
        &ArquivoUnicode,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
        NULL,
        NULL
    );

    /*

        A IoCreateFileSpecifyDeviceObjectHint rotina � usada por drivers de filtro do
        sistema de arquivos para enviar uma solicita��o de cria��o apenas para os
        filtros abaixo de um objeto de dispositivo especificado e para o sistema de arquivos.

    */

    Status = IoCreateFileSpecifyDeviceObjectHint(

        // Salve a al�a aqui, para fechar o arquivo depois
        &Alca,

        // Permiss�es que vamos usar ao abrir o arquivo
        GENERIC_READ | GENERIC_WRITE,

        // Os atributos que inicializamos
        &Atributos,
        &IoBlock,
        NULL,

        // Normal
        FILE_ATTRIBUTE_NORMAL,

        // Compartilhe as permiss�es de deletar
        FILE_SHARE_DELETE,

        // N�o � uma pasta
        FILE_OPEN,
        FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
        NULL,
        NULL,

        // N�o sei pra que serve
        CreateFileTypeNone,
        NULL,

        // Ignore os atributos
        IO_IGNORE_SHARE_ACCESS_CHECK | IO_IGNORE_READONLY_ATTRIBUTE,
        NULL
    );

    // Se falhar ao abrir o arquivo
    if (!NT_SUCCESS(Status))
    {
        // Tenta abrir o arquivo com permiss�o de leitura
        Status = ZwOpenFile(
            &Alca,
            GENERIC_READ,
            &Atributos,
            &IoBlock,
            FILE_SHARE_VALID_FLAGS,
            FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE
        );

        // Se falhar
        if (!NT_SUCCESS(Status))
        {
            // Tenta abrir o arquivo com permiss�o de leitura
            Status = IoCreateFileSpecifyDeviceObjectHint(
                &Alca,
                FILE_LIST_DIRECTORY,
                &Atributos,
                &IoBlock,
                NULL,
                FILE_ATTRIBUTE_NORMAL,
                0,
                FILE_OPEN,
                FILE_DIRECTORY_FILE,
                NULL,
                NULL,
                CreateFileTypeNone,
                NULL,
                IO_IGNORE_SHARE_ACCESS_CHECK | IO_IGNORE_READONLY_ATTRIBUTE,
                NULL
            );

            // Se conseguir
            if (NT_SUCCESS(Status))
            {
                //ZwClose(FileHandle);
                ZwDeleteFile(&Atributos);
                goto exit;
            }

            // Saia
            goto exit;
        }
    }

    // Obtenha o objeto do arquivo
    Status = ObReferenceObjectByHandle(

        // Indentificador do arquivo (al�a)
        Alca,

        // Deletar
        DELETE,
        *IoFileObjectType,
        0,

        // Object
        (PVOID*)&ObjetoArquivo,
        0
    );

    // Se falhar
    if (!NT_SUCCESS(Status))
    {
        //DbgPrint("Falha ao obter o objeto do arquivo: %x\n", Status);

        // Saia
        goto exit;
    }

    // Altere o valor para permitir deletar arquivos em uso
    ObjetoArquivo->SectionObjectPointer->ImageSectionObject = 0;

    // Acesso de deletar
    ObjetoArquivo->DeleteAccess = 1;

    // Acesso de escrita
    ObjetoArquivo->WriteAccess = 1;

    // Obtenha o objeto de dispositivo
    ObjetoDispositivo = IoGetBaseFileSystemDeviceObject(ObjetoArquivo);

    // Se falhar
    if (ObjetoDispositivo == NULL)
    {
        // Saia
        goto exit;
    }

    // A IoAllocateIrp rotina n�o associa o IRP a um thread.
    // O driver de aloca��o deve liberar o IRP em vez de complet�-lo de volta
    // para o gerenciador de E / S. 
    Irp = IoAllocateIrp(ObjetoDispositivo->StackSize, 1);

    // Se falhar
    if (Irp == NULL)
    {
        // Saia
        goto exit;
    }

    // Inicia o evento
    KeInitializeEvent(&Evento, SynchronizationEvent, 0);

    // Altere os valores de IRP
    // Ponteiro para um buffer de espa�o do sistema. 
    Irp->AssociatedIrp.SystemBuffer = &ValorDesconhecido;
    Irp->UserEvent = &Evento;
    Irp->UserIosb = &IoBlock;
    Irp->Tail.Overlay.OriginalFileObject = ObjetoArquivo;
    Irp->Tail.Overlay.Thread = KeGetCurrentThread();

    // CurrentLocation
    CurrentLocation = Irp->Tail.Overlay.CurrentStackLocation;

    // Request de KernelMode
    Irp->RequestorMode = KernelMode;
    --CurrentLocation;

    // Alte os valores
    CurrentLocation->MajorFunction = IRP_MJ_SET_INFORMATION;
    CurrentLocation->DeviceObject = ObjetoDispositivo;
    CurrentLocation->FileObject = ObjetoArquivo;
    CurrentLocation->Parameters.SetFile.FileInformationClass = FileDispositionInformation;
    CurrentLocation->Parameters.SetFile.FileObject = ObjetoArquivo;
    CurrentLocation->Parameters.SetFile.Length = sizeof(FILE_DISPOSITION_INFORMATION);

    // Complete a fun��o
    CurrentLocation->CompletionRoutine = CompletarAtributo;
    CurrentLocation->Context = &Evento;
    CurrentLocation->Control = 0xE0;

    // A IoCallDriver rotina envolve IofCallDriver que envia um IRP ao driver
    // associado a um objeto de dispositivo especificado.
    // Para obter mais informa��es, consulte IofCallDriver. 
    Status = IoCallDriver(ObjetoDispositivo, Irp);

    // Se falhar
    if (!NT_SUCCESS(Status))
    {
        // Saia
        goto exit;
    }

    // Espera a opera��o
    KeWaitForSingleObject(&Evento, 0, KernelMode, FALSE, NULL);

    // Status
    //KdPrint(("Irp.status is %x\n", Irp->IoStatus.Status));

    // Saia
    goto exit;

    // Fun��o que sai
exit:

    // Se o FileObject foi usado
    if (ObjetoArquivo != NULL)
    {
        // Libere-o
        ObfDereferenceObject(ObjetoArquivo);
    }

    // Se o arquivo estiver aberto
    if (Alca != NULL)
    {
        // Feche-o
        ZwClose(Alca);
    }

    // Retorne o status
    return Status;
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
            Status = AbrirArquivoIRP(UnArquivo, GENERIC_READ | GENERIC_WRITE | DELETE, &Io, &ObjetoArquivo, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE);

            // Se funcionar
            if (NT_SUCCESS(Status))
            {
                // Tire os atributos
                IrpLimparAtributos(ObjetoArquivo);

                //Status = ObOpenObjectByPointer(ObjetoArquivo, 0, NULL, DELETE, *IoFileObjectType, KernelMode, &Alca);

                Status = DeletarObjetoArquivo(ObjetoArquivo, ePasta);

                // Libere o objeto
                ObDereferenceObject(ObjetoArquivo);
            }
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}

    RtlFreeUnicodeString(&UnArquivo);

    return Status;
}

/// <summary>
/// L� uma linha
/// </summary>
/// <param name="Tamanho"></param>
/// <param name="Linha"></param>
VOID LerLinha(
    SIZE_T Tamanho,
    PCSTR Linha
)
{
    __try {

        CHAR Arquivo[MAX_PATH] = { 0 };

        // Mova o valor
        RtlMoveMemory(Arquivo, Linha, Tamanho);

        // Se falhar ao deletar no modo arquivo
        if (ListarPasta(Arquivo, TRUE, TRUE) != STATUS_SUCCESS)
        {
            // Delete
            DeletarArquivo(Arquivo, FALSE);
        }

        RtlZeroMemory(Arquivo, sizeof(CHAR));

    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

/// <summary>
/// Acha linhas em um texto
/// </summary>
/// <param name="tb"></param>
/// <param name="sz"></param>
/// <returns></returns>
PCSTR ProcurarLinha(SIZE_T tb, PCSTR sz)
{
    // Se for menor de 2
    if (tb < 2)
    {
        return 0;
    }

    tb--;

    do
    {
        if (*sz++ == '\r')
        {
            if (*sz == '\n')
            {
                // Retorne a string - 1
                return sz - 1;
            }
        }
    } while (--tb);

    // Retorne falso
    return 0;
}

/// <summary>
/// Obtem as informa��es corretas para encontrar as linhas
/// </summary>
/// <param name="buf"></param>
/// <param name="tb"></param>
/// <returns></returns>
NTSTATUS ProcessarArquivo(PCSTR buf, SIZE_T tb)
{
    // Junte tudo
    PCSTR Final = buf + tb, linha = buf;

    // While, para encontrar todas as linhas
    while (buf = ProcurarLinha(Final - linha, linha))
    {
        // Leia a linha
        LerLinha(buf - linha, linha);

        // Vamos configurar a linha
        linha = buf + 2;
    }

    // Se a linha for diferente do final
    if (linha != Final)
    {
        // Leia a linha
        LerLinha(
            Final - linha, // Tamanho do buffer
            linha // Linha
        );
    }

    // Terminamos
    return 0;
}

/// <summary>
/// Processa um arquivo, para que seja poss�vel ler o texto dele sem retornar lixo
/// </summary>
/// <param name="Atributos"></param>
/// <returns></returns>
NTSTATUS ProcessarInformacoesArquivo(POBJECT_ATTRIBUTES Atributos)
{
    // Al�a
    HANDLE Alca, Sessao = 0;

    // Status de bloqueio
    IO_STATUS_BLOCK iosb;

    // Abra o arquivo
    NTSTATUS Status = ZwOpenFile(&Alca, FILE_GENERIC_READ, Atributos, &iosb, FILE_SHARE_VALID_FLAGS | FILE_SHARE_DELETE, FILE_SYNCHRONOUS_IO_NONALERT);

    // Se conseguir abrir o arquivo
    if (NT_SUCCESS(Status))
    {
        // Informa��es do arquivo
        FILE_STANDARD_INFORMATION Informacao;

        // Obtenha as informa��es do arquivo
        Status = ZwQueryInformationFile(Alca, &iosb, &Informacao, sizeof(Informacao), FileStandardInformation);

        // Se conseguir
        if (NT_SUCCESS(Status))
        {
            // Se o final do arquvio for muito grande
            if (Informacao.EndOfFile.HighPart)
            {
                Status = STATUS_FILE_TOO_LARGE;
            }

            // Se n�o
            else if (!Informacao.EndOfFile.LowPart)
            {
                Status = STATUS_MAPPED_FILE_SIZE_ZERO;
            }

            // Se tudo ocorrer ok
            else
            {
                // Crie uma sess�o
                Status = ZwCreateSection(
                    &Sessao,

                    // Mapeie para leitura
                    SECTION_MAP_READ,
                    0,
                    0,

                    // Somente leitura
                    PAGE_READONLY,

                    // Commit
                    SEC_COMMIT,
                    Alca
                );
            }
        }

        // Se conseguir
        if (NT_SUCCESS(Status))
        {
            // Informa��es
            PVOID EnderecoBase = 0;
            SIZE_T VerTamanho = 0;

            // Mapeie a sess�o
            Status = ZwMapViewOfSection(
                // Sess�o
                Sessao,

                // Processo atual
                ZwCurrentProcess(),

                // Endere�o
                &EnderecoBase,
                0,
                0,
                0,

                // Tamanho
                &VerTamanho,
                ViewUnmap,
                0,

                // Somente leitura
                PAGE_READONLY
            );

            // Feche a sess�o
            ZwClose(Sessao);

            // Se obter sucesso
            if (NT_SUCCESS(Status))
            {
                // Processe o arquivo novamente, at� falhar ou terminar
                Status = ProcessarInformacoesArquivo((PCSTR)EnderecoBase, Informacao.EndOfFile.LowPart);

                // Desmapeie o arquivo
                ZwUnmapViewOfSection(ZwCurrentProcess(), EnderecoBase);
            }
        }

        // Se a al�a ainda estiver aberta
        if (Alca != NULL)
        {
            ZwClose(Alca);
        }
    }

    // Retorne o status
    return Status;
}

/// <summary>
/// Deleta arquivos durante a inicializa��o do BOOT
/// </summary>
/// <returns></returns>
NTSTATUS DeletarNoBoot()
{
    // V�riaveis
    HANDLE Alca;
    OBJECT_ATTRIBUTES Atributos;
    IO_STATUS_BLOCK IoR;
    IO_STATUS_BLOCK Io;

    UNICODE_STRING UnArquivo;
    RtlInitUnicodeString(&UnArquivo, L"\\??\\C:\\ProgramData\\NtAnti-Rootkit\\deleteOnBoot.txt");

    // Inicie o atributo
    InitializeObjectAttributes(&Atributos, &UnArquivo, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    // Abra o arquivo
    NTSTATUS Status = STATUS_SUCCESS;

    PUCHAR buffer = NULL;
    ULONG Tamanho = 4096;
    LARGE_INTEGER Offset = { 0 };

    // PagedPool = mem�ria do kernel e do driver de dispositivo que PODE transbordar da mem�ria f�sica para o arquivo de p�gina lento 
    // NonPagedPool = Pool n�o paginado � a quantidade de mem�ria do kernel e do driver de dispositivo que deve permanecer na mem�ria f�sica.
    buffer = (PUCHAR)ExAllocatePoolWithTag(PagedPool, Tamanho, 'file');

    // Se conseguir
    if (buffer)
    {
        // Abra o arquivo
        Status = ZwOpenFile(
            &Alca,
            GENERIC_READ,
            &Atributos,
            &IoR,
            FILE_SHARE_READ,
            FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
        );

        if (NT_SUCCESS(Status))
        {
            ProcessarInformacoesArquivo(&Atributos);

            // Repeti��o infinita
            while (1)
            {
                // Zere o tamanho do buffer
                // Tamanho
                Tamanho = 4096;

                // Leia o arquivo original, que vamos copiar
                Status = ZwReadFile(Alca, NULL, NULL, NULL, &Io, buffer, Tamanho, &Offset, NULL);

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
                Offset.QuadPart += Tamanho;

                size_t tb;

                // Vamos converter o BUFFER
                Status = RtlStringCbLengthA(
                    buffer, // Buffer
                    wcslen(buffer) * sizeof(PUCHAR), // Tamanho do buffer
                    &tb // Vamos usar aqui
                );

                if (NT_SUCCESS(Status))
                {
                    // Processe o arquivo
                    ProcessarArquivo(buffer, tb);
                }
            }
        }
    }

    goto sair;

sair:

    if (buffer != NULL)
    {
        ExFreePoolWithTag(buffer, 'file');
    }


    if (Alca != NULL)
    {
        ZwClose(Alca);
        ZwDeleteFile(&Atributos);
    }

    return Status;
}
