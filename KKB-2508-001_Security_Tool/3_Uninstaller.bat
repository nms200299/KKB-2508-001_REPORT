@echo off
echo [RswDetect UnInstaller]

net session >nul 2>&1
if %errorlevel% neq 0 (
    echo ������ �������� �����ؾ� �մϴ�.
    pause
    exit /b
)

echo [INFO] Ŀ�� ����̹��� �����մϴ�.
fltmc unload RswDetect
echo.

echo [INFO] C:\Program Files\RswDetect ������ �����մϴ�.
rmdir /S /Q "C:\Program Files\RswDetect"
echo.

echo [INFO] ������Ʈ���� �����մϴ�.
reg delete "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\RswDetect\Parameters" /f 
echo.

pause