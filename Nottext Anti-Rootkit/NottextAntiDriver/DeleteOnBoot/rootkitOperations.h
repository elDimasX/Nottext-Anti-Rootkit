
/*
#define MY_TAG 'file'
#define MAX_PATH 260

typedef struct _FIND_FILE_HANDLE_INFO {
    PVOID pDeviceObject; //卷设备(内部用)
    PVOID pFileObject;   //目录文件对象(内部用)
}FIND_FILE_HANDLE_INFO, * PFIND_FILE_HANDLE_INFO, * LPFIND_FILE_HANDLE_INFO;

typedef struct _FIND_FILE_OUTPUT {
    LARGE_INTEGER CreationTime;  //创建时间
    LARGE_INTEGER LastAccessTime; //最近访问时间
    LARGE_INTEGER LastWriteTime;  //最近写入时间
    LARGE_INTEGER ChangeTime;     //变更时间
    LARGE_INTEGER EndOfFile;      //文件大小
    LARGE_INTEGER AllocationSize; //占用空间大小
    ULONG    ulFileAttributes;  //属性
    WCHAR    wShortFileName[14]; //8.3 格式名
    WCHAR    wFileName[MAX_PATH]; //名称
} FIND_FILE_OUTPUT, * PFIND_FILE_OUTPUT, * LPFIND_FILE_OUTPUT;

typedef struct _FIND_FIRST_FILE_OUTPUT {
    FIND_FILE_HANDLE_INFO stFileFileHandleInfo; //被查询的目录打开信息
    FIND_FILE_OUTPUT stFindFileItem; //发现项信息
}FIND_FIRST_FILE_OUTPUT, * PFIND_FIRST_FILE_OUTPUT, * LPFIND_FIRST_FILE_OUTPUT;

//自己发送IRP请求查询目录下的1项封装
NTSTATUS RootkitFindFileItem(
    IN PDEVICE_OBJECT pDeviceObject,
    IN PFILE_OBJECT pFileObject,
    IN BOOLEAN bRestartScan,
    OUT PFIND_FILE_OUTPUT pFindFileOut
)
{
    PFILE_BOTH_DIR_INFORMATION pFileBothDirInfo;
    IO_STATUS_BLOCK iosb;
    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL;

    PAGED_CODE();
    ASSERT(pDeviceObject != NULL && pFileObject != NULL && pFindFileOut != NULL);

    //分配缓冲区
    pFileBothDirInfo = (PFILE_BOTH_DIR_INFORMATION)ExAllocatePoolWithTag(PagedPool,
        PAGE_SIZE,
        MY_TAG);
    if (pFileBothDirInfo == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    RtlZeroMemory(pFileBothDirInfo,
        PAGE_SIZE);

    //发送IRP请求查询目录下的1项
    ntStatus = IrpQueryDirectoryFile(pDeviceObject,
        pFileObject,
        &iosb,
        (PVOID)pFileBothDirInfo,
        PAGE_SIZE,
        FileBothDirectoryInformation,
        TRUE,
        NULL,
        bRestartScan);
    if (NT_SUCCESS(ntStatus)) {
        //成功了就进行保存
        RtlZeroMemory(pFindFileOut,
            sizeof(FIND_FILE_OUTPUT));
        pFindFileOut->CreationTime.QuadPart = pFileBothDirInfo->CreationTime.QuadPart;
        pFindFileOut->LastAccessTime.QuadPart = pFileBothDirInfo->LastAccessTime.QuadPart;
        pFindFileOut->LastWriteTime.QuadPart = pFileBothDirInfo->LastWriteTime.QuadPart;
        pFindFileOut->ChangeTime.QuadPart = pFileBothDirInfo->ChangeTime.QuadPart;
        pFindFileOut->EndOfFile.QuadPart = pFileBothDirInfo->EndOfFile.QuadPart;
        pFindFileOut->AllocationSize.QuadPart = pFileBothDirInfo->AllocationSize.QuadPart;
        pFindFileOut->ulFileAttributes = pFileBothDirInfo->FileAttributes;
        if (pFileBothDirInfo->ShortNameLength > 0) {
            RtlCopyMemory(pFindFileOut->wShortFileName,
                pFileBothDirInfo->ShortName,
                (SIZE_T)pFileBothDirInfo->ShortNameLength);
            pFindFileOut->wShortFileName[sizeof(pFindFileOut->wShortFileName) / sizeof(WCHAR) - 1] = L'\0';
        }
        if (pFileBothDirInfo->FileNameLength > 0) {
            if (pFileBothDirInfo->FileNameLength > sizeof(pFindFileOut->wFileName)) {
                pFileBothDirInfo->FileNameLength = sizeof(pFindFileOut->wFileName);
            }
            RtlCopyMemory(pFindFileOut->wFileName,
                pFileBothDirInfo->FileName,
                (SIZE_T)pFileBothDirInfo->FileNameLength);
            pFindFileOut->wFileName[sizeof(pFindFileOut->wFileName) / sizeof(WCHAR) - 1] = L'\0';
        }
    }

    //释放缓冲区
    ExFreePoolWithTag((PVOID)pFileBothDirInfo,
        MY_TAG);

    return ntStatus;
}

//第1次查询目录下的项(设备控制请求中调用)
NTSTATUS RootkitFindFirstFileForIoctl(
    IN LPCWSTR wDirPath,
    OUT PFIND_FIRST_FILE_OUTPUT pFindFirstFileOutput
)
{
    PDEVICE_OBJECT pDeviceObject = NULL;
    PFILE_OBJECT pFileObject = NULL;
    NTSTATUS ntStatus = STATUS_UNSUCCESSFUL, ntLocStatus;
    UNICODE_STRING unsDirPath;
    IO_STATUS_BLOCK iosb;

    PAGED_CODE();
    ASSERT(wDirPath != NULL && pFindFirstFileOutput != NULL);

    do {
        //检查目录路径是否以"\??\"打头
        RtlInitUnicodeString(&unsDirPath,
            wDirPath);
        if (unsDirPath.Length >= 4 * sizeof(WCHAR)) {
            if (unsDirPath.Buffer[0] == L'\\'
                && unsDirPath.Buffer[1] == L'?'
                && unsDirPath.Buffer[2] == L'?'
                && unsDirPath.Buffer[3] == L'\\') {
                //如果以"\??\"打头，传入IrpCreateFile前需要去掉头部"\??\"
                unsDirPath.Buffer += 4;
                unsDirPath.Length -= 4 * sizeof(WCHAR);
                unsDirPath.MaximumLength -= 4 * sizeof(WCHAR);
            }
        }

        //自己发送IRP请求,打开目标目录
        ntStatus = IrpCreateFile(&pFileObject,
            &pDeviceObject,
            FILE_LIST_DIRECTORY | FILE_TRAVERSE | SYNCHRONIZE,
            &unsDirPath,
            &iosb,
            NULL,
            FILE_ATTRIBUTE_NORMAL,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            FILE_OPEN,
            FILE_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT,
            NULL,
            0);
        if (!NT_SUCCESS(ntStatus)) {
            break;
        }

        //自己发送IRP请求,查询目录下的第1条
        ntStatus = RootkitFindFileItem(pDeviceObject,
            pFileObject,
            TRUE,
            &pFindFirstFileOutput->stFindFileItem);
        if (NT_SUCCESS(ntStatus)) {
            pFindFirstFileOutput->stFileFileHandleInfo.pDeviceObject = (PVOID)pDeviceObject;
            pFindFirstFileOutput->stFileFileHandleInfo.pFileObject = (PVOID)pFileObject;
        }
    } while (FALSE);

    if (!NT_SUCCESS(ntStatus)) {
        //失败时的处理
        if (pFileObject != NULL) {
            ASSERT(pDeviceObject != NULL);

            //自己发送IRP请求关闭查询目录
            ntLocStatus = IrpCleanupFile(pDeviceObject,
                pFileObject);
            ASSERT(NT_SUCCESS(ntLocStatus));

            ntLocStatus = IrpCloseFile(pDeviceObject,
                pFileObject);
            ASSERT(NT_SUCCESS(ntLocStatus));
        }
    }

    return ntStatus;
}

*/