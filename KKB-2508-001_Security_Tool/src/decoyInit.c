#include "pch.h"
#include "decoyInit.h"

WCHAR* DecoyFileList[] = {
    L"test.avi",
    L"test.doc",
    L"test.docx",
    L"test.hwp",
    L"test.jpeg",
    L"test.jpg",
    L"test.mp4",
    L"test.pdf",
    L"test.png",
    L"test.ppt",
    L"test.pptx",
    L"test.txt",
    L"test.xls",
    L"test.xlsx",
    L"test.zip"
};

CONST size_t DecoyFileCount = sizeof(DecoyFileList) / sizeof(DecoyFileList[0]);

VOID decoyInit(PUNICODE_STRING DstDecoyPath) {
    if ((DstDecoyPath == NULL) || (DstDecoyPath->Length == 0)) {
        return;
    }

    DbgPrint("[RswDetect] Decoy Init\n");

    for (size_t i = 0; i < DecoyFileCount; i++)
    {
        UNICODE_STRING srcPath, dstPath;
        WCHAR srcBuffer[256];
        WCHAR dstBuffer[256];
        RtlZeroMemory(srcBuffer, 256);
        RtlZeroMemory(dstBuffer, 256);
        RtlInitEmptyUnicodeString(&srcPath, srcBuffer, sizeof(srcBuffer));
        RtlUnicodeStringPrintf(&srcPath, L"%ws\\%ws", FILE_DECOY_PATH, DecoyFileList[i]);
        RtlInitEmptyUnicodeString(&dstPath, dstBuffer, sizeof(dstBuffer));
        RtlUnicodeStringPrintf(&dstPath, L"%ws\\%ws", DstDecoyPath->Buffer, DecoyFileList[i]);

        NTSTATUS status;
        HANDLE hSrc, hDst;
        status = _OpenFile(&srcPath, &hSrc, GENERIC_READ, FILE_SHARE_READ, FILE_OPEN);
        if (!NT_SUCCESS(status)) {
            continue;
        }

        status = _OpenFile(&dstPath, &hDst, GENERIC_WRITE, 0, FILE_OVERWRITE_IF);
        if (!NT_SUCCESS(status)) {
            ZwClose(hSrc);
            continue;
        }

        status = _CopyFile(hSrc, hDst);
        ZwClose(hSrc);
        ZwClose(hDst);
    }
}