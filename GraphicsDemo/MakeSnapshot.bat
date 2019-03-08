set PLATFORM_TARGET=%1
set CONFIGURATION=Release
set VERSION=0.0.2
rem https://docs.microsoft.com/en-us/windows-server/administration/windows-commands/call#batch-parameters
set SNAPSHOT_LOCATION="..\Snapshot\%VERSION%\"
set SHADERS=*.glsl
set RESOURCES=.\Resources
set FONTS=.\Fonts
set BUILD_LOCATION="..\%PLATFORM_TARGET%\%CONFIGURATION%\"
 
echo %SHADERS%
rem make directory for snapshot
mkdir %SNAPSHOT_LOCATION%
rem copy shaders
xcopy /y %SHADERS% %SNAPSHOT_LOCATION%
xcopy /y /s %RESOURCES% %SNAPSHOT_LOCATION%\Resources\
xcopy /y %BUILD_LOCATION%* %SNAPSHOT_LOCATION%
xcopy /y /s %FONTS% %SNAPSHOT_LOCATION%\Fonts\