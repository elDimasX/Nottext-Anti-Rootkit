

#define MAX_PATH          600 
/// https://bbs.pediy.com/thread-264070.htm
/// https://titanwolf.org/Network/Articles/Article?AID=4520fea9-1486-4905-8dfc-5bbb14fd5db3#gsc.tab=0

/*
	Operações para forçar a remoção de um arquivo
*/

/// <summary>
/// Abre um objeto apartir de um ponteiro
/// </summary>
/// 
/// <returns>Retorna um NTSTATUS</returns>
NTSTATUS ObOpenObjectByPointer(
	PVOID Object,
	ULONG HandleAttributes,
	PACCESS_STATE PassedAccessState,
	ACCESS_MASK DesiredAccess,
	POBJECT_TYPE ObjectType,
	KPROCESSOR_MODE AccessMode,
	PHANDLE Handle
);


/// <summary>
/// Esta função aloca espaço para um objeto NT de qualquer
/// Pool paginado ou não paginado.Ele captura o nome opcional e
/// Parâmetros SECURITY_DESCRIPTOR para uso posterior quando o objeto é
/// inserido em uma tabela de objetos. Nenhuma cota é cobrada neste momento.
/// Isso ocorre quando o objeto é inserido em uma tabela de objetos.
/// </summary>
/// 
/// <returns>Retorna um NTSTATUS</returns>
NTSTATUS ObCreateObject(
	KPROCESSOR_MODE ProbeMode,
	POBJECT_TYPE ObjectType,
	POBJECT_ATTRIBUTES ObjectAttributes,
	KPROCESSOR_MODE OwnershipMode,
	PVOID ParseContext,
	ULONG ObjectBodySize,
	ULONG PagedPoolCharge,
	ULONG NonPagedPoolCharge,
	PVOID* Object
);

/// <summary>
/// sua rotina inicializa uma estrutura ACCESS_STATE.  Isto consiste do:
/// -zerando toda a estrutura
/// 
/// -mapeamento de tipos de acesso genéricos no DesiredAccess passado
/// e colocá - lo na estrutura
/// 
/// -"capturar" o Contexto do Assunto, que deve ser realizado para o
/// duração da tentativa de acesso(pelo menos até que a auditoria seja realizada).
/// 
/// -Alocar um ID de operação, que é um LUID que será usado
/// associar diferentes partes da tentativa de acesso na auditoria registro.
/// </summary>
/// 
/// <param name="AccessState">Acesso e estado</param>
/// <param name="AuxData">AuxData</param>
/// <param name="DesiredAccess">Retornar acesso</param>
/// <param name="GenericMapping">2Arquivo de mapeamento</param>
/// 
/// <returns>Retorna um NTSTATUS</returns>
NTSTATUS SeCreateAccessState(PACCESS_STATE AccessState, PVOID AuxData, ACCESS_MASK DesiredAccess, PGENERIC_MAPPING GenericMapping);

/// <summary>
/// Estrutura
/// </summary>
typedef struct _AUX_ACCESS_DATA {
	PPRIVILEGE_SET PrivilegesUsed;
	GENERIC_MAPPING GenericMapping;
	ACCESS_MASK AccessesToAudit;
	ACCESS_MASK MaximumAuditMask;
	ULONG Unknown[256];
} AUX_ACCESS_DATA, * PAUX_ACCESS_DATA;

// Obtém o dispositivo
NTSTATUS ObterDriverDispositivo(_In_ PUNICODE_STRING NomeDriver, _Out_ PDEVICE_OBJECT* ObjetoDispositivo, _Out_ PDEVICE_OBJECT* LerDispositivo);

