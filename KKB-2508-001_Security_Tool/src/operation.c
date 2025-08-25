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
    } // IRQL 검사

    ULONG ProcessId = FltGetRequestorProcessId(Data);
    // 파일에 접근한 PID 구하기
    if ((ProcessId == 0) || (ProcessId == 4)) {
        return FLT_PREOP_SUCCESS_NO_CALLBACK;
    } // 시스템 프로세스에서 접근 시, 허용

    NTSTATUS status;
    PFLT_FILE_NAME_INFORMATION pFileInfo = NULL;
    UNICODE_STRING DstFilePath;
    PUNICODE_STRING pOrgFilePath = NULL;


    switch (Data->Iopb->MajorFunction) {
        case IRP_MJ_CREATE: {
            if (!(Data->Iopb->Parameters.Create.Options & FILE_DELETE_ON_CLOSE)) {
                return FLT_PREOP_SUCCESS_NO_CALLBACK;
            }
        } // IRP_MJ_CREATE는 FILE_DELETE_ON_CLOSE 옵션이 없으면 무조건 허용

        case IRP_MJ_SET_INFORMATION: {
            // 파일 복사, 이동의 경우, 목적지 경로가 덮어씌워질 수 있어 아래 로직에서 처리
            switch (Data->Iopb->Parameters.SetFileInformation.FileInformationClass) {
            case FileRenameInformation:
            case FileRenameInformationBypassAccessCheck: {
                PFILE_RENAME_INFORMATION pRenameInfo = (PFILE_RENAME_INFORMATION)Data->Iopb->Parameters.SetFileInformation.InfoBuffer;
                if ((pRenameInfo == NULL) || (pRenameInfo->FileNameLength == 0)) {
                    return FLT_PREOP_SUCCESS_NO_CALLBACK;
                } // 목적지 경로를 알 수 없는 경우, 해당 IRP를 허용

                DstFilePath.Length = (USHORT)pRenameInfo->FileNameLength;
                DstFilePath.MaximumLength = DstFilePath.Length;
                DstFilePath.Buffer = pRenameInfo->FileName;
                pOrgFilePath = &DstFilePath;
                // IRP의 목적지 경로를 비교 대상으로 함
                break;
            }
            }
        }

        default: {
            if (pOrgFilePath != NULL) break;
            // 위 RENAME 조건에 매칭되면 아래 로직을 수행하지 않음

            status = FltGetFileNameInformation(Data, FLT_FILE_NAME_OPENED, &pFileInfo);
            // IRP에 명시된 파일 경로를 알아냄

            if ((!NT_SUCCESS(status)) || (pFileInfo == NULL)) {
                return FLT_PREOP_SUCCESS_NO_CALLBACK;
            } // 파일 경로를 알 수 없으면 해당 IRP를 허용

            status = FltParseFileNameInformation(pFileInfo);
            if (!NT_SUCCESS(status)) {
                FltReleaseFileNameInformation(pFileInfo);
                return FLT_POSTOP_FINISHED_PROCESSING;
            }
            // 파일 경로를 파싱하여 구조체 멤버를 채움

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
        // 랜섬웨어가 디코이 파일에 접근할 시, 차단
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
        // 파일 시그니처가 훼손될 시, 차단
    } 

    if (pFileInfo != NULL) {
        FltReleaseFileNameInformation(pFileInfo);
        pFileInfo = NULL;
    } // FltGetFileNameInformation 함수로 할당받은 파일 경로가 있는 경우, 해제합니다. 

    if (RansomeResult) {
        PEPROCESS EProcess = PsGetCurrentProcess();
        PCHAR ProcessName = PsGetProcessImageFileName(EProcess);
        // 파일에 접근한 프로세스 이름 구하기
        
        DbgPrint("[RswDetect]\tProcess Terminated. [%s](%d)\n", ProcessName, ProcessId);
        _TerminateProcess((HANDLE)(ULONG_PTR)ProcessId);
        // 프로세스 종료

        Data->IoStatus.Status = STATUS_ACCESS_DENIED;
        Data->IoStatus.Information = 0;
        return FLT_PREOP_COMPLETE;
        // 조건에 매칭된다면 IRP를 거부합니다.
    }

    return FLT_PREOP_SUCCESS_NO_CALLBACK;
    // 위 조건에 해당하지 않으면 IRP를 허용합니다.
}