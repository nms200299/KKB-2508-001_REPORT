#pragma once

typedef struct _Setting {
	BOOLEAN EnableDecoy;
	BOOLEAN EnableSignature;
	UNICODE_STRING DstDecoyPath;
} Setting;

NTSTATUS loadSetting();
VOID FinalizeSetting();