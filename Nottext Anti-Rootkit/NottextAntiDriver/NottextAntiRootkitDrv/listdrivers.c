
/// <summary>
/// Estrutura para detectar os drivers carregados
/// Link: https://blog.csdn.net/qq1841370452/article/details/54755208
/// Link: https://blog.csdn.net/qq1841370452/article/details/81269038
/// </summary>
typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID      DllBase;
	PVOID      EntryPoint;
	ULONG32    SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	UINT32   Unknow[17];

}LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

typedef struct _LDR_DATA_TABLE_ENTRY_64
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID      DllBase;
	PVOID      EntryPoint;
	ULONG32    SizeOfImage;
	UINT8      Unknow0[0x4];
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
}LDR_DATA_TABLE_ENTRY_64, * PLDR_DATA_TABLE_ENTRY_64;

/// <summary>
/// Lista os drivers
/// </summary>
VOID ListarDrivers()
{

// Se for 64 bits
#if _WIN64
	// Tabela de entrada
	LDR_DATA_TABLE_ENTRY_64* primeiraLista, * segundaLista;
	primeiraLista = (LDR_DATA_TABLE_ENTRY_64*)ObjetoDriverGlobal->DriverSection;
#else
	LDR_DATA_TABLE_ENTRY* primeiraLista, * segundaLista;
	primeiraLista = (LDR_DATA_TABLE_ENTRY*)ObjetoDriverGlobal->DriverSection;
#endif

	
	PLIST_ENTRY Lista;

	// Flink
	Lista = primeiraLista->InLoadOrderLinks.Flink;

	// Procure os drivers
	while (Lista != &primeiraLista->InLoadOrderLinks)
	{
		// Se for NULL
		if (Lista == NULL || &primeiraLista->InLoadOrderLinks == NULL)
			break;

#if _WIN64
		// PLIST_ENTRY lista dupla circular ligada é conectada no início e no final, Flink representa o nó anterior
		segundaLista = (LDR_DATA_TABLE_ENTRY_64*)Lista;
#else
		segundaLista = (LDR_DATA_TABLE_ENTRY*)Lista;
#endif

		// Tente alocar espaço
		ULONG tamanho = segundaLista->FullDllName.Length + segundaLista->BaseDllName.Length + 8;
		PUNICODE_STRING NomeDriver = ExAllocatePoolWithTag(PagedPool, tamanho, 'drve');

		if (NomeDriver)
		{
			__try {
				// Converta
				//sprintf(NomeDriver, "%wZ Local: %wZ\r\n", &segundaLista->BaseDllName, &segundaLista->FullDllName);
				NTSTATUS Status = RtlStringCchPrintfA(NomeDriver, tamanho, "%wZ Local: %wZ\r\n", &segundaLista->BaseDllName, &segundaLista->FullDllName);

				if (NT_SUCCESS(Status))
				{
					// Nome do driver
					EscreverNoArquivo(DRIVERS_LOG, NomeDriver, FILE_APPEND_DATA);
				}
			} __except (EXCEPTION_EXECUTE_HANDLER){}

			ExFreePoolWithTag(NomeDriver, 'drve');

			Lista = Lista->Flink;
		}
	}
}


