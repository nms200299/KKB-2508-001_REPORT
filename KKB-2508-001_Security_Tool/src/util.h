#pragma once
NTSTATUS _CopyFile(HANDLE src, HANDLE dst);

NTSTATUS _OpenFile(
    PUNICODE_STRING filePath,
    PHANDLE fileHandle,
    ACCESS_MASK access,
    ULONG shareAccess,
    ULONG createDisposition
);

VOID _UpperPWCH(PWCH str);

VOID _TerminateProcess(HANDLE pid);

VOID _GetFileExt(WCHAR* Path, ULONG PathLen, PUNICODE_STRING OriExt);