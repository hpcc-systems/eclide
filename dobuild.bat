@echo off
cd build 
cmake ..
msbuild eclide.sln /m /p:Configuration=Release
cd ..