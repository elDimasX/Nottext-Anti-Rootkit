//
// Função de porta de comunicação
//

/// <summary>
/// Quando o serviço usar o "FltConnectCommunicationPort"
/// </summary>
/// 
/// <param name="portaCliente">Vamos alterar o ClientePorta com este novo valor</param>
/// <param name="Cookie">Cookie</param>
/// <param name="Contexto">Contexto</param>
/// <param name="Tamanho">Tamanho</param>
/// <param name="ConexaoCookie">ConexaoCookie</param>
/// 
/// <returns>Retorna um NTSTATUS, se for OK, o serviço se conectará</returns>
NTSTATUS ServicoConectado(
	IN PFLT_PORT portaCliente, 
	IN PVOID Cookie,
	IN PVOID Contexto,
	IN ULONG Tamanho,
	IN PVOID ConexaoCookie
)
{

		// Altere o ClientePorta, informe as funções que o serviço foi conectado
		ClientePorta = portaCliente;

		// Sucesso
		return STATUS_SUCCESS;


}

/// <summary>
/// Quando o serviço for finalizado
/// </summary>
/// 
/// <param name="Cookie">Cookie</param>
/// 
/// <returns>Retorna um NTSTATUS</returns>
NTSTATUS ServicoDesconectado(
	IN PVOID Cookie // Sessão
)
{
	// Feche a porta de comunicação
	FltCloseClientPort(
		Filtro, // Filtro global
		&ClientePorta // O cliente
	);

	// NULL
	ClientePorta = NULL;
	return STATUS_SUCCESS;
}

/// <summary>
/// Quando o serviço enviar a mensagem para a porta
/// </summary>
/// 
/// <param name="portaCookie">Cookie</param>
/// <param name="Mensagem">Mensagem</param>
/// <param name="MensagemTamanho">Tamanho da mensagem</param>
/// <param name="EnviarMensagem">Enviar mensagem para o serviço</param>
/// <param name="EnviarMensagemTamanho">Tamanho da mensagem para enviar ao serviço</param>
/// <param name="Tamanho">Tamanho</param>
/// 
/// <returns></returns>
NTSTATUS ServicoMensagem(
	IN PVOID portaCookie,
	IN PVOID Mensagem,
	IN ULONG MensagemTamanho,
	OUT PVOID EnviarMensagem,
	OUT ULONG EnviarMensagemTamanho,
	IN PULONG Tamanho
)
{
	// Resete a lista
	LimparLista();

	// Leia novamente
	LerListaDePastasParaProteger();

	return STATUS_SUCCESS;
}

