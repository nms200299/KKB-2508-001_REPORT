/*++
Module Name:
    main.c

Abstract:
    KKB-2508-001 랜섬웨어 전용 보안 드라이버

Environment:
    Kernel mode
--*/

#include "pch.h"
#include "main.h"


PFLT_FILTER gFilterHandle;
Setting gDriverSetting;



NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    DbgPrint("[RswDetect] Driver Load\n");

    NTSTATUS status = STATUS_SUCCESS;
    DriverObject->DriverUnload = UnloadDriver;

    UNICODE_STRING sPsGetProcessImageFileName = RTL_CONSTANT_STRING(L"PsGetProcessImageFileName");
    PsGetProcessImageFileName = (GET_PROCESS_IMAGE_NAME)MmGetSystemRoutineAddress(&sPsGetProcessImageFileName);


    loadSetting(&gDriverSetting);
    DbgPrint("[RswDetect] EnableDecoy = %d\n", gDriverSetting.EnableDecoy);
    DbgPrint("[RswDetect] EnableSignature = %d\n", gDriverSetting.EnableSignature);
    DbgPrint("[RswDetect] DstDecoyPath = %wZ\n", &gDriverSetting.DstDecoyPath);

    if (gDriverSetting.EnableDecoy) {
        decoyInit(&gDriverSetting.DstDecoyPath);
    }

    status = FltRegisterFilter(DriverObject,
        &FilterRegistration,
        &gFilterHandle);

    if (NT_SUCCESS(status)) {
        status = FltStartFiltering(gFilterHandle);
        if (!NT_SUCCESS(status)) {
            FltUnregisterFilter(gFilterHandle);
        }
    }

    return status;
}

VOID UnloadDriver(IN PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
    FinalizeSetting();
    DbgPrint("[RswDetect] Driver UnLoad\n");
}
