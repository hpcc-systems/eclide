@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86

set ROOT_DIR=D:\hpccsystems
set SRC_DIR=%ROOT_DIR%\src
set BUILD_DIR=%ROOT_DIR%\build
set EXTERNALS_DIR=%ROOT_DIR%\externals2
echo %SRC_DIR%
echo %BUILD_DIR%
echo %EXTERNALS_DIR%

md %BUILD_DIR%\eclide
cd %BUILD_DIR%\eclide

cmake -G"Visual Studio 10" -DWITH_DYNAMIC_MSVC_RUNTIME=1 -DBOOST_INCLUDEDIR:PATH="%EXTERNALS_DIR%/Boost/include/boost-1_46_1" -DBOOST_LIBRARYDIR:PATH="%EXTERNALS_DIR%/Boost/lib" -DAGG_INCLUDE_DIR:PATH="%EXTERNALS_DIR%/agg/agg-2.4" -DWTL_INCLUDE_DIR:PATH="%EXTERNALS_DIR%/wtl81_11324" -DBUGTRAP_INCLUDE_DIR="%EXTERNALS_DIR%/BugTrapSrc/BugTrap/Win32/BugTrap" -DGRAPHLAYOUT_INCLUDE_DIR="%SRC_DIR%/GraphControl/graphlayout" -DGRAPHLAYOUT_LIBRARY_DIR="%BUILD_DIR%/GraphControl_3_10/projects/HPCCSystemsGraphViewControl/RelWithDebInfo/npHPCCSystemsGraphViewControl.lib" -DGRAPHVIZ_INCLUDE_DIR="%EXTERNALS_DIR%/graphviz-2.26.3" -DGSOAP_INCLUDE_DIR="%EXTERNALS_DIR%/gsoap" -DSCINTILLA_INCLUDE_DIR="%EXTERNALS_DIR%/scintilla303/scintilla" -DZLIB_INCLUDE_DIR="%EXTERNALS_DIR%/zlib-vc/zlib" "%SRC_DIR%/eclide"

rem set CONFIG_MODE=Debug
rem call :DOBUILD
rem set CONFIG_MODE=Release
rem call :DOBUILD
set CONFIG_MODE=RelWithDebInfo
call :DOBUILD
goto :EOF

:DOBUILD
msbuild /m /p:Configuration=%CONFIG_MODE% /nologo /noconsolelogger /fileLogger /flp:logfile=%BUILD_DIR%\EclIDE_%CONFIG_MODE%.txt /verbosity:quiet eclide.sln
if not errorlevel 0 goto :ERROR 
goto :EOF

cd ..\..

goto :EOF

:ERROR
echo Build "EclIDE_%CONFIG_MODE%" Failed!
pause
