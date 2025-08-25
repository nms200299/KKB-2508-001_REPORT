#pragma once
#define REG_SETTING_PATH L"\\Registry\\Machine\\SYSTEM\\CurrentControlSet\\Services\\RswDetect\\Parameters"
#define FILE_DECOY_PATH L"\\??\\C:\\Program Files\\RswDetect\\decoy"
#define FILE_ALERT_PATH L"\\??\\C:\\Program Files\\RswDetect\\alert"

extern PFLT_FILTER gFilterHandle;
extern Setting gDriverSetting;