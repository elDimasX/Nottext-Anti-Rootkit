
// Filtro para o sistema de arquivos
PFLT_FILTER FiltroMinifiltro;

UNICODE_STRING LockerExtensionsToScan[] = {
	RTL_CONSTANT_STRING(L"txt"),
	{0, 0, NULL}
};

/// <summary>
/// Requisição criada
/// </summary>
/// <param name="Data"></param>
/// <param name="Objects"></param>
/// <param name="Context"></param>
/// <returns></returns>
FLT_PREOP_CALLBACK_STATUS IrpMjCriado(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS Objetos,
	_In_ PVOID* Contexto
)
{
	// Arquivo
	PFILE_OBJECT ObjetoArquivo = Data->Iopb->TargetFileObject;
	NTSTATUS StatusRetornar = FLT_PREOP_SUCCESS_NO_CALLBACK;

	return StatusRetornar;

	// Valores
	ULONG_PTR minimo;
	ULONG_PTR maximo;

	// A rotina IoGetStackLimits retorna os limites do quadro de pilha do segmento atual. 
	IoGetStackLimits(
		&minimo, // O minímo
		&maximo // O máximo
	);

	if (
		// Verifique se os nomes não são NULL
		((ULONG_PTR)ObjetoArquivo > minimo) &&
		((ULONG_PTR)ObjetoArquivo < maximo))
	{
		return StatusRetornar;
	}

	UNREFERENCED_PARAMETER(Contexto);
	UNREFERENCED_PARAMETER(Objetos);

	if (Data->RequestorMode == KernelMode)
		return StatusRetornar;

	PFLT_FILE_NAME_INFORMATION Informacao;

	// Obtenha as informações
	NTSTATUS Status = FltGetFileNameInformation(
		Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &Informacao
	);

	if (NT_SUCCESS(Status))
	{
		// Analise a estrutura
		Status = FltParseFileNameInformation(Informacao);

		if (NT_SUCCESS(Status))
		{
			// Se for algum arquivo que queremos proteger
			if (

				// Se o arquivo estiver sendo deletado
				FlagOn(Data->Iopb->Parameters.Create.Options, FILE_DELETE_ON_CLOSE) ||

				// OU modificando o arquivo
				(Data->Iopb->Parameters.Create.SecurityContext->DesiredAccess &
					(FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES | FILE_WRITE_EA |
						FILE_APPEND_DATA | DELETE | WRITE_DAC | WRITE_OWNER))
				)
			{
				// Aloque espaço
				PCHAR Arquivo = ExAllocatePoolWithTag(PagedPool, Informacao->Name.Length, 'file');

				if (Arquivo)
				{
					// Obtenha o nome do arquivo
					sprintf(
						Arquivo,
						"C:%ws",
						ObjetoArquivo->FileName.Buffer
					);

					// Deixa tudo em maiusculo
					_strupr(Arquivo);

					// Se for algum destes arquivos
					if (strstr(Arquivo, "NOTTEXT ANTI-ROOTKIT.EXE") || strstr(Arquivo, "KERNELPROCESSLIST.EXE") || strstr(Arquivo, "CLEANUP.EXE"))
					{
						Data->IoStatus.Status = STATUS_ACCESS_DENIED;
						Data->IoStatus.Information = 0;
						StatusRetornar = FLT_PREOP_COMPLETE;
					}

					// Libere o valor alocado
					ExFreePoolWithTag(Arquivo, 'file');
				}
			}
		}

		if (Informacao != NULL)
		{
			FltReleaseFileNameInformation(Informacao);
		}
	}

	return StatusRetornar;
}


/// <summary>
/// Configura quais eventos queremos capturar
/// </summary>
const FLT_OPERATION_REGISTRATION Callbacks[] =
{

	{IRP_MJ_CREATE, 0, IrpMjCriado, NULL},
	//{IRP_MJ_DIRECTORY_CONTROL, 0, IrpMjCriado, NULL},
	{IRP_MJ_OPERATION_END}

};

/// <summary>
/// Configurações para o registro
/// </summary>
const FLT_REGISTRATION Registracao =
{
	// Tamanho
	sizeof(FLT_REGISTRATION),

	// Versão
	FLT_REGISTRATION_VERSION,
	0,
	NULL,

	// As callbacks
	Callbacks,

	// Função de descarregar
	DescarregarDriver, 
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL
};
