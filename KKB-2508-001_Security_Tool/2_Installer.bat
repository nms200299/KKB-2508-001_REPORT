@echo off
echo [RswDetect Installer]

net session >nul 2>&1
if %errorlevel% neq 0 (
    echo 관리자 권한으로 실행해야 합니다.
    pause
    exit /b
)

echo [INFO] C:\Program Files\RswDetect 폴더를 생성합니다.
if not exist "C:\Program Files\RswDetect" (
    mkdir "C:\Program Files\RswDetect"
)
echo.

echo [INFO] C:\Program Files\RswDetect 폴더에 파일을 복사합니다.
cd /d %~dp0
xcopy ".\bin" "C:\Program Files\RswDetect" /E /I /Y
echo.


echo [INFO] 커널 드라이버를 설치합니다.
sc create RswDetect type=kernel start= demand binPath="C:\Program Files\RswDetect\RswDetect.sys"
echo.

echo [INFO] RswDetect 옵션을 설정합니다.
cmd.exe /c "C:\Program Files\RswDetect\RswDetectSetting.bat"
echo.

echo [INFO] DebugView를 실행합니다.
start cmd.exe /k "C:\Program Files\RswDetect\DebugView\Dbgview.exe"
fltmc load RswDetect
echo.

pause