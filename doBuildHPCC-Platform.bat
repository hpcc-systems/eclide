@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86

set ROOT_DIR=D:\hpccsystems
set SRC_DIR=%ROOT_DIR%\src
set BUILD_DIR=%ROOT_DIR%\build
set EXTERNALS_DIR=%ROOT_DIR%\externals
echo %SRC_DIR%
echo %BUILD_DIR%
echo %EXTERNALS_DIR%

md %BUILD_DIR%\HPCC-Platform
cd %BUILD_DIR%\HPCC-Platform

cmake -G"Visual Studio 9 2008" -DEXTERNALS_DIRECTORY="%EXTERNALS_DIR%" "%SRC_DIR%/HPCC-Platform"

pause

call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
msbuild /p:Configuration=Release hpccsystems-platform.sln

cd ..\..

pause
