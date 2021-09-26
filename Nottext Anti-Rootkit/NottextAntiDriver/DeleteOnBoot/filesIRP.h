
#define MAX_PATH          600 
/// https://bbs.pediy.com/thread-264070.htm
/// https://titanwolf.org/Network/Articles/Article?AID=4520fea9-1486-4905-8dfc-5bbb14fd5db3#gsc.tab=0

/*

	Operações para forçar a remoção de um arquivo

*/

/// <summary>
/// Abre um objeto apartir de um ponteiro
/// </summary>
/// <param name="Object"></param>
/// <param name="HandleAttributes"></param>
/// <param name="PassedAccessState"></param>
/// <param name="DesiredAccess"></param>
/// <param name="ObjectType"></param>
/// <param name="AccessMode"></param>
/// <param name="Handle"></param>
/// <returns></returns>
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
/// <param name="ProbeMode"></param>
/// <param name="ObjectType"></param>
/// <param name="ObjectAttributes"></param>
/// <param name="OwnershipMode"></param>
/// <param name="ParseContext"></param>
/// <param name="ObjectBodySize"></param>
/// <param name="PagedPoolCharge"></param>
/// <param name="NonPagedPoolCharge"></param>
/// <param name="Object"></param>
/// <returns></returns>
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
/// <param name="AccessState"></param>
/// <param name="AuxData"></param>
/// <param name="DesiredAccess"></param>
/// <param name="GenericMapping"></param>
/// <returns></returns>
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

/// <summary>
/// Obtém o dispositivo
/// </summary>
/// <param name="NomeDriver"></param>
/// <param name="ObjetoDispositivo"></param>
/// <param name="LerDispositivo"></param>
/// <returns></returns>
NTSTATUS ObterDriverDispositivo(_In_ PUNICODE_STRING NomeDriver, _Out_ PDEVICE_OBJECT* ObjetoDispositivo, _Out_ PDEVICE_OBJECT* LerDispositivo);

