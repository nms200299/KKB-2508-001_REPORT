#include "pch.h"
#include "registration.h"

PFLT_FILTER gFilterHandle;

FLT_OPERATION_REGISTRATION OperationCallback[] = {
    {IRP_MJ_CREATE,
        0, PreOperation, NULL},
    {IRP_MJ_WRITE,
        0, PreOperation, NULL},
    {IRP_MJ_SET_INFORMATION,
        0, PreOperation, NULL},
    { IRP_MJ_OPERATION_END }
};

NTSTATUS FilterUnloadCallback(
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
)
{
    UNREFERENCED_PARAMETER(Flags);
    FltUnregisterFilter(gFilterHandle);
    gFilterHandle = NULL;
    return STATUS_SUCCESS;
}


FLT_REGISTRATION FilterRegistration = {
    sizeof(FLT_REGISTRATION),                       //  Size
    FLT_REGISTRATION_VERSION,                       //  Version
    0,                                              //  Flags
    NULL,                                           //  Context
    OperationCallback,                              //  Operation callbacks
    FilterUnloadCallback,                           //  Filters unload routine
    NULL, NULL, NULL, NULL, NULL, NULL, NULL        //  Unused routines
};