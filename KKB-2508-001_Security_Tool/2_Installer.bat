@echo off
echo [RswDetect Installer]

net session >nul 2>&1
if %errorlevel% neq 0 (
    echo ������ �������� �����ؾ� �մϴ�.
    pause
    exit /b
)

echo [INFO] C:\Program Files\RswDetect ������ �����մϴ�.
if not exist "C:\Program Files\RswDetect" (
    mkdir "C:\Program Files\RswDetect"
)
echo.

echo [INFO] C:\Program Files\RswDetect ������ ������ �����մϴ�.
cd /d %~dp0
xcopy ".\bin" "C:\Program Files\RswDetect" /E /I /Y
echo.


echo [INFO] Ŀ�� ����̹��� ��ġ�մϴ�.
sc create RswDetect type=kernel start= demand binPath="C:\Program Files\RswDetect\RswDetect.sys"
echo.

echo [INFO] RswDetect �ɼ��� �����մϴ�.
cmd.exe /c "C:\Program Files\RswDetect\RswDetectSetting.bat"
echo.

echo [INFO] DebugView�� �����մϴ�.
start cmd.exe /k "C:\Program Files\RswDetect\DebugView\Dbgview.exe"
fltmc load RswDetect
echo.

pause