/// <summary>
/// Obtém o dispositivo
/// </summary>
/// <param name="NomeDriver"></param>
/// <param name="ObjetoDispositivo"></param>
/// <param name="LerDispositivo"></param>
/// <returns></returns>
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
/// <param name="Arquivo"></param>
/// <param name="Acesso"></param>
/// <param name="Io"></param>
/// <param name="ObjetoArquivo"></param>
/// <returns></returns>
NTSTATUS AbrirArquivoIRP(_In_ UNICODE_STRING Arquivo, _In_ ACCESS_MASK Acesso, _In_ PIO_STATUS_BLOCK Io, _Out_ PFILE_OBJECT* ObjetoArquivo)
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
	IrpSp->Parameters.Create.Options = (FILE_OPEN_IF << 24) | 0;
	IrpSp->Parameters.Create.FileAttributes = FILE_ATTRIBUTE_NORMAL;
	IrpSp->Parameters.Create.ShareAccess = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
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
/// Deleta um arquivo por FILE_OBJECT
/// </summary>
/// <param name="ObjetoArquivo"></param>
/// <param name="ePasta"></param>
/// <returns></returns>
NTSTATUS DeletarObjetoArquivo(_Out_ PFILE_OBJECT ObjetoArquivo, _In_ BOOLEAN ePasta)
{
	PDEVICE_OBJECT ObjetoDispositivo, ObjetoBase;
	PIRP Irp;
	KEVENT Evento;
	FILE_DISPOSITION_INFORMATION Informacao;
	IO_STATUS_BLOCK ioStatus;
	PIO_STACK_LOCATION irpSp;
	PSECTION_OBJECT_POINTERS ObjetoPonteiro;

	ObjetoDispositivo = IoGetRelatedDeviceObject(ObjetoArquivo);

	// Obtenha a base
	ObjetoBase = IoGetDeviceAttachmentBaseRef(ObjetoDispositivo);

	// Aloque o IRP
	Irp = IoAllocateIrp(ObjetoBase->StackSize, TRUE);

	// Se falhar
	if (Irp == NULL) {
		return STATUS_UNSUCCESSFUL;
	}

	// Inicie o evento
	KeInitializeEvent(&Evento, SynchronizationEvent, FALSE);

	// Delete
	Informacao.DeleteFile = TRUE;

	// IRP
	Irp->AssociatedIrp.SystemBuffer = &Informacao;
	Irp->UserEvent = &Evento;
	Irp->UserIosb = &ioStatus;
	Irp->Tail.Overlay.OriginalFileObject = ObjetoArquivo;
	Irp->Tail.Overlay.Thread = (PETHREAD)KeGetCurrentThread();
	Irp->RequestorMode = KernelMode;

	// Configure
	irpSp = IoGetNextIrpStackLocation(Irp);
	irpSp->MajorFunction = IRP_MJ_SET_INFORMATION;
	irpSp->DeviceObject = ObjetoBase;
	irpSp->FileObject = ObjetoArquivo;
	irpSp->Parameters.SetFile.Length = sizeof(FILE_DISPOSITION_INFORMATION);
	irpSp->Parameters.SetFile.FileInformationClass = FileDispositionInformation;
	irpSp->Parameters.SetFile.FileObject = ObjetoArquivo;

	// Complete a rotina
	IoSetCompletionRoutine(
		Irp,
		CompletarAtributo,
		&Evento,
		TRUE,
		TRUE,
		TRUE);

	// Se não for pasta
	if (ePasta == FALSE)
	{
		// Nenhum processo
		ObjetoArquivo->SectionObjectPointer->ImageSectionObject = 0;
		ObjetoArquivo->DeleteAccess = 1;

		// Limpe as sesões
		ObjetoPonteiro = ObjetoArquivo->SectionObjectPointer;
		ObjetoPonteiro->ImageSectionObject = 0;
		ObjetoPonteiro->DataSectionObject = 0;

		// A MmFlushImageSection libera a seção da imagem para um arquivo. 
		CONST BOOLEAN ImageSectionFlushed = MmFlushImageSection(
			ObjetoPonteiro,
			MmFlushForDelete //  O arquivo está sendo excluído. 
		);
	}

	// Chame o IRP
	NTSTATUS Status = IoCallDriver(ObjetoBase, Irp);

	// Espere
	KeWaitForSingleObject(&Evento, Executive, KernelMode, TRUE, NULL);

	return Status;
}

/// <summary>
/// Cria um arquivo com IRP
/// </summary>
/// <param name="Arquivo"></param>
/// <param name="Acesso"></param>
/// <returns></returns>
NTSTATUS CriarArquivoIRP(_In_ PUNICODE_STRING Arquivo, _In_ ACCESS_MASK Acesso, _In_ PIO_STATUS_BLOCK Io, _In_ PFILE_OBJECT* ObjetoArquvo)
{
	// Status
	NTSTATUS Status;
	PIRP Irp;

	// Variáveis
	KEVENT Evento;
	ACCESS_STATE EstadoAcesso;
	AUX_ACCESS_DATA AuxData;
	OBJECT_ATTRIBUTES Atributos;
	PFILE_OBJECT NovoObjetoArquivo;

	IO_SECURITY_CONTEXT ContextoSeguranca;
	PIO_STACK_LOCATION IrpSp;
	PDEVICE_OBJECT DispositivoObjeto;
	PDEVICE_OBJECT DispositivoLer;

	UNICODE_STRING NomeDriver;
	wchar_t* NomeArquivoBuf = NULL;
	wchar_t LocalArquivo[MAX_PATH];

	// Se algo estiver errado
	if (Arquivo == NULL || Io == NULL || ObjetoArquvo == NULL || Arquivo->Length <= SYMBOLICLINKLENG)
	{
		return STATUS_INVALID_PARAMETER;
	}

	// Zere
	RtlZeroMemory(LocalArquivo, sizeof(LocalArquivo));

	// Copie o Arquivo->Buffer para o LocalArquivo, com limite de 7 * sizeof(wchar_t)
	RtlCopyMemory(LocalArquivo, Arquivo->Buffer, (SYMBOLICLINKLENG + 1) * sizeof(wchar_t));

	// Inicie o UNICODE com o LocalArquivo
	RtlInitUnicodeString(&NomeDriver, LocalArquivo);

	// Obtenha o dispositivo (se é o C:\\ ou outro driver)
	Status = ObterDriverDispositivo(&NomeDriver, &DispositivoObjeto, &DispositivoLer);

	if (!NT_SUCCESS(Status))
	{
		return Status;
	}
}

