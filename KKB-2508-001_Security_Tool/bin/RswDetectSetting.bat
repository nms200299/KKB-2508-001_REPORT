@echo off
echo [RswDetectSetting]

sc stop RswDetect >nul

set DECOY_PATH=\??\%USERPROFILE%\Desktop\$DECOY
mkdir %DECOY_PATH% >nul
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\RswDetect\Parameters" /v DstDecoyPath /t REG_SZ /d %DECOY_PATH% /f >nul

set /p answer=Enable Decoy Base Defence (Y/N): 
if /i "%answer%"=="Y" (
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\RswDetect\Parameters" /v EnableDecoy /t REG_DWORD /d 1 /f
) else (
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\RswDetect\Parameters" /v EnableDecoy /t REG_DWORD /d 0 /f
)
set /p answer=Enable Signature Base Defence (Y/N): 
if /i "%answer%"=="Y" (
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\RswDetect\Parameters" /v EnableSignature /t REG_DWORD /d 1 /f
) else (
reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\RswDetect\Parameters" /v EnableSignature /t REG_DWORD /d 0 /f
)


sc start RswDetect >nul

echo.
pause