
/// <summary>
/// IRP criado
/// </summary>
/// <param name="DeviceObject"></param>
/// <param name="Irp"></param>
/// <returns></returns>
NTSTATUS IRPCriado(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	// Informações e status
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	// Não incrimente mais nenhum valor, apenas, concluia o IRP
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Sucesso
	return STATUS_SUCCESS;
}

/// <summary>
/// IRP recebido
/// </summary>
/// <param name="DeviceObject"></param>
/// <param name="Irp"></param>
/// <returns></returns>
NTSTATUS IRPRecebido(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	PIO_STACK_LOCATION Io = IoGetCurrentIrpStackLocation(Irp);

	// Mensagem para retornar ao user-mode
	CHAR* StatusRetornar = "fail!";

	// Mensagem do user-mode
	PCHAR MensagemUsuario = (PCHAR)Irp->AssociatedIrp.SystemBuffer;

	// Informção pra retornar ao user-mode, necessário aumentar o valor caso
	// Queira enviar uma mensagem com mais de 10 caracterias
	Irp->IoStatus.Information = 10;

	// Verificar uma pasta
	if (Io->Parameters.DeviceIoControl.IoControlCode == LISTAR_PASTA)
	{

		// Se conseguir
		if (NT_SUCCESS(ListarPasta(MensagemUsuario, FALSE)))
		{
			// Se conseguir
			StatusRetornar = "success!";
		}
	}
	else if (Io->Parameters.DeviceIoControl.IoControlCode == DELETAR_ARQUIVO)
	{
		if (NT_SUCCESS(DeletarArquivo(MensagemUsuario)))
		{
			StatusRetornar = "success!";
		}
	}
	else if (Io->Parameters.DeviceIoControl.IoControlCode == CRIAR_ARQUIVO)
	{
		if (NT_SUCCESS(CriarArquivo(MensagemUsuario)))
		{
			StatusRetornar = "success!";
		}
	}
	else if (Io->Parameters.DeviceIoControl.IoControlCode == LISTAR_PROCESSOS)
	{
		if (NT_SUCCESS(ListarProcessos()))
		{
			StatusRetornar = "success!";
		}
	}
	else if (Io->Parameters.DeviceIoControl.IoControlCode == TERMINAR_PROCESSO)
	{
		// ANSI_STRING e UNICODE
		ANSI_STRING As;
		UNICODE_STRING Un;

		// Inicie os valores
		RtlInitAnsiString(&As, MensagemUsuario);
		RtlAnsiStringToUnicodeString(&Un, &As, TRUE);
		
		// Inicie o PID
		ULONG Pid;
		RtlUnicodeStringToInteger(&Un, 0, &Pid);

		// Termine o processo
		if (NT_SUCCESS(TerminarProcesso(Pid)))
		{
			StatusRetornar = "success!";
		}

		// Libere o UNICODE
		RtlFreeUnicodeString(&Un);
	}
	else if (Io->Parameters.DeviceIoControl.IoControlCode == TERMINAR_PROCESSO_E_BLOQUEAR)
	{
		// ANSI_STRING e UNICODE
		ANSI_STRING As;
		UNICODE_STRING Un;

		// Inicie os valores
		RtlInitAnsiString(&As, MensagemUsuario);
		RtlAnsiStringToUnicodeString(&Un, &As, TRUE);

		// Inicie o PID
		ULONG Pid;
		RtlUnicodeStringToInteger(&Un, 0, &Pid);

		// Termine o processo
		if (NT_SUCCESS(TerminarProcesso(Pid)))
		{
			PEPROCESS Processo = NULL;

			// Obtenha o processo
			PsLookupProcessByProcessId((HANDLE*)Pid, &Processo);

			// Variaveis
			ANSI_STRING AsArquivo;
			UNICODE_STRING UnArquivo;

			// Arquivo completo
			PVOID Arquivo = ExAllocatePoolWithTag(PagedPool, 500, 'KIC5');

			if (Arquivo)
			{
				sprintf(
					Arquivo,
					"\\??\\%wZ",
					(UNICODE_STRING*)LocalProcesso(Processo)
				);

				RtlInitAnsiString(&AsArquivo, Arquivo);
				RtlAnsiStringToUnicodeString(&UnArquivo, &AsArquivo, TRUE);

				// Libere
				ExFreePoolWithTag(Arquivo, 'KIC5');

				// Variaveis
				OBJECT_ATTRIBUTES Atributos;
				IO_STATUS_BLOCK Io;
				HANDLE Alca;

				InitializeObjectAttributes(&Atributos, &UnArquivo, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

				// Abra
				NTSTATUS Status = ZwOpenFile(
					&Alca,
					GENERIC_READ,
					&Atributos,
					&Io,
					0,
					FILE_NON_DIRECTORY_FILE
				);

				if (NT_SUCCESS(Status))
				{
					StatusRetornar = "success!";
				}

				if (UnArquivo.Buffer != NULL)
				{
					// Libere o UNICODE
					RtlFreeUnicodeString(&UnArquivo);
				}
			}
		}

		if (Un.Buffer != NULL)
		{
			// Libere o UNICODE
			RtlFreeUnicodeString(&Un);
		}
	}
	// Deletar uma pasta
	else if (Io->Parameters.DeviceIoControl.IoControlCode == DELETAR_PASTA)
	{
		// Se conseguir
		if (NT_SUCCESS(ListarPasta(MensagemUsuario, TRUE)))
		{
			// Se conseguir
			StatusRetornar = "success!";
		}
	}

	// Máximo copiar para enviar ao user-mode
	int MaximoCopiar = 6;

	// Se for a mensagem de sucesso
	if (StatusRetornar == "success!")
	{
		// Altere o limite máximo
		MaximoCopiar = 9;
	}

	// Copie a mensagem para a memória do user-mode
	RtlCopyMemory(

		// Vamos enviar pra cá
		Irp->AssociatedIrp.SystemBuffer,

		// Status
		StatusRetornar,

		// Tamanho máximo da mensagem para retornar
		MaximoCopiar
	);

	// Sucesso
	Irp->IoStatus.Status = STATUS_SUCCESS;

	// Complete a requisição
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

/// <summary>
/// IRP fechado
/// </summary>
/// <param name="DeviceObject"></param>
/// <param name="Irp"></param>
/// <returns></returns>
NTSTATUS IRPFechado(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{


	// Informações e status
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	// Não incrimente mais nenhum valor, apenas, concluia o IRP
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	// Sucesso
	return STATUS_SUCCESS;
}

