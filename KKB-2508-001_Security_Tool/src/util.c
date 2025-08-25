#include "pch.h"
#include "util.h"

NTSTATUS _OpenFile(
    PUNICODE_STRING filePath,
    PHANDLE fileHandle,
    ACCESS_MASK access,
    ULONG shareAccess,
    ULONG createDisposition
)
{
    OBJECT_ATTRIBUTES objAttr;
    IO_STATUS_BLOCK ioStatus;

    InitializeObjectAttributes(
        &objAttr,
        filePath,
        OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE,
        NULL,
        NULL
    );

    return ZwCreateFile(
        fileHandle,
        access,
        &objAttr,
        &ioStatus,
        NULL,
        FILE_ATTRIBUTE_NORMAL,
        shareAccess,
        createDisposition,
        FILE_SYNCHRONOUS_IO_NONALERT,
        NULL, 0
    );
}

NTSTATUS _CopyFile(HANDLE src, HANDLE dst)
{
    NTSTATUS status;
    IO_STATUS_BLOCK ioStatus;
    UCHAR buffer[4096];
    LARGE_INTEGER offset = { 0 };

    while (TRUE)
    {
        status = ZwReadFile(src, NULL, NULL, NULL, &ioStatus, buffer, sizeof(buffer), &offset, NULL);
        if (status == STATUS_END_OF_FILE) break;
        if (!NT_SUCCESS(status)) return status;

        status = ZwWriteFile(dst, NULL, NULL, NULL, &ioStatus, buffer, (ULONG)ioStatus.Information, &offset, NULL);
        if (!NT_SUCCESS(status)) return status;

        offset.QuadPart += ioStatus.Information;
    }
    return STATUS_SUCCESS;
}


VOID _UpperPWCH(PWCH str)
{
    if (!str) return;
    while (*str != L'\0')
    {
        *str = RtlUpcaseUnicodeChar(*str); 
        str++;
    }
}

VOID _TerminateProcess(HANDLE pid) {
    HANDLE hProcess = NULL;
    OBJECT_ATTRIBUTES objAttr;
    CLIENT_ID clientId;

    clientId.UniqueProcess = pid;
    clientId.UniqueThread = NULL;
    InitializeObjectAttributes(&objAttr, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);

    NTSTATUS status = ZwOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &objAttr, &clientId);
    if (!NT_SUCCESS(status)) {
        return;
    }
    status = ZwTerminateProcess(hProcess, STATUS_ACCESS_DENIED);
    ZwClose(hProcess);
    if (!NT_SUCCESS(status)) {
        return;
    }
    return;
}

VOID _GetFileExt(WCHAR* Path, ULONG PathLen, PUNICODE_STRING OriExt) {
    for (LONG i = PathLen - 1; i >= 0; i--) {
        if (Path[i] == L'.') {
            OriExt->Buffer = &Path[i+1];
            OriExt->Length = (USHORT)((PathLen - i -1) * sizeof(WCHAR));
            OriExt->MaximumLength = OriExt->Length;
            break;
        }
    }
}