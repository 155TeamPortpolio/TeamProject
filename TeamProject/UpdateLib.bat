@echo off
setlocal

set BASE=%~dp0
set CONFIG=%~1
if "%CONFIG%" == "" set CONFIG=Debug
echo [UpdateLib] Current Configuration Mode: %CONFIG%

REM 경로 설정
set PHYSX_SRC_DIR=%BASE%Engine\ThirdPartyLib\PhysX\%CONFIG%

REM 생성될 때까지 기다리는 함수
call :WaitForFile "%BASE%Engine\Bin\Engine.dll" 5
call :WaitForFile "%BASE%Engine\Bin\Engine.lib" 5
call :WaitForFile "%BASE%Engine\Bin\fmodL.dll" 1
call :WaitForFile "%BASE%Engine\Bin\fmod.dll" 1

REM FMOD 라이브러리 함수
call :WaitForFile "%BASE%Engine\ThirdPartyLib\fmod_vc.lib" 1
call :WaitForFile "%BASE%Engine\ThirdPartyLib\fmodL_vc.lib" 1

REM PhysX
call :WaitForFile "%PHYSX_SRC_DIR%\PhysX_64.lib" 1


REM 여기서 부터 자신 프로젝트 기준으로 복사====================DemoProject부분을 자신 폴더명으로

REM 실제 복사 
xcopy /y "%BASE%Engine\Bin\Engine.dll"                                          "%BASE%DemoProject\Bin\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmodL.dll"                         "%BASE%DemoProject\Bin\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmod.dll"                          "%BASE%DemoProject\Bin\"

xcopy /y "%PHYSX_SRC_DIR%\PhysX_64.dll"           "%BASE%DemoProject\Bin\"
xcopy /y "%PHYSX_SRC_DIR%\PhysXCommon_64.dll"     "%BASE%DemoProject\Bin\"
xcopy /y "%PHYSX_SRC_DIR%\PhysXFoundation_64.dll" "%BASE%DemoProject\Bin\"
xcopy /y "%PHYSX_SRC_DIR%\PhysXCooking_64.dll"    "%BASE%DemoProject\Bin\"

xcopy /y /s /e /i "%BASE%Engine\Bin\Engine_Shaders\"                "%BASE%DemoProject\Bin\ShaderFiles\"
xcopy /y /s /e /i "%BASE%Engine\Public\Engine_Shader\"          "%BASE%DemoProject\Bin\ShaderFiles\"

xcopy /y /s /e /i "%BASE%Engine\Public\"      "%BASE%EngineSDK\Inc\"

REM 여기까지==========================================================

xcopy /y "%BASE%Engine\Bin\Engine.lib"                                                      "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmod_vc.lib"                             "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmodL_vc.lib"                           "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\assimp-vc143-mtd.lib"        "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\assimp-vc143-mt.lib"           "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\DirectXTKd.lib"                        "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\DirectXTK.lib"                           "%BASE%EngineSDK\Lib\"

xcopy /y "%PHYSX_SRC_DIR%\PhysX_64.lib"                   "%BASE%EngineSDK\Lib\"
xcopy /y "%PHYSX_SRC_DIR%\PhysXCommon_64.lib"             "%BASE%EngineSDK\Lib\"
xcopy /y "%PHYSX_SRC_DIR%\PhysXFoundation_64.lib"         "%BASE%EngineSDK\Lib\"
xcopy /y "%PHYSX_SRC_DIR%\PhysXExtensions_static_64.lib"   "%BASE%EngineSDK\Lib\"
xcopy /y "%PHYSX_SRC_DIR%\PhysXPvdSDK_static_64.lib"       "%BASE%EngineSDK\Lib\"
xcopy /y "%PHYSX_SRC_DIR%\PhysXCharacterKinematic_static_64.lib" "%BASE%EngineSDK\Lib\"
xcopy /y "%PHYSX_SRC_DIR%\PhysXCooking_64.lib"             "%BASE%EngineSDK\Lib\"

goto :eof

:WaitForFile
set file=%~1
set retry=%~2
set count=0

:loop
if exist %file% (
    echo Found %file%
    goto :eof
)

set /a count+=1
if %count% GEQ %retry% (
    echo [ERROR] %file% not found after %retry% seconds.
    exit /b 1
)

echo Waiting for %file% ...
timeout /t 1 > nul
goto loop