
#include "header.h"
#include "files.h"
#include "irps.h"
#include "process.h"
#include "hideprocess.h"
#include "minifilter.h"

/*


	Status = IoCreateFile(&Alca, 0x80000000, &Atributos,
		&Io, 0, 0, 3u, 1u, 1u, 0, 0, 0, 0,
		0x100u);

	if (Status < 0)
	{
		DbgPrint("fudeu 1");
		return Status;
	}

	PFILE_OBJECT SystemRootFileObject;
	Status = ObReferenceObjectByHandle(Alca, 1u, 0, 0,&SystemRootFileObject, 0);

	if (Status < 0)
	{
		DbgPrint("fudeu 2");
		return Status;
	}

	PDEVICE_OBJECT TargetDevice = IoGetRelatedDeviceObject(SystemRootFileObject);

	if (!TargetDevice)
	{
		DbgPrint("fudeu 2");
		return STATUS_UNSUCCESSFUL;
	}

	ObfReferenceObject(TargetDevice);
	PDEVICE_OBJECT SourceDevice;

	Status = IoCreateDevice(ObjetoDriver, 0xCu, 0, TargetDevice->DeviceType,
		TargetDevice->Characteristics, 0, &SourceDevice);

	if (Status < 0)
	{
		DbgPrint("fudeu 3");
		return Status;
	}

	PDEVICE_OBJECT DeviceAttachedTo = IoAttachDeviceToDeviceStack(SourceDevice,
		TargetDevice);

	if (!DeviceAttachedTo)
	{
		DbgPrint("fudeu 4");
		IoDeleteDevice(SourceDevice);
		return STATUS_UNSUCCESSFUL;
	}

	return STATUS_SUCCESS;

*/

/// <summary>
/// Inicia o driver
/// </summary>
/// <param name="DriverObject"></param>
/// <param name="RegistryPath"></param>
/// <returns></returns>
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

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
		else {

			// Registre
			Status = FltRegisterFilter(DriverObject, &Registracao, &FiltroMinifiltro);

			if (NT_SUCCESS(Status))
			{
				Status = FltStartFiltering(FiltroMinifiltro);
			}
		}

		// Configure as mensagens
		DriverObject->MajorFunction[IRP_MJ_CREATE] = IRPCriado;
		DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IRPRecebido;
		DriverObject->MajorFunction[IRP_MJ_CLOSE] = IRPFechado;
	}

	return STATUS_SUCCESS;
}

/// <summary>
/// Função que descarrega o filtro
/// </summary>
NTSTATUS DescarregarDriver(
	_In_ FLT_FILTER_UNLOAD_FLAGS Descarregar
)
{
	// Vamos remover o filtro
	FltUnregisterFilter(FiltroMinifiltro);

	IoDeleteDevice(DispositivoGlobal);
	IoDeleteSymbolicLink(&SysNome);
	return STATUS_SUCCESS;
}
