
#include "header.h"
#include "files.h"
#include "irps.h"
#include "process.h"

/// <summary>
/// Inicia o driver
/// </summary>
/// <param name="DriverObject"></param>
/// <param name="RegistryPath"></param>
/// <returns></returns>
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	ListarProcessos();

	// 1 Segundo
	Tempo.QuadPart = -1000 * -1000 * 1;

	// Crie o dispositivo para comunicações
	NTSTATUS Status = IoCreateDevice(
		DriverObject,
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
		}

		// Configure as mensagens
		DriverObject->MajorFunction[IRP_MJ_CREATE] = IRPCriado;
		DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IRPRecebido;
		DriverObject->MajorFunction[IRP_MJ_CLOSE] = IRPFechado;
	}

	DriverObject->DriverUnload = Unload;

	return STATUS_SUCCESS;
}

/// <summary>
/// Descarrega o driver
/// </summary>
/// <param name="DriverObject"></param>
VOID Unload(_In_ PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);

	IoDeleteDevice(DispositivoGlobal);
	IoDeleteSymbolicLink(&SysNome);
}

