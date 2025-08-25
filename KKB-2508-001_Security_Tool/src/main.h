#pragma once

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
);

VOID UnloadDriver(IN PDRIVER_OBJECT DriverObject);