
// Filtro para o sistema de arquivos
PFLT_FILTER FiltroMinifiltro;



/// <summary>
/// Requisição criada
/// </summary>
/// <param name="Data"></param>
/// <param name="Objects"></param>
/// <param name="Context"></param>
/// <returns></returns>
FLT_PREOP_CALLBACK_STATUS IrpMjCriado(
	_Inout_ PFLT_CALLBACK_DATA  Data,
	_In_ PCFLT_RELATED_OBJECTS Objetos,
	_In_ PVOID* Contexto
)
{
	PFLT_FILE_NAME_INFORMATION Informacao;
	NTSTATUS StatusRetornar = FLT_PREOP_SUCCESS_NO_CALLBACK;

	// Obtenha as informações
	NTSTATUS Status = FltGetFileNameInformation(
		Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &Informacao
	);

	if (NT_SUCCESS(Status))
	{
		// Analise a estrutura
		Status = FltParseFileNameInformation(Informacao);

		FltReleaseFileNameInformation(Informacao);
	}

	return StatusRetornar;
}


/// <summary>
/// Configura quais eventos queremos capturar
/// </summary>
const FLT_OPERATION_REGISTRATION Callbacks[] =
{

	{IRP_MJ_CREATE, 0, IrpMjCriado, NULL},
	{IRP_MJ_OPERATION_END}

};

// <summary>
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