/*






*/
/// <summary>
/// Deleta o arquivo, enviando um IRP direto para drivers de baixo nível
/// Um nível mais simples de remoção
/// </summary>
NTSTATUS DeletarArquivoNormal(_In_ UNICODE_STRING ArquivoUnicode)
{
	// Atributos
	OBJECT_ATTRIBUTES Atributos;

	// Objeto
	PFILE_OBJECT ObjetoArquivo = NULL;

	// Dispositivo
	PDEVICE_OBJECT ObjetoDispositivo;

	// Alca
	HANDLE Alca = NULL;

	// Status
	NTSTATUS Status;

	// IRP
	PIRP Irp;

	// CurrentLocation
	PIO_STACK_LOCATION CurrentLocation;

	// Evento
	KEVENT Evento;
	IO_STATUS_BLOCK	IoBlock;

	// Valor desconhecido
	ULONG ValorDesconhecido = 1;

	// Inicie os atributos
	InitializeObjectAttributes(
		&Atributos,
		&ArquivoUnicode,
		OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
		NULL,
		NULL
	);

	/*

		A IoCreateFileSpecifyDeviceObjectHint rotina é usada por drivers de filtro do
		sistema de arquivos para enviar uma solicitação de criação apenas para os
		filtros abaixo de um objeto de dispositivo especificado e para o sistema de arquivos.

	*/

	Status = IoCreateFileSpecifyDeviceObjectHint(

		// Salve a alça aqui, para fechar o arquivo depois
		&Alca,

		// Permissões que vamos usar ao abrir o arquivo
		GENERIC_READ | GENERIC_WRITE,

		// Os atributos que inicializamos
		&Atributos,
		&IoBlock,
		NULL,

		// Normal
		FILE_ATTRIBUTE_NORMAL,

		// Compartilhe as permissões de deletar
		FILE_SHARE_DELETE,

		// Não é uma pasta
		FILE_OPEN,
		FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
		NULL,
		NULL,

		// Não sei pra que serve
		CreateFileTypeNone,
		NULL,

		// Ignore os atributos
		IO_IGNORE_SHARE_ACCESS_CHECK | IO_IGNORE_READONLY_ATTRIBUTE,
		NULL
	);

	// Se falhar ao abrir o arquivo
	if (!NT_SUCCESS(Status))
	{
		// Tenta abrir o arquivo com permissão de leitura
		Status = ZwOpenFile(
			&Alca,
			GENERIC_READ,
			&Atributos,
			&IoBlock,
			FILE_SHARE_VALID_FLAGS,
			FILE_SYNCHRONOUS_IO_NONALERT | FILE_NON_DIRECTORY_FILE
		);

		// Se falhar
		if (!NT_SUCCESS(Status))
		{
			// Tenta abrir o arquivo com permissão de leitura
			Status = IoCreateFileSpecifyDeviceObjectHint(
				&Alca,
				FILE_LIST_DIRECTORY,
				&Atributos,
				&IoBlock,
				NULL,
				FILE_ATTRIBUTE_NORMAL,
				0,
				FILE_OPEN,
				FILE_DIRECTORY_FILE,
				NULL,
				NULL,
				CreateFileTypeNone,
				NULL,
				IO_IGNORE_SHARE_ACCESS_CHECK | IO_IGNORE_READONLY_ATTRIBUTE,
				NULL
			);

			// Se conseguir
			if (NT_SUCCESS(Status))
			{
				//ZwClose(FileHandle);
				ZwDeleteFile(&Atributos);
				goto exit;
			}

			// Saia
			goto exit;
		}
	}

	// Obtenha o objeto do arquivo
	Status = ObReferenceObjectByHandle(

		// Indentificador do arquivo (alça)
		Alca,

		// Deletar
		DELETE,
		*IoFileObjectType,
		0,

		// Object
		(PVOID*)&ObjetoArquivo,
		0
	);

	// Se falhar
	if (!NT_SUCCESS(Status))
	{
		//DbgPrint("Falha ao obter o objeto do arquivo: %x\n", Status);

		// Saia
		goto exit;
	}

	// Altere o valor para permitir deletar arquivos em uso
	ObjetoArquivo->SectionObjectPointer->ImageSectionObject = 0;

	// Acesso de deletar
	ObjetoArquivo->DeleteAccess = 1;

	// Acesso de escrita
	ObjetoArquivo->WriteAccess = 1;

	// Obtenha o objeto de dispositivo
	ObjetoDispositivo = IoGetBaseFileSystemDeviceObject(ObjetoArquivo);

	// Se falhar
	if (ObjetoDispositivo == NULL)
	{
		// Saia
		goto exit;
	}

	// A IoAllocateIrp rotina não associa o IRP a um thread.
	// O driver de alocação deve liberar o IRP em vez de completá-lo de volta
	// para o gerenciador de E / S. 
	Irp = IoAllocateIrp(ObjetoDispositivo->StackSize, 1);

	// Se falhar
	if (Irp == NULL)
	{
		// Saia
		goto exit;
	}

	// Inicia o evento
	KeInitializeEvent(&Evento, SynchronizationEvent, 0);

	// Altere os valores de IRP
	// Ponteiro para um buffer de espaço do sistema. 
	Irp->AssociatedIrp.SystemBuffer = &ValorDesconhecido;
	Irp->UserEvent = &Evento;
	Irp->UserIosb = &IoBlock;
	Irp->Tail.Overlay.OriginalFileObject = ObjetoArquivo;
	Irp->Tail.Overlay.Thread = KeGetCurrentThread();

	// CurrentLocation
	CurrentLocation = Irp->Tail.Overlay.CurrentStackLocation;

	// Request de KernelMode
	Irp->RequestorMode = KernelMode;
	--CurrentLocation;

	// Alte os valores
	CurrentLocation->MajorFunction = IRP_MJ_SET_INFORMATION;
	CurrentLocation->DeviceObject = ObjetoDispositivo;
	CurrentLocation->FileObject = ObjetoArquivo;
	CurrentLocation->Parameters.SetFile.FileInformationClass = FileDispositionInformation;
	CurrentLocation->Parameters.SetFile.FileObject = ObjetoArquivo;
	CurrentLocation->Parameters.SetFile.Length = sizeof(FILE_DISPOSITION_INFORMATION);

	// Complete a função
	CurrentLocation->CompletionRoutine = CompletarAtributo;
	CurrentLocation->Context = &Evento;
	CurrentLocation->Control = 0xE0;

	// A IoCallDriver rotina envolve IofCallDriver que envia um IRP ao driver
	// associado a um objeto de dispositivo especificado.
	// Para obter mais informações, consulte IofCallDriver. 
	Status = IoCallDriver(ObjetoDispositivo, Irp);

	// Se falhar
	if (!NT_SUCCESS(Status))
	{
		// Saia
		goto exit;
	}

	// Espera a operação
	KeWaitForSingleObject(&Evento, 0, KernelMode, FALSE, NULL);

	// Status
	//KdPrint(("Irp.status is %x\n", Irp->IoStatus.Status));

	// Saia
	goto exit;

	// Função que sai
exit:

	// Se o FileObject foi usado
	if (ObjetoArquivo != NULL)
	{
		// Libere-o
		ObfDereferenceObject(ObjetoArquivo);
	}

	// Se o arquivo estiver aberto
	if (Alca != NULL)
	{
		// Feche-o
		ZwClose(Alca);
	}

	// Retorne o status
	return Status;
}

