
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
		if (NT_SUCCESS(ListarPasta(MensagemUsuario, FALSE, TRUE)))
		{
			// Se conseguir
			StatusRetornar = "success!";
		}
	}
	else if (Io->Parameters.DeviceIoControl.IoControlCode == DELETAR_ARQUIVO)
	{
		if (NT_SUCCESS(DeletarArquivo(MensagemUsuario, FALSE)))
		{
			StatusRetornar = "success!";
		}
	}
	else if (Io->Parameters.DeviceIoControl.IoControlCode == CRIAR_ARQUIVO)
	{
		IO_STATUS_BLOCK IoP;
		PFILE_OBJECT Ob;

		// ANSI e STRING
		UNICODE_STRING UnArquivo;
		ANSI_STRING AsArquivo;

		// Inicie
		RtlInitAnsiString(&AsArquivo, MensagemUsuario);
		RtlAnsiStringToUnicodeString(&UnArquivo, &AsArquivo, TRUE);
		
		// Crie o arquivo
		if (NT_SUCCESS(AbrirArquivoIRP(UnArquivo, GENERIC_ALL, &IoP, &Ob, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_DELETE)))
		{
			// Se conseguir
			StatusRetornar = "success!";

			// Libere o arquivo
			ObDereferenceObject(Ob);
		}

		if (UnArquivo.Buffer != NULL)
			RtlFreeUnicodeString(&UnArquivo);
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

		// Outro ANSI e UNICODE, usado para pegar o local do processo
		ANSI_STRING AsArquivo;
		UNICODE_STRING UnArquivo;

		// Inicie os valores
		RtlInitAnsiString(&As, MensagemUsuario);
		RtlAnsiStringToUnicodeString(&Un, &As, TRUE);

		// Inicie o PID
		ULONG Pid;
		
		// Converta UNICODE para INT
		RtlUnicodeStringToInteger(&Un, 0, &Pid);

		PEPROCESS Processo = NULL;

		// Obtenha o processo apartir do PID
		PsLookupProcessByProcessId((HANDLE*)Pid, &Processo);

		// Arquivo completo
		PVOID Arquivo = ExAllocatePoolWithTag(PagedPool, 800, 'KIC5');

		// Se conseguir alocar espaço
		if (Arquivo)
		{
			sprintf(
				Arquivo,
				"\\??\\%wZ",

				// Local do processo
				(UNICODE_STRING*)LocalProcesso(Processo)
			);

			// Inicie o local do arquivo
			RtlInitAnsiString(&AsArquivo, Arquivo);
			RtlAnsiStringToUnicodeString(&UnArquivo, &AsArquivo, TRUE);

			// Libere
			ExFreePoolWithTag(Arquivo, 'KIC5');

			IO_STATUS_BLOCK IoSt;
			PFILE_OBJECT ObjetoArquivo;

			// Abra o arquivo, mas não compartilhe nenhum acesso
			AbrirArquivoIRP(UnArquivo, GENERIC_READ | DELETE, &IoSt, &ObjetoArquivo, 0);

			// Termine o processo
			NTSTATUS Status = TerminarProcesso(Pid);

			if (NT_SUCCESS(Status))
			{
				StatusRetornar = "success!";
			}
		}

		if (Un.Buffer != NULL)
		{
			// Libere o UNICODE
			RtlFreeUnicodeString(&Un);
		}

		if (UnArquivo.Buffer != NULL)
		{
			// Libere o UNICODE
			RtlFreeUnicodeString(&UnArquivo);
		}
	}
	// Deletar uma pasta
	else if (Io->Parameters.DeviceIoControl.IoControlCode == DELETAR_PASTA)
	{
		// Se conseguir
		if (NT_SUCCESS(ListarPasta(MensagemUsuario, TRUE, TRUE)))
		{
			// Se conseguir
			StatusRetornar = "success!";
		}
	}
	// Salva o nome do arquivo para copiar depois
	else if (Io->Parameters.DeviceIoControl.IoControlCode == SALVAR_ARQUIVO_PARA_COPIAR)
	{
		if (NomeBackupCopiar != NULL)
		{
			ExFreePoolWithTag(NomeBackupCopiar, 'copy');
		}

		// Tente alocar espaço na memória
		NomeBackupCopiar = ExAllocatePoolWithTag(PagedPool, 800, 'copy');
		
		// Se conseguir
		if (NomeBackupCopiar != NULL)
		{
			// Se for menor que 690
			if (sizeof(MensagemUsuario) < 790)
			{
				// Copie o backup
				sprintf(
					NomeBackupCopiar,
					"%s",
					MensagemUsuario
				);
			}
		}
	}
	// Se for para copiar
	else if (Io->Parameters.DeviceIoControl.IoControlCode == COPIAR_ARQUIVO)
	{
		// Se conseguir copiar o arquivo
		if (NT_SUCCESS(CopiarArquivo(MensagemUsuario, NomeBackupCopiar)))
		{
			StatusRetornar = "success!";
		}
	}
	// Se for para renomear
	else if (Io->Parameters.DeviceIoControl.IoControlCode == RENOMEAR_ARQUIVO)
	{
		if (NT_SUCCESS(RenomearArquivo(MensagemUsuario)))
		{
			StatusRetornar = "success!";
		}
		/**
		// ANSI_STRING e UNICODE
		ANSI_STRING As;
		UNICODE_STRING Un;

		// AS e UNICODE para o novo nome
		ANSI_STRING AsNovo;
		UNICODE_STRING UnNovo;

		// Inicie os valores, o nome antigo
		RtlInitAnsiString(&As, NomeBackupCopiar);
		RtlAnsiStringToUnicodeString(&Un, &As, TRUE);

		// Inicie os valores, o novo nome
		RtlInitAnsiString(&AsNovo, MensagemUsuario);
		RtlAnsiStringToUnicodeString(&UnNovo, &AsNovo, TRUE);

		// Objeto do arquivo
		PFILE_OBJECT Objeto = NULL;
		IO_STATUS_BLOCK IoB;

		// Abra o arquivo
		NTSTATUS Status = AbrirArquivoIRP(Un, FILE_ALL_ACCESS, &IoB, &Objeto, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE);

		// Se conseguir
		if (NT_SUCCESS(Status))
		{
			// Aloque espaço
			PFILE_RENAME_INFORMATION Renomear = NULL;
			Renomear = (PFILE_RENAME_INFORMATION)ExAllocatePoolWithTag(PagedPool, sizeof(FILE_RENAME_INFORMATION) + UnNovo.MaximumLength, 'file');

			// Se conseguir alocar espaço
			if (Renomear)
			{
				// Zerar o buffer
				RtlZeroMemory(Renomear, sizeof(Renomear));
				Renomear->FileNameLength = UnNovo.Length;

				// Copie
				wcscpy(Renomear->FileName, UnNovo.Buffer);
				Renomear->ReplaceIfExists = TRUE;
				Renomear->RootDirectory = NULL;

				// Renomeie
				if (NT_SUCCESS(irpSetFileAttributes(Objeto, &IoB, Renomear, sizeof(FILE_RENAME_INFORMATION) + UnNovo.MaximumLength, FileRenameInformation, TRUE)))
				{
					StatusRetornar = "sucsess!";
				}

				// Libere o FILE_RENAME_INFORMATION
				ExFreePoolWithTag(Renomear, 'file');
			}

			// Libere o arquivo
			ObDereferenceObject(Objeto);
		}

		// Se não estiver vazio
		if (Un.Buffer != NULL)
			RtlFreeUnicodeString(&Un);

		// Vazio
		if (UnNovo.Buffer != NULL)
			RtlFreeUnicodeString(&UnNovo);
		*/
	}
	// Se for para esconder um processo
	else if (Io->Parameters.DeviceIoControl.IoControlCode == OCULTAR_PROCESSO)
	{
		// ANSI_STRING e UNICODE
		ANSI_STRING As;
		UNICODE_STRING Un;

		// Inicie os valores
		RtlInitAnsiString(&As, MensagemUsuario);
		RtlAnsiStringToUnicodeString(&Un, &As, TRUE);

		// Se conseguir
		if (Un.Buffer != NULL)
		{
			// Inicie o PID
			ULONG Pid;
			RtlUnicodeStringToInteger(&Un, 0, &Pid);

			OcultarProcesso(Pid);
			StatusRetornar = "success!";

			// Libere o UNICODE
			RtlFreeUnicodeString(&Un);
		}


	}
	// Se for para desligar o computador
	else if (Io->Parameters.DeviceIoControl.IoControlCode == DESLIGAR_COMPUTADOR)
	{
		// Desligue o PC
		DesligarOuReiniciarPC(TRUE);
	}
	// Se for para reiniciar o computador
	else if (Io->Parameters.DeviceIoControl.IoControlCode == REINICIAR_COMPUTADOR)
	{
		// Reinicie o PC
		DesligarOuReiniciarPC(FALSE);
	}
	// Se for para proteger um processo
	else if (Io->Parameters.DeviceIoControl.IoControlCode == PROTEGER_PROCESSO)
	{
		// ANSI_STRING e UNICODE
		ANSI_STRING As;
		UNICODE_STRING Un;

		// Inicie os valores
		RtlInitAnsiString(&As, MensagemUsuario);
		RtlAnsiStringToUnicodeString(&Un, &As, TRUE);

		// Se conseguir
		if (Un.Buffer != NULL)
		{
			// Inicie o PID
			ULONG Pid;
			RtlUnicodeStringToInteger(&Un, 0, &Pid);

			// Tente
			__try {

				// Protega o processo
				ProtegerProcesso(Pid);
				StatusRetornar = "success!";

			} __except (EXCEPTION_EXECUTE_HANDLER){}

			// Libere o UNICODE
			RtlFreeUnicodeString(&Un);
		}
	}
	// Se for para proteger um arquivo contra exclusão ou escrita
	else if (Io->Parameters.DeviceIoControl.IoControlCode == PROTEGER_ARQUIVO)
	{
		// ANSI_STRING e UNICODE
		ANSI_STRING As;
		UNICODE_STRING Un;

		// Inicie os valores
		RtlInitAnsiString(&As, MensagemUsuario);
		RtlAnsiStringToUnicodeString(&Un, &As, TRUE);

		// Se conseguir
		if (Un.Buffer != NULL)
		{
			IO_STATUS_BLOCK IoSt;
			PFILE_OBJECT ObjetoArquivo = NULL;

			// Se abrir o arquivo
			NTSTATUS Status = AbrirArquivoIRP(Un, GENERIC_READ | DELETE, &IoSt, &ObjetoArquivo, FILE_SHARE_READ);

			if (NT_SUCCESS(Status))
			{
				StatusRetornar = "success!";
			}

			// Libere o UNICODE
			RtlFreeUnicodeString(&Un);
		}
	}
	// Se for para listar os drivers
	else if (Io->Parameters.DeviceIoControl.IoControlCode == LISTAR_DRIVERS)
	{
		ListarDrivers();
		StatusRetornar = "success!";
	}
	// Descarrega um serviço
	else if (Io->Parameters.DeviceIoControl.IoControlCode == DESCARREGAR_SERVICO)
	{
		if (NT_SUCCESS(CarregarOuDescarregarServico(MensagemUsuario, TRUE)))
		{
			StatusRetornar = "success!";
		}
	}
	// Se for para listar os serviços
	else if (Io->Parameters.DeviceIoControl.IoControlCode == LISTAR_SERVICOS)
	{
		ListarServicos();
		StatusRetornar = "success!";
	}
	// Carrega um serviço
	else if (Io->Parameters.DeviceIoControl.IoControlCode == CARREGAR_SERVICO)
	{
		if (NT_SUCCESS(CarregarOuDescarregarServico(MensagemUsuario, FALSE)))
		{
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

