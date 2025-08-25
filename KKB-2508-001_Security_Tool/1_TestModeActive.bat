@echo off
@echo off
echo [RswDetect TestModeActive]

net session >nul 2>&1
if %errorlevel% neq 0 (
    echo 관리자 권한으로 실행해야 합니다.
    pause
    exit /b
)

echo [INFO] 테스트 모드를 활성화합니다.
bcdedit -set loadoptions disable_intergrity_checks      
bcdedit -set testsigning on
echo.

echo [INFO] 10초 후에 PC를 재부팅합니다.
shutdown -r -t 10
echo.
pause