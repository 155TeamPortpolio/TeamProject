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

REM 실제 복사 
xcopy /y "%BASE%Engine\Bin\Engine.dll"      "%BASE%DemoProject\Bin\"
xcopy /y "%BASE%Engine\Bin\fmodL.dll"      "%BASE%DemoProject\Bin\"
xcopy /y "%BASE%Engine\Bin\fmod.dll"        "%BASE%DemoProject\Bin\"

REM ==========================================================
xcopy /y /s /e /i "%BASE%Engine\Public\"      "%BASE%EngineSDK\Inc\"

REM /// 엔진 셰이더(.cso) 파일을 클라이언트로 복사  ///
xcopy /y /s /e /i "%BASE%Engine\Bin\Engine_Shaders\"                "%BASE%DemoProject\Bin\ShaderFiles\"
xcopy /y /s /e /i "%BASE%Engine\Public\Engine_Shader\"          "%BASE%DemoProject\Bin\ShaderFiles\"

xcopy /y "%BASE%Engine\Bin\Engine.lib"                                                      "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmod_vc.lib"                             "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmodL_vc.lib"                           "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\assimp-vc143-mtd.lib"        "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\assimp-vc143-mt.lib"           "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\DirectXTKd.lib"                        "%BASE%EngineSDK\Lib\"
xcopy /y "%BASE%Engine\ThirdPartyLib\DirectXTK.lib"                           "%BASE%EngineSDK\Lib\"

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