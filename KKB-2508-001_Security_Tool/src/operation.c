#include "pch.h"
#include "operation.h"

FLT_PREOP_CALLBACK_STATUS
PreOperation(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Flt_CompletionContext_Outptr_ PVOID* CompletionContext
)
{
    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(CompletionContext);

    if (KeGetCurrentIrql() > PASSIVE_LEVEL) {
        return FLT_PREOP_SUCCESS_NO_CALLBACK;
    } // IRQL �˻�

    ULONG ProcessId = FltGetRequestorProcessId(Data);
    // ���Ͽ� ������ PID ���ϱ�
    if ((ProcessId == 0) || (ProcessId == 4)) {
        return FLT_PREOP_SUCCESS_NO_CALLBACK;
    } // �ý��� ���μ������� ���� ��, ���

    NTSTATUS status;
    PFLT_FILE_NAME_INFORMATION pFileInfo = NULL;
    UNICODE_STRING DstFilePath;
    PUNICODE_STRING pOrgFilePath = NULL;


    switch (Data->Iopb->MajorFunction) {
        case IRP_MJ_CREATE: {
            if (!(Data->Iopb->Parameters.Create.Options & FILE_DELETE_ON_CLOSE)) {
                return FLT_PREOP_SUCCESS_NO_CALLBACK;
            }
        } // IRP_MJ_CREATE�� FILE_DELETE_ON_CLOSE �ɼ��� ������ ������ ���

        case IRP_MJ_SET_INFORMATION: {
            // ���� ����, �̵��� ���, ������ ��ΰ� ������� �� �־� �Ʒ� �������� ó��
            switch (Data->Iopb->Parameters.SetFileInformation.FileInformationClass) {
            case FileRenameInformation:
            case FileRenameInformationBypassAccessCheck: {
                PFILE_RENAME_INFORMATION pRenameInfo = (PFILE_RENAME_INFORMATION)Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
                if ((pRenameInfo == NULL) || (pRenameInfo->FileNameLength == 0)) {
                    return FLT_PREOP_SUCCESS_NO_CALLBACK;
                } // ������ ��θ� �� �� ���� ���, �ش� IRP�� ���

                DstFilePath.Length = (USHORT)pRenameInfo->FileNameLength;
                DstFilePath.MaximumLength = DstFilePath.Length;
                DstFilePath.Buffer = pRenameInfo->FileName;
                pOrgFilePath = &DstFilePath;
                // IRP�� ������ ��θ� �� ������� ��
                break;
            }
            }
        }

        default: {
            if (pOrgFilePath != NULL) break;
            // �� RENAME ���ǿ� ��Ī�Ǹ� �Ʒ� ������ �������� ����

            status = FltGetFileNameInformation(Data, FLT_FILE_NAME_OPENED, &pFileInfo);
            // IRP�� ��õ� ���� ��θ� �˾Ƴ�

            if ((!NT_SUCCESS(status)) || (pFileInfo == NULL)) {
                return FLT_PREOP_SUCCESS_NO_CALLBACK;
            } // ���� ��θ� �� �� ������ �ش� IRP�� ���

            status = FltParseFileNameInformation(pFileInfo);
            if (!NT_SUCCESS(status)) {
                FltReleaseFileNameInformation(pFileInfo);
                return FLT_POSTOP_FINISHED_PROCESSING;
            }
            // ���� ��θ� �Ľ��Ͽ� ����ü ����� ä��

            pOrgFilePath = &pFileInfo->Name;
            break;
        }
    }

    int RansomeResult = 0;

    if (gDriverSetting.EnableDecoy) {
        UNICODE_STRING TargetPath;
        WCHAR buffer[256];
        RtlInitEmptyUnicodeString(&TargetPath, buffer, sizeof(buffer));
        RtlUnicodeStringPrintf(&TargetPath, L"*%ws\\%ws", gDriverSetting.DstDecoyPath.Buffer + 6, L"TEST.*");
        _UpperPWCH(TargetPath.Buffer);
        if (FsRtlIsNameInExpression(&TargetPath, pOrgFilePath, TRUE, NULL)) {
            DbgPrint("[RswDetect] Accessed the decoy file.\n");
            RansomeResult = 1;
        }
        // ������� ������ ���Ͽ� ������ ��, ����
    }


    
    if ((gDriverSetting.EnableSignature) && (Data->Iopb->MajorFunction == IRP_MJ_WRITE)){
        UNICODE_STRING OrgExt;
        _GetFileExt(pOrgFilePath->Buffer, pOrgFilePath->Length / sizeof(WCHAR), &OrgExt);

        if ((OrgExt.Buffer != NULL) &&
            (FlagOn(Data->Flags, FLTFL_CALLBACK_DATA_SYSTEM_BUFFER)) ||
            (FlagOn(Data->Flags, FLTFL_CALLBACK_DATA_IRP_OPERATION))) {
            ULONG Length = Data->Iopb->Parameters.Write.Length;
            LONGLONG ByteOffset = Data->Iopb->Parameters.Write.ByteOffset.QuadPart;
            PVOID Buffer = Data->Iopb->Parameters.Write.WriteBuffer;

            if (CmpFileSignatures(&OrgExt, Length, ByteOffset, Buffer)) {
                DbgPrint("[RswDetect] File Signature Corrupted.\n");
                RansomeResult = 2;
            }
        }
        // ���� �ñ״�ó�� �Ѽյ� ��, ����
    } 

    if (pFileInfo != NULL) {
        FltReleaseFileNameInformation(pFileInfo);
        pFileInfo = NULL;
    } // FltGetFileNameInformation �Լ��� �Ҵ���� ���� ��ΰ� �ִ� ���, �����մϴ�. 

    if (RansomeResult) {
        PEPROCESS EProcess = PsGetCurrentProcess();
        PCHAR ProcessName = PsGetProcessImageFileName(EProcess);
        // ���Ͽ� ������ ���μ��� �̸� ���ϱ�
        
        DbgPrint("[RswDetect]\tProcess Terminated. [%s](%d)\n", ProcessName, ProcessId);
        _TerminateProcess((HANDLE)(ULONG_PTR)ProcessId);
        // ���μ��� ����

        Data->IoStatus.Status = STATUS_ACCESS_DENIED;
        Data->IoStatus.Information = 0;
        return FLT_PREOP_COMPLETE;
        // ���ǿ� ��Ī�ȴٸ� IRP�� �ź��մϴ�.
    }

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
    // �� ���ǿ� �ش����� ������ IRP�� ����մϴ�.
}