/// <summary>
/// Obtém o dispositivo
/// </summary>
/// 
/// <param name="NomeDriver">Nome do driver de dispositivo para se conectar</param>
/// <param name="ObjetoDispositivo">Referência para um DEVICE_OBJECT</param>
/// <param name="LerDispositivo">Ler dispositivo</param>
/// 
/// <returns>Retorna um NTSTATUS</returns>
NTSTATUS ObterDriverDispositivo(_In_ PUNICODE_STRING NomeDriver, _Out_ PDEVICE_OBJECT* ObjetoDispositivo, _Out_ PDEVICE_OBJECT* LerDispositivo)
{

	// Status
	NTSTATUS Status;

	// Atributos e outros
	OBJECT_ATTRIBUTES Atributos;
	HANDLE DeviceHandle = NULL;
	IO_STATUS_BLOCK ioStatus;
	PFILE_OBJECT ObjetoArquivo;

	// Verifique se algum é NULL
	if (NomeDriver == NULL || ObjetoDispositivo == NULL || LerDispositivo == NULL)
		return STATUS_INVALID_PARAMETER;

	// Inicie os atributos
	InitializeObjectAttributes(&Atributos, NomeDriver, OBJ_CASE_INSENSITIVE, NULL, NULL);

	// Abra o arquivo (Driver, não o arquivo que mandamos)
	Status = IoCreateFile(
		&DeviceHandle,
		SYNCHRONIZE | FILE_ANY_ACCESS,
		&Atributos,
		&ioStatus, NULL, 0,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		FILE_OPEN,
		FILE_SYNCHRONOUS_IO_NONALERT | FILE_DIRECTORY_FILE,
		NULL, 0,
		CreateFileTypeNone, NULL,
		IO_NO_PARAMETER_CHECKING
	);

	// Se falhar
	if (!NT_SUCCESS(Status))
	{
		return Status;
	}

	// Acesse o indentificador da alça
	Status = ObReferenceObjectByHandle(DeviceHandle, FILE_READ_DATA, *IoFileObjectType, KernelMode, &ObjetoArquivo, NULL);

	// Se falhar
	if (!NT_SUCCESS(Status))
	{
		ZwClose(DeviceHandle);
		return Status;
	}

	// Se for 0
	if (ObjetoArquivo->Vpb == 0 || ObjetoArquivo->Vpb->RealDevice == NULL)
	{
		// Saia
		ObDereferenceObject(ObjetoArquivo);
		ZwClose(DeviceHandle);
		return STATUS_UNSUCCESSFUL;
	}

	// Obtenha os dispositivos
	*ObjetoDispositivo = ObjetoArquivo->Vpb->DeviceObject;
	*LerDispositivo = ObjetoArquivo->Vpb->RealDevice;

	// Feche
	ObDereferenceObject(ObjetoArquivo);
	ZwClose(DeviceHandle);

	return STATUS_SUCCESS;
}

#define SYMBOLICLINKLENG            6 

