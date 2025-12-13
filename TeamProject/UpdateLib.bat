@echo off
setlocal

set BASE=%~dp0
set CONFIG=%~1

REM ===== 디버깅: 받은 파라미터 출력 =====
echo [UpdateLib] Received parameter: [%CONFIG%]

REM 공백/따옴표 제거 (안전장치)
set CONFIG=%CONFIG:"=%
set CONFIG=%CONFIG: =%

REM 파라미터가 없으면 기본값
if "%CONFIG%"=="" (
    echo [UpdateLib] No parameter received, defaulting to Release
    set "CONFIG=Release"
)

REM =======================================================
if /I "%CONFIG%"=="Debug" goto :SetDebug
if /I "%CONFIG%"=="Release" goto :SetRelease

REM 예외 처리
goto :SetError

REM -------------------------------------------------------
:SetDebug
set "PHYSX_DIR=Engine\ThirdPartyLib\PhysX\deb"
echo [UpdateLib] Mode: DEBUG (Source: %PHYSX_DIR%)
goto :StartCopy

REM -------------------------------------------------------
:SetRelease
set "PHYSX_DIR=Engine\ThirdPartyLib\PhysX\rel"
echo [UpdateLib] Mode: RELEASE (Source: %PHYSX_DIR%)
goto :StartCopy

REM -------------------------------------------------------
:SetError
echo [UpdateLib] ERROR: Unknown configuration '%CONFIG%'
echo [UpdateLib] Valid values: Debug or Release
exit /b 1

REM =======================================================
:StartCopy
REM 생성될 때까지 기다리는 함수
echo [UpdateLib] Waiting for Engine.dll...
call :WaitForFile "%BASE%Engine\Bin\Engine.dll" 15
call :WaitForFile "%BASE%Engine\Bin\Engine.lib" 15
call :WaitForFileUnlock "%BASE%Engine\Bin\Engine.dll" 10

REM FMOD 라이브러리 함수
call :WaitForFile "%BASE%Engine\ThirdPartyLib\fmod_vc.lib" 5
call :WaitForFile "%BASE%Engine\ThirdPartyLib\fmodL_vc.lib" 5

REM PhysX
call :WaitForFile "%BASE%%PHYSX_DIR%\PhysX_64.lib" 5


REM 여기서 부터 자신 프로젝트 기준으로 복사====================DemoProject부분을 자신 폴더명으로

REM 실제 복사 
echo [UpdateLib] Copying files...
xcopy /y "%BASE%Engine\Bin\Engine.dll" "%BASE%DemoProject\Bin\"
if errorlevel 1 (
    echo [ERROR] Failed to copy Engine.dll
    exit /b 1
)

xcopy /y "%BASE%Engine\ThirdPartyLib\fmodL.dll" "%BASE%DemoProject\Bin\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmod.dll" "%BASE%DemoProject\Bin\"

xcopy /y "%BASE%%PHYSX_DIR%\PhysX_64.dll" "%BASE%DemoProject\Bin\"
xcopy /y "%BASE%%PHYSX_DIR%\PhysXCommon_64.dll" "%BASE%DemoProject\Bin\"
xcopy /y "%BASE%%PHYSX_DIR%\PhysXFoundation_64.dll" "%BASE%DemoProject\Bin\"
xcopy /y "%BASE%%PHYSX_DIR%\PhysXCooking_64.dll" "%BASE%DemoProject\Bin\"
xcopy /y "%BASE%%PHYSX_DIR%\PhysXGpu_64.dll" "%BASE%DemoProject\Bin\"

xcopy /y /s /e /i "%BASE%Engine\Bin\Engine_Shaders\" "%BASE%DemoProject\Bin\ShaderFiles\"
xcopy /y /s /e /i "%BASE%Engine\Public\Engine_Shader\" "%BASE%DemoProject\Bin\ShaderFiles\"
xcopy /y /s /e /i "%BASE%Engine\Public\" "%BASE%EngineSDK\Inc\"

REM 여기까지==========================================================

xcopy /y "%BASE%Engine\Bin\Engine.lib" "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmod_vc.lib" "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmodL_vc.lib" "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\assimp-vc143-mtd.lib" "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\assimp-vc143-mt.lib" "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\DirectXTKd.lib" "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\DirectXTK.lib" "%BASE%EngineSDK\Lib\"

xcopy /y "%BASE%%PHYSX_DIR%\PhysX_64.lib" "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%%PHYSX_DIR%\PhysXCommon_64.lib" "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%%PHYSX_DIR%\PhysXFoundation_64.lib" "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%%PHYSX_DIR%\PhysXCooking_64.lib" "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%%PHYSX_DIR%\PhysXExtensions_static_64.lib" "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%%PHYSX_DIR%\PhysXPvdSDK_static_64.lib" "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%%PHYSX_DIR%\PhysXCharacterKinematic_static_64.lib" "%BASE%EngineSDK\Lib\"

if exist "%BASE%%PHYSX_DIR%\*.pdb" (
    xcopy /y "%BASE%%PHYSX_DIR%\*.pdb" "%BASE%EngineSDK\Lib\"
)

echo [UpdateLib] Copy completed successfully for %CONFIG% mode
goto :eof

:WaitForFile
set file=%~1
set retry=%~2
set count=0
:loop
if exist %file% (
    echo [UpdateLib] Found %file%
    goto :eof
)
set /a count+=1
if %count% GEQ %retry% (
    echo [ERROR] %file% not found after %retry% seconds.
    exit /b 1
)
echo [UpdateLib] Waiting for %file% ... (%count%/%retry%)
timeout /t 1 /nobreak > nul
goto loop

:WaitForFileUnlock
set file=%~1
set retry=%~2
set count=0
:unlock_loop
REM 파일을 복사해서 쓰기 가능한지 테스트
copy /y "%file%" "%file%.test" >nul 2>&1
if %errorlevel% equ 0 (
    del "%file%.test" >nul 2>&1
    echo [UpdateLib] %file% is ready (unlocked)
    goto :eof
)
set /a count+=1
if %count% GEQ %retry% (
    echo [WARNING] %file% still locked after %retry% seconds, proceeding anyway...
    goto :eof
)
echo [UpdateLib] Waiting for %file% to be unlocked... (%count%/%retry%)
timeout /t 1 /nobreak > nul
goto unlock_loop