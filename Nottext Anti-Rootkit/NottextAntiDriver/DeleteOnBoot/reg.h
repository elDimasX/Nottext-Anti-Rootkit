

/// <summary>
/// Lista todos os serviços na máquina
/// </summary>
VOID ListarServicos()
{
	// Registro
	UNICODE_STRING Registro;
	RtlInitUnicodeString(&Registro, L"\\Registry\\Machine\\System\\CurrentControlSet\\Services");

	// Alça e atributos
	HANDLE Alca;
	OBJECT_ATTRIBUTES Atributos;

	// Inicia os atributos
	InitializeObjectAttributes(&Atributos, &Registro, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	// Abre a chave
	NTSTATUS Status = ZwOpenKey(&Alca, KEY_ALL_ACCESS, &Atributos);

	// Se conseguir
	if (NT_SUCCESS(Status))
	{
		// Obtenha as chaves
		ULONG Tamanho;
		ZwQueryKey(Alca, KeyFullInformation, NULL, 0, &Tamanho);

		// Aloque espaço
		PKEY_FULL_INFORMATION PInfo = (PKEY_FULL_INFORMATION)ExAllocatePoolWithTag(PagedPool, Tamanho, 'tagr');

		// Se conseguir
		if (PInfo)
		{
			// Obtenha as chaves
			Status = ZwQueryKey(Alca, KeyFullInformation, PInfo, Tamanho, &Tamanho);

			if (NT_SUCCESS(Status))
			{
				// Procure as subchaves
				for (ULONG i = 0; i < PInfo->SubKeys; i++)
				{
					// Enumere as chaves
					ZwEnumerateKey(Alca, i, KeyBasicInformation, NULL, 0, &Tamanho);

					// Aloque espaço
					PKEY_BASIC_INFORMATION Info = (PKEY_BASIC_INFORMATION)ExAllocatePoolWithTag(PagedPool, Tamanho, 'tagr');

					// Se conseguir alocar
					if (Info)
					{
						// Enumere a chave
						Status = ZwEnumerateKey(Alca, i, KeyBasicInformation, Info, Tamanho, &Tamanho);

						// Se conseguir
						if (NT_SUCCESS(Status))
						{
							// Configure o UNICODE
							UNICODE_STRING unikeyName;
							unikeyName.Length =unikeyName.MaximumLength = (USHORT)Info->NameLength;
							unikeyName.Buffer = Info->Name;

							// Serviço
							PUNICODE_STRING Servico = ExAllocatePoolWithTag(PagedPool, 800, 'regs');
							PUNICODE_STRING LocalCompleto = ExAllocatePoolWithTag(PagedPool, 500, 'locl');

							// Se conseguir alocar espaço
							if (Servico)
							{
								// Converta
								sprintf(Servico, "%wZ", &unikeyName);

								// Local completo, serve para depois, obter o local do arquivo
								if (LocalCompleto)
								{
									// Concatene
									strcpy(LocalCompleto, "\\Registry\\Machine\\System\\CurrentControlSet\\Services\\");
									strcat(LocalCompleto, Servico);

									// Agora, obtenha o local completo
									if (LocalArquivo(LocalCompleto, Servico) == TRUE)
									{
										// Escreva
										EscreverNoArquivo(SERVICOS_LOG, Servico, FILE_APPEND_DATA);
									}

									// Libere
									ExFreePoolWithTag(LocalCompleto, 'regs');
								}

								// Libere
								ExFreePoolWithTag(Servico, 'regs');
							}
						}

						// Libere o pool alocado
						ExFreePoolWithTag(Info, 'tagr');
					}
				}
			}

			// Libere
			ExFreePoolWithTag(PInfo, 'tagr');
		}

		// Feche a alça
		ZwClose(Alca);
	}
}

/// <summary>
/// Verifica se é pra iniciar ou parar um serviço
/// </summary>
/// <param name="NomeServico"></param>
/// <param name="Parar"></param>
/// <returns></returns>
NTSTATUS CarregarOuDescarregarServico(PUNICODE_STRING NomeServico, BOOLEAN Parar)
{
	// ANSI_STRING e UNICODE
	ANSI_STRING As;
	UNICODE_STRING Un;
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	// Inicie os valores
	RtlInitAnsiString(&As, NomeServico);
	RtlAnsiStringToUnicodeString(&Un, &As, TRUE);

	// Se conseguir
	if (Un.Buffer != NULL)
	{
		if (Parar == TRUE)
		{
			// Tente descarregar o serviço
			Status = ZwUnloadDriver(&Un);
		}
		else {
			// Tente carregar o serviço
			Status = ZwLoadDriver(&Un);
		}

		// Libere o UNICODE
		RtlFreeUnicodeString(&Un);
	}

	return Status;
}

/// <summary>
/// Retorna a imagem do arquivo
/// </summary>
/// <param name="NomeServico"></param>
/// <returns></returns>
BOOLEAN LocalArquivo(PUNICODE_STRING NomeServico, PUNICODE_STRING RetornarValor)
{
	// ANSI_STRING e UNICODE
	ANSI_STRING As;
	UNICODE_STRING Un;
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	// Inicie os valores
	RtlInitAnsiString(&As, NomeServico);
	RtlAnsiStringToUnicodeString(&Un, &As, TRUE);

	// Se conseguir
	if (Un.Buffer != NULL)
	{
		// Varivaveis
		HANDLE Alca = NULL;
		NTSTATUS Status = STATUS_SUCCESS;
		OBJECT_ATTRIBUTES Atributos;

		// Valor que vamos obter
		UNICODE_STRING ImagemLocal = RTL_CONSTANT_STRING(L"ImagePath");

		// Informações
		PKEY_VALUE_PARTIAL_INFORMATION AcKeyInfo = NULL;
		ULONG Length = 0;

		// Inicie os atributos
		InitializeObjectAttributes(&Atributos, &Un, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

		// Abra a chave
		Status = ZwOpenKey(&Alca, KEY_READ | KEY_WRITE, &Atributos);

		// Se conseguir
		if (NT_SUCCESS(Status))
		{
			// Leia o valor da chave de registro pela primeira vez para detectar o tamanho dos dados
			Status = ZwQueryValueKey( Alca, &ImagemLocal, KeyValuePartialInformation, NULL, 0, &Length);

			// Se falhar
			if (!NT_SUCCESS(Status) && Status != STATUS_BUFFER_OVERFLOW && Status != STATUS_BUFFER_TOO_SMALL)
			{
				// Feche a alça
				ZwClose(Alca);
				return FALSE;
			}

			// Alocar memória dinamicamente com base no tamanho da chave de registro detectada acima
			AcKeyInfo = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePoolWithTag(PagedPool, Length, 'tag2');

			// Se conseguir alocar
			if (AcKeyInfo)
			{
				// Leia a chave de registro novamente
				Status = ZwQueryValueKey(Alca, &ImagemLocal, KeyValuePartialInformation, AcKeyInfo, Length, &Length);

				// Se conseguir
				if (NT_SUCCESS(Status))
				{
					// UNICODE
					UNICODE_STRING UnRetornar;
					RtlInitUnicodeString(&UnRetornar, AcKeyInfo->Data);

					// Local do arquivo
					PUNICODE_STRING LocalArquivo = ExAllocatePoolWithTag(PagedPool, 800, 'tagr');

					if (LocalArquivo)
					{
						// Converta para retornar
						sprintf(LocalArquivo, " Local: %wZ\r\n", &UnRetornar);

						// Concatene
						strcat(RetornarValor, LocalArquivo);

						// Libere o POOL
						ExFreePoolWithTag(LocalArquivo, 'tagr');
					}
				}
				
				// Libere o POOL
				ExFreePoolWithTag(AcKeyInfo, 'tag2');
			}

			// Feche a alça
			ZwClose(Alca);
		}

		// Libere o UNICODE
		RtlFreeUnicodeString(&Un);
	}

	return TRUE;
}
