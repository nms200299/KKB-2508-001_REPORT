@echo off
echo [RswDetect UnInstaller]

net session >nul 2>&1
if %errorlevel% neq 0 (
    echo 관리자 권한으로 실행해야 합니다.
    pause
    exit /b
)

echo [INFO] 커널 드라이버를 종료합니다.
fltmc unload RswDetect
echo.

echo [INFO] C:\Program Files\RswDetect 폴더를 삭제합니다.
rmdir /S /Q "C:\Program Files\RswDetect"
echo.

echo [INFO] 레지스트리를 정리합니다.
reg delete "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\RswDetect\Parameters" /f 
echo.

pause