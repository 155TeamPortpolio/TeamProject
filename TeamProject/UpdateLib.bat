@echo off
setlocal enabledelayedexpansion

REM ========== 프로젝트 폴더 등록 (공백으로 구분) ==========
set PROJECT_LIST=MapTool DemoProject AnimationTool CameraTool EffectTool UITool
REM ======================================================

set BASE=%~dp0
set CONFIG=%~1

echo [UpdateLib] Received parameter: [%CONFIG%]

set CONFIG=%CONFIG:"=%
set CONFIG=%CONFIG: =%

if "%CONFIG%"=="" (
    echo [UpdateLib] No parameter received, defaulting to Release
    set "CONFIG=Release"
)

if /I "%CONFIG%"=="Debug" goto :SetDebug
if /I "%CONFIG%"=="Release" goto :SetRelease

goto :SetError

:SetDebug
set "PHYSX_DIR=Engine\ThirdPartyLib\PhysX\deb"
echo [UpdateLib] Mode: DEBUG (Source: %PHYSX_DIR%)
goto :StartCopy

:SetRelease
set "PHYSX_DIR=Engine\ThirdPartyLib\PhysX\rel"
echo [UpdateLib] Mode: RELEASE (Source: %PHYSX_DIR%)
goto :StartCopy

:SetError
echo [UpdateLib] ERROR: Unknown configuration '%CONFIG%'
echo [UpdateLib] Valid values: Debug or Release
exit /b 1

:StartCopy
echo [UpdateLib] Waiting for Engine.dll...
call :WaitForFile "%BASE%Engine\Bin\Engine.dll" 15
call :WaitForFile "%BASE%Engine\Bin\Engine.lib" 15
call :WaitForFileUnlock "%BASE%Engine\Bin\Engine.dll" 10

call :WaitForFile "%BASE%Engine\ThirdPartyLib\fmod_vc.lib" 5
call :WaitForFile "%BASE%Engine\ThirdPartyLib\fmodL_vc.lib" 5
call :WaitForFile "%BASE%%PHYSX_DIR%\PhysX_64.lib" 5

echo [UpdateLib] Registered projects: %PROJECT_LIST%
echo [UpdateLib] Starting copy to project folders...

for %%P in (%PROJECT_LIST%) do (
    if exist "%BASE%%%P\Bin\" (
        echo [UpdateLib] Copying to %%P...
        call :CopyToProject "%BASE%%%P" "%%P"
    ) else (
        echo [UpdateLib] WARNING: %%P\Bin folder not found, skipping...
    )
)

echo [UpdateLib] Copying to EngineSDK...
if not exist "%BASE%EngineSDK\Lib\" mkdir "%BASE%EngineSDK\Lib\"
if not exist "%BASE%EngineSDK\Inc\" mkdir "%BASE%EngineSDK\Inc\"

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

xcopy /y /s /e /i "%BASE%Engine\Public\" "%BASE%EngineSDK\Inc\"

echo [UpdateLib] Copy completed successfully for %CONFIG% mode
goto :eof

:CopyToProject
set PROJECT_PATH=%~1
set PROJECT_NAME=%~2

xcopy /y "%BASE%Engine\Bin\Engine.dll" "%PROJECT_PATH%\Bin\" >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Failed to copy Engine.dll to %PROJECT_NAME%
    goto :eof
)

xcopy /y "%BASE%Engine\ThirdPartyLib\fmodL.dll" "%PROJECT_PATH%\Bin\" >nul 2>&1
xcopy /y "%BASE%Engine\ThirdPartyLib\fmod.dll" "%PROJECT_PATH%\Bin\" >nul 2>&1

xcopy /y "%BASE%%PHYSX_DIR%\PhysX_64.dll" "%PROJECT_PATH%\Bin\" >nul 2>&1
xcopy /y "%BASE%%PHYSX_DIR%\PhysXCommon_64.dll" "%PROJECT_PATH%\Bin\" >nul 2>&1
xcopy /y "%BASE%%PHYSX_DIR%\PhysXFoundation_64.dll" "%PROJECT_PATH%\Bin\" >nul 2>&1
xcopy /y "%BASE%%PHYSX_DIR%\PhysXCooking_64.dll" "%PROJECT_PATH%\Bin\" >nul 2>&1
xcopy /y "%BASE%%PHYSX_DIR%\PhysXGpu_64.dll" "%PROJECT_PATH%\Bin\" >nul 2>&1

xcopy /y /s /e /i "%BASE%Engine\Bin\Engine_Shaders\" "%PROJECT_PATH%\Bin\ShaderFiles\" >nul 2>&1
xcopy /y /s /e /i "%BASE%Engine\Public\Engine_Shader\" "%PROJECT_PATH%\Bin\ShaderFiles\" >nul 2>&1

echo [UpdateLib] %PROJECT_NAME% completed
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