call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\VC\vcvarsall.bat" x86
call doPreBuildSetup.bat
msbuild /m SetupHPCC.sln /p:Configuration=Release
call doPostBuildSetup.bat