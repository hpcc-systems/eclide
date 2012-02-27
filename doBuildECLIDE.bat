@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86

set ROOT_DIR=D:\hpccsystems
set SRC_DIR=%ROOT_DIR%\src
set BUILD_DIR=%SRC_DIR%\build
set EXTERNALS_DIR=%ROOT_DIR%\externals2
echo %SRC_DIR%
echo %BUILD_DIR%
echo %EXTERNALS_DIR%

md %BUILD_DIR%\eclide
cd %BUILD_DIR%\eclide

cmake -G"Visual Studio 10" -DWITH_DYNAMIC_MSVC_RUNTIME=1 -DBOOST_INCLUDEDIR:PATH="%EXTERNALS_DIR%/Boost/include/boost-1_46_1" -DBOOST_LIBRARYDIR:PATH="%EXTERNALS_DIR%/Boost/lib" -DAGG_INCLUDE_DIR:PATH="%EXTERNALS_DIR%/agg/agg-2.4" -DWTL_INCLUDE_DIR:PATH="%EXTERNALS_DIR%/wtl81_11324" -DBUGTRAP_INCLUDE_DIR="%EXTERNALS_DIR%/BugTrapSrc/BugTrap/Win32/BugTrap" -DGRAPHLAYOUT_INCLUDE_DIR="%SRC_DIR%/GraphControl/graphlayout" -DGRAPHLAYOUT_LIBRARY_DIR="%BUILD_DIR%/GraphControl/bin/RelWithDebInfo/graphlayout.lib" -DGRAPHVIZ_INCLUDE_DIR="%EXTERNALS_DIR%/graphviz-2.26.3" -DGSOAP_INCLUDE_DIR="%EXTERNALS_DIR%/gsoap" -DSCINTILLA_INCLUDE_DIR="%EXTERNALS_DIR%/scintilla303/scintilla" -DZLIB_INCLUDE_DIR="%EXTERNALS_DIR%/zlib-vc/zlib" "%SRC_DIR%/eclide"

msbuild /m /p:Configuration=Debug /verbosity:minimal eclide.sln
msbuild /m /p:Configuration=Release eclide.sln
msbuild /m /p:Configuration=RelWithDebInfo eclide.sln

cd ..\..