/// <summary>
/// Abre um arquivo com IRP
/// </summary>
/// 
/// <param name="Arquivo">UNICODE para o arquivo</param>
/// <param name="Acesso">Acesso apra abertuda</param>
/// <param name="Io">Io</param>
/// <param name="ObjetoArquivo">Rerêrencia para salvar o PFILE_OBJECT da sessão</param>
/// 
/// <returns>Retornau um NTSTATUS</returns>
NTSTATUS AbrirArquivoIRP(_In_ UNICODE_STRING Arquivo, _In_ ACCESS_MASK Acesso, _In_ PIO_STATUS_BLOCK Io, _Out_ PFILE_OBJECT* ObjetoArquivo, USHORT CompartilharAcesso)
{
	// Status
	NTSTATUS Status;

	// IRP e evento
	PIRP Irp;
	KEVENT Evento;

	// Acesso
	static ACCESS_STATE AccessState;
	static AUX_ACCESS_DATA AuxData;

	// Atributos e contexto
	OBJECT_ATTRIBUTES Atributos;
	PFILE_OBJECT NovoObjetoArquivo;
	IO_SECURITY_CONTEXT SecurityContext;
	PIO_STACK_LOCATION IrpSp;

	// Dispositivos
	PDEVICE_OBJECT ObjetoDispositivo = NULL;
	PDEVICE_OBJECT LerDispositivo = NULL;

	// Nome do driver
	UNICODE_STRING NomeDriver;
	wchar_t* ArquivoBuf = NULL;
	static wchar_t Caminho[MAX_PATH] = { 0 };

	// Zere a memória
	RtlZeroMemory(Caminho, sizeof(Caminho));

	// Copie o valor
	RtlCopyMemory(Caminho, Arquivo.Buffer, (SYMBOLICLINKLENG + 1) * sizeof(wchar_t));

	// Inicie o UNICODE_STRING
	RtlInitUnicodeString(&NomeDriver, Caminho);

	// Obtenha o driver
	Status = ObterDriverDispositivo(&NomeDriver, &ObjetoDispositivo, &LerDispositivo);

	// Se falhar
	if (!NT_SUCCESS(Status))
	{
		// Pare
		RtlZeroMemory(Caminho, sizeof(Caminho));
		return Status;
	}

	// Zere a memória
	RtlZeroMemory(Caminho, sizeof(Caminho));

	// Copie
	RtlCopyMemory(Caminho, &Arquivo.Buffer[SYMBOLICLINKLENG], Arquivo.Length - SYMBOLICLINKLENG);

	// Inicie o UNICODE
	RtlInitUnicodeString(&NomeDriver, Caminho);

	// Aloque
	ArquivoBuf = ExAllocatePool(NonPagedPool, NomeDriver.MaximumLength);

	// Se não conseguir
	if (ArquivoBuf == NULL)
	{
		// Pare
		RtlZeroMemory(Caminho, sizeof(Caminho));
		return STATUS_UNSUCCESSFUL;
	}

	// Zere
	RtlZeroMemory(ArquivoBuf, NomeDriver.MaximumLength);

	// Copie para o ArquivoBuf
	RtlCopyMemory(ArquivoBuf, NomeDriver.Buffer, NomeDriver.Length);

	// Se for NULL
	if (ObjetoDispositivo == NULL || LerDispositivo == NULL || ObjetoDispositivo->StackSize <= 0)
	{
		// Pare
		RtlZeroMemory(Caminho, sizeof(Caminho));
		ExFreePool(ArquivoBuf);
		return STATUS_UNSUCCESSFUL;
	}

	// Limpe o 'Caminho', não queremos ocupar espaço na memória
	RtlZeroMemory(Caminho, sizeof(Caminho));

	// Inicie os atributos
	InitializeObjectAttributes(&Atributos, NULL, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, 0, NULL);

	// Inicie o objeto
	Status = ObCreateObject(KernelMode, *IoFileObjectType, &Atributos, KernelMode, NULL, sizeof(FILE_OBJECT), 0, 0, &NovoObjetoArquivo);

	// Se falhar
	if (!NT_SUCCESS(Status))
	{
		ExFreePool(ArquivoBuf);
		return Status;
	}

	// Aloque o IRP
	Irp = IoAllocateIrp(ObjetoDispositivo->StackSize, FALSE);

	// Se não conseguir
	if (Irp == NULL)
	{
		// Realize as operações
		ExFreePool(ArquivoBuf);
		ObDereferenceObject(NovoObjetoArquivo);
		return STATUS_UNSUCCESSFUL;
	}

	// Inicie o evento
	KeInitializeEvent(&Evento, SynchronizationEvent, FALSE);

	// Zere o valor
	RtlZeroMemory(NovoObjetoArquivo, sizeof(FILE_OBJECT));
	NovoObjetoArquivo->Type = IO_TYPE_FILE;
	NovoObjetoArquivo->Size = sizeof(FILE_OBJECT);
	NovoObjetoArquivo->DeviceObject = LerDispositivo;
	NovoObjetoArquivo->Flags = FO_SYNCHRONOUS_IO;

	// Inicie o UNICODE
	RtlInitUnicodeString(&NovoObjetoArquivo->FileName, ArquivoBuf);

	// Inicie o evento
	KeInitializeEvent(&NovoObjetoArquivo->Lock, SynchronizationEvent, FALSE);
	KeInitializeEvent(&NovoObjetoArquivo->Event, NotificationEvent, FALSE);

	// Incie a estrutura de acesso
	Status = SeCreateAccessState(&AccessState, &AuxData, Acesso, IoGetFileObjectGenericMapping());

	// Se falhar
	if (!NT_SUCCESS(Status))
	{
		// Libere o IRP
		IoFreeIrp(Irp);
		ObDereferenceObject(NovoObjetoArquivo);
		return Status;
	}

	SecurityContext.SecurityQos = NULL;
	SecurityContext.AccessState = &AccessState;
	SecurityContext.DesiredAccess = Acesso;
	SecurityContext.FullCreateOptions = 0;

	// Configure o IRP
	Irp->MdlAddress = NULL;
	Irp->AssociatedIrp.SystemBuffer = NULL;
	Irp->Flags = IRP_CREATE_OPERATION | IRP_SYNCHRONOUS_API;
	
	Irp->RequestorMode = KernelMode;
	Irp->UserIosb = Io;
	Irp->UserEvent = &Evento;
	Irp->PendingReturned = FALSE;
	Irp->Cancel = FALSE;
	Irp->CancelRoutine = NULL;
	Irp->Tail.Overlay.Thread = PsGetCurrentThread();
	Irp->Tail.Overlay.AuxiliaryBuffer = NULL;
	Irp->Tail.Overlay.OriginalFileObject = NovoObjetoArquivo;

	// Proximo local do IRP
	IrpSp = IoGetNextIrpStackLocation(Irp);
	IrpSp->MajorFunction = IRP_MJ_CREATE;
	IrpSp->DeviceObject = ObjetoDispositivo;
	IrpSp->FileObject = NovoObjetoArquivo;
	IrpSp->Parameters.Create.SecurityContext = &SecurityContext;
	IrpSp->Parameters.Create.Options = (FILE_OPEN_IF << 24) | FILE_SYNCHRONOUS_IO_NONALERT | FILE_OPEN_FOR_BACKUP_INTENT;
	IrpSp->Parameters.Create.FileAttributes = FILE_ATTRIBUTE_NORMAL;

	IrpSp->Parameters.Create.ShareAccess = CompartilharAcesso;

	IrpSp->Parameters.Create.EaLength = 0;

	// Complete o IRP
	IoSetCompletionRoutine(Irp, CompletarAtributo, 0, TRUE, TRUE, TRUE);

	// Chame o Driver de dispositivo
	Status = IoCallDriver(ObjetoDispositivo, Irp);

	// Se for pendente
	if (Status == STATUS_PENDING)
	{
		// Espere
		KeWaitForSingleObject(&Evento, Executive, KernelMode, TRUE, 0);
	}

	// Se falhar
	if (!NT_SUCCESS(Status))
	{
		// Zere
		NovoObjetoArquivo->DeviceObject = NULL;
		ObDereferenceObject(NovoObjetoArquivo);
	}

	// Se não falahr
	else
	{
		// Incrementa um valor de váriavel
		InterlockedIncrement(&NovoObjetoArquivo->DeviceObject->ReferenceCount);

		//
		if (NovoObjetoArquivo->Vpb)
		{
			InterlockedIncrement(&NovoObjetoArquivo->Vpb->ReferenceCount);
		}

		// FileObject
		*ObjetoArquivo = NovoObjetoArquivo;

		//ObDereferenceObject(pNewFileObject);  
	}

	return Status;
}

