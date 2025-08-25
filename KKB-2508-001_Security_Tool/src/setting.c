#include "pch.h"
#include "setting.h"

BOOLEAN ReadRegValue(HANDLE hKey, PCWSTR name, PUNICODE_STRING dstVar)
{
    UNICODE_STRING ustrName;
    ULONG len;
    PKEY_VALUE_PARTIAL_INFORMATION pInfo = NULL;
    NTSTATUS status;
    BOOLEAN result = FALSE;

    RtlInitUnicodeString(&ustrName, name);

    status = ZwQueryValueKey(hKey, &ustrName,
        KeyValuePartialInformation, NULL, 0, &len);

    if (status != STATUS_BUFFER_TOO_SMALL && status != STATUS_BUFFER_OVERFLOW) {
        return FALSE;
    }

    pInfo = (PKEY_VALUE_PARTIAL_INFORMATION)ExAllocatePoolWithTag(NonPagedPool, len, 'PINF');
    if (!pInfo) return FALSE;

    status = ZwQueryValueKey(hKey, &ustrName,
        KeyValuePartialInformation, pInfo, len, &len);

    if (!NT_SUCCESS(status)) {
        return FALSE;
    }
    
    switch (pInfo->Type) {
        case REG_DWORD: {
            if (*(PULONG)pInfo->Data != 0)
                result = TRUE;
           
            break;
        }
        case REG_SZ: {
            if (dstVar != NULL) {
                PWCHAR dataStr = (PWCHAR)ExAllocatePoolWithTag(NonPagedPool, pInfo->DataLength + sizeof(512), 'DATA');
                if (!dataStr) break;
                RtlZeroMemory(dataStr, pInfo->DataLength + sizeof(512));
                RtlCopyMemory(dataStr, (PWCHAR)pInfo->Data, pInfo->DataLength);
                dstVar->Buffer = dataStr;
                dstVar->Length = (USHORT)pInfo->DataLength;
                dstVar->MaximumLength = (USHORT)pInfo->DataLength + sizeof(512);
            }
            break;
        }
    }
    ExFreePoolWithTag(pInfo, 'PINF');
    return result;
}

VOID FinalizeSetting() {
    ExFreePoolWithTag(gDriverSetting.DstDecoyPath.Buffer, 'DATA');
}

NTSTATUS loadSetting() {
    HANDLE hKey;
    NTSTATUS status;
    UNICODE_STRING ustrRegPath;
    OBJECT_ATTRIBUTES objAttr;

    RtlInitUnicodeString(&ustrRegPath, REG_SETTING_PATH);

    InitializeObjectAttributes(&objAttr, &ustrRegPath,
        OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    status = ZwOpenKey(&hKey, KEY_READ, &objAttr);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    gDriverSetting.EnableDecoy = ReadRegValue(hKey, L"EnableDecoy", NULL);
    gDriverSetting.EnableSignature = ReadRegValue(hKey, L"EnableSignature", NULL);
    ReadRegValue(hKey, L"DstDecoyPath", &gDriverSetting.DstDecoyPath);

    ZwClose(hKey);
    return STATUS_SUCCESS;
}
