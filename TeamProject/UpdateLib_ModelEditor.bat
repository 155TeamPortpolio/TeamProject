@echo off
setlocal

set BASE=%~dp0

REM 실제 복사 
xcopy /y "%BASE%Engine\Bin\Engine.dll"                                          "%BASE%ModelEditor\Bin\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmodL.dll"                         "%BASE%ModelEditor\Bin\"
xcopy /y "%BASE%Engine\ThirdPartyLib\fmod.dll"                          "%BASE%ModelEditor\Bin\"
xcopy /y "%BASE%Engine\ThirdPartyLib\assimp-vc143-mtd.dll"        "%BASE%ModelEditor\Bin\"
xcopy /y "%BASE%Engine\ThirdPartyLib\assimp-vc143-mt.dll"           "%BASE%ModelEditor\Bin\"
xcopy /y /s /e /i "%BASE%Engine\Bin\Engine_Shaders\"                "%BASE%ModelEditor\Bin\ShaderFiles\"
xcopy /y /s /e /i "%BASE%Engine\Public\Engine_Shader\"          "%BASE%ModelEditor\Bin\ShaderFiles\"

REM xcopy /y /s /e /i "%BASE%Engine\Public\"      "%BASE%EngineSDK\Inc\"

REM 여기까지==========================================================

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