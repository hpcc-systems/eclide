@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86

set ROOT_DIR=D:\hpccsystems
set SRC_DIR=%ROOT_DIR%\src
set BUILD_DIR=%SRC_DIR%\build
set EXTERNALS_DIR=%ROOT_DIR%\externals
echo %SRC_DIR%
echo %BUILD_DIR%
echo %EXTERNALS_DIR%

md %BUILD_DIR%\LN
cd %BUILD_DIR%\LN

cmake -G"Visual Studio 9 2008" -DEXTERNALS_DIRECTORY="%EXTERNALS_DIR%" "%SRC_DIR%/LN"

pause

call "C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\vcvarsall.bat" x86
msbuild /p:Configuration=Release lexisnexisrs-platform.sln

cd ..\..

pause