/// <summary>
/// Lê uma linha
/// </summary>
/// <param name="Tamanho"></param>
/// <param name="Linha"></param>
VOID LerLinha(
	SIZE_T Tamanho,
	PCSTR Linha
)
{
	__try {

		CHAR Arquivo[720] = { 0 };

		// Mova o valor
		RtlMoveMemory(Arquivo, Linha, Tamanho);

		// Se falhar ao deletar no modo arquivo
		if (ListarPasta(Arquivo, TRUE, TRUE) != STATUS_SUCCESS)
		{
			// Delete
			DeletarArquivo(Arquivo, FALSE);
		}

		RtlZeroMemory(Arquivo, sizeof(CHAR));

	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
	}
}

/// <summary>
/// Acha linhas em um texto
/// </summary>
/// <param name="tb"></param>
/// <param name="sz"></param>
/// <returns></returns>
PCSTR ProcurarLinha(
	SIZE_T tb,
	PCSTR sz
)
{
	// Se for menor de 2
	if (tb < 2)
	{
		return 0;
	}

	tb--;

	do
	{
		if (*sz++ == '\r')
		{
			if (*sz == '\n')
			{
				// Retorne a string - 1
				return sz - 1;
			}
		}
	} while (--tb);

	// Retorne falso
	return 0;
}