/// <summary>
/// Lista uma pasta com IRP direto
/// </summary>
/// 
/// <param name="ObjetoPasta">Referência para um FILE_OBJECT</param>
/// <param name="Io">Io</param>
/// <param name="ArquivoInformacao">Informação</param>
/// <param name="Tamanho">Tamanho</param>
/// <param name="InformacaoClasse">Tamanho da estrutura</param>
/// <param name="OPTIONAL">Arquivo</param>
/// 
/// <returns>Retorna um NTSTATUS</returns>
NTSTATUS ListarPastaIRP(_In_ PFILE_OBJECT ObjetoPasta, _Out_ PIO_STATUS_BLOCK Io, _Out_ PVOID ArquivoInformacao, _In_ ULONG Tamanho, _In_ FILE_INFORMATION_CLASS InformacaoClasse, _In_ PUNICODE_STRING Arquivo OPTIONAL)
{
	// Variaveis
	NTSTATUS Status;
	PIRP Irp;
	KEVENT kEvent;
	PIO_STACK_LOCATION IrpSp;

	// Se não tiver nada
	if (ObjetoPasta->Vpb == 0 || ObjetoPasta->Vpb->DeviceObject == NULL)
		return STATUS_UNSUCCESSFUL;

	// Aloque um IRP
	Irp = IoAllocateIrp(ObjetoPasta->Vpb->DeviceObject->StackSize, FALSE);
	if (Irp == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	// Inicie o KEVENT
	KeInitializeEvent(&kEvent, SynchronizationEvent, FALSE);

	// Zere com o tamanho
	RtlZeroMemory(ArquivoInformacao, Tamanho);

	// Configure o Irp
	Irp->UserEvent = &kEvent;
	Irp->UserIosb = Io;
	Irp->UserBuffer = ArquivoInformacao;
	Irp->Tail.Overlay.Thread = PsGetCurrentThread();
	Irp->Tail.Overlay.OriginalFileObject = ObjetoPasta;
	Irp->Overlay.AsynchronousParameters.UserApcRoutine = (PIO_APC_ROUTINE)NULL;

	// Configure o IrpSp
	IrpSp = IoGetNextIrpStackLocation(Irp);
	IrpSp->MajorFunction = IRP_MJ_DIRECTORY_CONTROL;
	IrpSp->MinorFunction = IRP_MN_QUERY_DIRECTORY;
	IrpSp->FileObject = ObjetoPasta;
	IrpSp->Flags = SL_RESTART_SCAN;
	IrpSp->Parameters.QueryDirectory.Length = Tamanho;
	IrpSp->Parameters.QueryDirectory.FileName = Arquivo;
	IrpSp->Parameters.QueryDirectory.FileInformationClass = InformacaoClasse;

	// Complete
	IoSetCompletionRoutine(Irp, CompletarAtributo, 0, TRUE, TRUE, TRUE);

	// Faça o evento ocorrer
	Status = IoCallDriver(ObjetoPasta->Vpb->DeviceObject, Irp);

	// Se estiver pendente
	if (Status == STATUS_PENDING)
		KeWaitForSingleObject(&kEvent, Executive, KernelMode, TRUE, 0);

	// Retorne o status
	return Io->Status;
}

/// <summary>
/// Limpa os atributos
/// </summary>
/// 
/// <param name="FileHandle">FILE_OBJECT do arquivo</param>
/// 
/// <returns>Retorna um BOOLEAN</returns>
BOOLEAN IrpLimparAtributos(_In_ PFILE_OBJECT ObjetoArquivo)
{
	// Arquivo
	NTSTATUS Status = STATUS_SUCCESS;

	// Device
	PDEVICE_OBJECT DispositivoObjetivo, BaseDispositivo;
	PIRP Irp;

	// Evento
	KEVENT evento;
	FILE_BASIC_INFORMATION Informacao;
	IO_STATUS_BLOCK ioStatus;
	PIO_STACK_LOCATION irpSp;

	// Obtenha o dsispotivo
	DispositivoObjetivo = IoGetRelatedDeviceObject(ObjetoArquivo);

	BaseDispositivo = IoGetDeviceAttachmentBaseRef(DispositivoObjetivo);

	Irp = IoAllocateIrp(BaseDispositivo->StackSize, TRUE);

	if (Irp == NULL)
	{
		return FALSE;
	}

	// Inicie o evento
	KeInitializeEvent(&evento, SynchronizationEvent, FALSE);

	// Copie as informações
	memset(&Informacao, 0, sizeof(FILE_BASIC_INFORMATION));

	// Arquivo normal
	Informacao.FileAttributes = FILE_ATTRIBUTE_NORMAL;
	Irp->AssociatedIrp.SystemBuffer = &Informacao;
	Irp->UserEvent = &evento;
	Irp->UserIosb = &ioStatus;
	Irp->Tail.Overlay.OriginalFileObject = ObjetoArquivo;
	Irp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
	Irp->RequestorMode = KernelMode;

	// Configure o Irp
	irpSp = IoGetNextIrpStackLocation(Irp);
	irpSp->MajorFunction = IRP_MJ_SET_INFORMATION;
	irpSp->DeviceObject = BaseDispositivo;
	irpSp->FileObject = ObjetoArquivo;
	irpSp->Parameters.SetFile.ReplaceIfExists = TRUE;
	irpSp->Parameters.SetFile.Length = sizeof(FILE_BASIC_INFORMATION);
	irpSp->Parameters.SetFile.FileInformationClass = FileBasicInformation;
	irpSp->Parameters.SetFile.FileObject = ObjetoArquivo;

	// Complete a rotina
	IoSetCompletionRoutine(
		Irp,
		CompletarAtributo,
		&evento,
		TRUE,
		TRUE,
		TRUE);

	// Chame o IRP
	IoCallDriver(BaseDispositivo, Irp);

	// Espere
	KeWaitForSingleObject(&evento, Executive, KernelMode, TRUE, NULL);

	return TRUE;
}


#include "deleteFile.c"
