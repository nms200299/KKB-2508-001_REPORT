@echo off
@echo off
echo [RswDetect TestModeActive]

net session >nul 2>&1
if %errorlevel% neq 0 (
    echo ������ �������� �����ؾ� �մϴ�.
    pause
    exit /b
)

echo [INFO] �׽�Ʈ ��带 Ȱ��ȭ�մϴ�.
bcdedit -set loadoptions disable_intergrity_checks      
bcdedit -set testsigning on
echo.

echo [INFO] 10�� �Ŀ� PC�� ������մϴ�.
shutdown -r -t 10
echo.
pause