/// <summary>
/// Obtem as informações corretas para encontrar as linhas
/// </summary>
/// <param name="buf"></param>
/// <param name="tb"></param>
/// <returns></returns>
NTSTATUS ProcessarArquivo(PCSTR buf, SIZE_T tb)
{
	// Junte tudo
	PCSTR Final = buf + tb, linha = buf;

	// While, para encontrar todas as linhas
	while (buf = ProcurarLinha(Final - linha, linha))
	{
		// Leia a linha
		LerLinha(buf - linha, linha);

		// Vamos configurar a linha
		linha = buf + 2;
	}

	// Se a linha for diferente do final
	if (linha != Final)
	{
		// Leia a linha
		LerLinha(
			Final - linha, // Tamanho do buffer
			linha // Linha
		);
	}

	// Terminamos
	return 0;
}

/// <summary>
/// Processa um arquivo, para que seja possível ler o texto dele sem retornar lixo
/// </summary>
/// <param name="Atributos"></param>
/// <returns></returns>
NTSTATUS ProcessarInformacoesArquivo(POBJECT_ATTRIBUTES Atributos)
{
	// Alça
	HANDLE Alca, Sessao = 0;

	// Status de bloqueio
	IO_STATUS_BLOCK iosb;

	// Abra o arquivo
	NTSTATUS Status = ZwOpenFile(&Alca, FILE_GENERIC_READ, Atributos, &iosb, FILE_SHARE_VALID_FLAGS | FILE_SHARE_DELETE, FILE_SYNCHRONOUS_IO_NONALERT);

	// Se conseguir abrir o arquivo
	if (NT_SUCCESS(Status))
	{
		// Informações do arquivo
		FILE_STANDARD_INFORMATION Informacao;

		// Obtenha as informações do arquivo
		Status = ZwQueryInformationFile(Alca, &iosb, &Informacao, sizeof(Informacao), FileStandardInformation);

		// Se conseguir
		if (NT_SUCCESS(Status))
		{
			// Se o final do arquvio for muito grande
			if (Informacao.EndOfFile.HighPart)
			{
				Status = STATUS_FILE_TOO_LARGE;
			}

			// Se não
			else if (!Informacao.EndOfFile.LowPart)
			{
				Status = STATUS_MAPPED_FILE_SIZE_ZERO;
			}

			// Se tudo ocorrer ok
			else
			{
				// Crie uma sessão
				Status = ZwCreateSection(
					&Sessao,

					// Mapeie para leitura
					SECTION_MAP_READ,
					0,
					0,

					// Somente leitura
					PAGE_READONLY,

					// Commit
					SEC_COMMIT,
					Alca
				);
			}
		}

		// Se conseguir
		if (NT_SUCCESS(Status))
		{
			// Informações
			PVOID EnderecoBase = 0;
			SIZE_T VerTamanho = 0;

			// Mapeie a sessão
			Status = ZwMapViewOfSection(
				// Sessão
				Sessao,

				// Processo atual
				ZwCurrentProcess(),

				// Endereço
				&EnderecoBase,
				0,
				0,
				0,

				// Tamanho
				&VerTamanho,
				ViewUnmap,
				0,

				// Somente leitura
				PAGE_READONLY
			);

			// Feche a sessão
			ZwClose(Sessao);

			// Se obter sucesso
			if (NT_SUCCESS(Status))
			{
				// Processe o arquivo novamente, até falhar ou terminar
				Status = ProcessarInformacoesArquivo((PCSTR)EnderecoBase, Informacao.EndOfFile.LowPart);

				// Desmapeie o arquivo
				ZwUnmapViewOfSection(ZwCurrentProcess(), EnderecoBase);
			}
		}

		// Se a alça ainda estiver aberta
		if (Alca != NULL)
		{
			ZwClose(Alca);
		}
	}

	// Retorne o status
	return Status;
}

