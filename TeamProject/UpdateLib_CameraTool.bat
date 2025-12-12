@echo off
setlocal

set BASE=%~dp0

REM 생성될 때까지 기다리는 함수
call :WaitForFile "%BASE%Engine\Bin\Engine.dll" 5
call :WaitForFile "%BASE%Engine\Bin\Engine.lib" 5
call :WaitForFile "%BASE%Engine\Bin\fmodL.dll" 1
call :WaitForFile "%BASE%Engine\Bin\fmod.dll" 1

REM FMOD 라이브러리 함수
call :WaitForFile "%BASE%Engine\ThirdPartyLib\fmod_vc.lib" 1
call :WaitForFile "%BASE%Engine\ThirdPartyLib\fmodL_vc.lib" 1

REM PhysX
call :WaitForFile "%BASE%Engine\Bin\PhysX_64.lib" 1


REM 여기서 부터 자신 프로젝트 기준으로 복사====================DemoProject부분을 자신 폴더명으로

REM 실제 복사 
xcopy /y "%BASE%Engine\Bin\Engine.dll"                                          "%BASE%CameraTool\Bin\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmodL.dll"                         "%BASE%CameraTool\Bin\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmod.dll"                          "%BASE%CameraTool\Bin\"

xcopy /y "%BASE%Engine\Bin\PhysX_64.dll"           "%BASE%CameraTool\Bin\"
xcopy /y "%BASE%Engine\Bin\PhysXCommon_64.dll"     "%BASE%CameraTool\Bin\"
xcopy /y "%BASE%Engine\Bin\PhysXFoundation_64.dll" "%BASE%CameraTool\Bin\"
xcopy /y "%BASE%Engine\Bin\PhysXCooking_64.dll" "%BASE%CameraTool\Bin\"

xcopy /y /s /e /i "%BASE%Engine\Bin\Engine_Shaders\"                "%BASE%CameraTool\Bin\ShaderFiles\"
xcopy /y /s /e /i "%BASE%Engine\Public\Engine_Shader\"          "%BASE%CameraTool\Bin\ShaderFiles\"

xcopy /y /s /e /i "%BASE%Engine\Public\"      "%BASE%EngineSDK\Inc\"

REM 여기까지==========================================================

xcopy /y "%BASE%Engine\Bin\Engine.lib"                                                      "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmod_vc.lib"                             "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmodL_vc.lib"                           "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\assimp-vc143-mtd.lib"        "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\assimp-vc143-mt.lib"           "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\DirectXTKd.lib"                        "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\DirectXTK.lib"                           "%BASE%EngineSDK\Lib\"

xcopy /y "%BASE%Engine\ThirdPartyLib\PhysX_64.lib"                                                "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\PhysXCommon_64.lib"                              "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\PhysXFoundation_64.lib"                         "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\PhysXExtensions_static_64.lib"            "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\PhysXPvdSDK_static_64.lib"                           "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\PhysXCharacterKinematic_static_64.lib"   "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\PhysXCooking_64.lib"                                         "%BASE%EngineSDK\Lib\"

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