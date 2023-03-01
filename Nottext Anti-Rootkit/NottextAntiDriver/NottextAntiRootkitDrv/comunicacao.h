//
// Fun��o de porta de comunica��o
//

/// <summary>
/// Quando o servi�o usar o "FltConnectCommunicationPort"
/// </summary>
/// 
/// <param name="portaCliente">Vamos alterar o ClientePorta com este novo valor</param>
/// <param name="Cookie">Cookie</param>
/// <param name="Contexto">Contexto</param>
/// <param name="Tamanho">Tamanho</param>
/// <param name="ConexaoCookie">ConexaoCookie</param>
/// 
/// <returns>Retorna um NTSTATUS, se for OK, o servi�o se conectar�</returns>
NTSTATUS ServicoConectado(
	IN PFLT_PORT portaCliente, 
	IN PVOID Cookie,
	IN PVOID Contexto,
	IN ULONG Tamanho,
	IN PVOID ConexaoCookie
)
{

		// Altere o ClientePorta, informe as fun��es que o servi�o foi conectado
		ClientePorta = portaCliente;

		// Sucesso
		return STATUS_SUCCESS;


}

/// <summary>
/// Quando o servi�o for finalizado
/// </summary>
/// 
/// <param name="Cookie">Cookie</param>
/// 
/// <returns>Retorna um NTSTATUS</returns>
NTSTATUS ServicoDesconectado(
	IN PVOID Cookie // Sess�o
)
{
	// Feche a porta de comunica��o
	FltCloseClientPort(
		Filtro, // Filtro global
		&ClientePorta // O cliente
	);

	// NULL
	ClientePorta = NULL;
	return STATUS_SUCCESS;
}

/// <summary>
/// Quando o servi�o enviar a mensagem para a porta
/// </summary>
/// 
/// <param name="portaCookie">Cookie</param>
/// <param name="Mensagem">Mensagem</param>
/// <param name="MensagemTamanho">Tamanho da mensagem</param>
/// <param name="EnviarMensagem">Enviar mensagem para o servi�o</param>
/// <param name="EnviarMensagemTamanho">Tamanho da mensagem para enviar ao servi�o</param>
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