/// <summary>
/// Deleta arquivos durante a inicialização do BOOT
/// </summary>
/// <returns></returns>
NTSTATUS DeletarNoBoot()
{
	// Váriaveis
	HANDLE Alca;
	OBJECT_ATTRIBUTES Atributos;
	IO_STATUS_BLOCK IoR;
	IO_STATUS_BLOCK Io;

	UNICODE_STRING UnArquivo;
	RtlInitUnicodeString(&UnArquivo, L"\\??\\C:\\ProgramData\\NtAnti-Rootkit\\deleteOnBoot.txt");

	// Inicie o atributo
	InitializeObjectAttributes(&Atributos, &UnArquivo, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	// Abra o arquivo
	NTSTATUS Status = STATUS_SUCCESS;

	PUCHAR buffer = NULL;
	ULONG Tamanho = 4096;
	LARGE_INTEGER Offset = { 0 };

	// PagedPool = memória do kernel e do driver de dispositivo que PODE transbordar da memória física para o arquivo de página lento 
	// NonPagedPool = Pool não paginado é a quantidade de memória do kernel e do driver de dispositivo que deve permanecer na memória física.
	buffer = (PUCHAR)ExAllocatePoolWithTag(PagedPool, Tamanho, 'file');

	// Se conseguir
	if (buffer)
	{
		// Abra o arquivo
		Status = ZwOpenFile(
			&Alca,
			GENERIC_READ,
			&Atributos,
			&IoR,
			FILE_SHARE_READ,
			FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT
		);

		if (NT_SUCCESS(Status))
		{
			ProcessarInformacoesArquivo(&Atributos);

			// Repetição infinita
			while (1)
			{
				// Zere o tamanho do buffer
				// Tamanho
				Tamanho = 4096;

				// Leia o arquivo original, que vamos copiar
				Status = ZwReadFile(Alca, NULL, NULL, NULL, &Io, buffer, Tamanho, &Offset, NULL);

				// Se falhar
				if (!NT_SUCCESS(Status))
				{
					// Se for o fim do arquivo
					if (Status == STATUS_END_OF_FILE)
					{
						Status = STATUS_SUCCESS;
						break;
					}
					else
					{
						// Saia
						break;
					}
				}

				// Tamanhho
				Tamanho = Io.Information;
				Offset.QuadPart += Tamanho;

				size_t tb;

				// Vamos converter o BUFFER
				Status = RtlStringCbLengthA(
					buffer, // Buffer
					wcslen(buffer) * sizeof(PUCHAR), // Tamanho do buffer
					&tb // Vamos usar aqui
				);

				if (NT_SUCCESS(Status))
				{
					// Processe o arquivo
					ProcessarArquivo(buffer, tb);
				}
			}
		}
	}

	goto sair;

sair:

	if (buffer != NULL)
	{
		ExFreePoolWithTag(buffer, 'file');
	}


	if (Alca != NULL)
	{
		ZwClose(Alca);
		ZwDeleteFile(&Atributos);
	}

	return Status;
}

