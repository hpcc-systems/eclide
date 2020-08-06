# ECL IDE

##  Building on windows

### Prerequisits 
* git (optional - needed to clone sources)
* cmake
* Visual Studio:
    * 2015
    * 2017 ver <= 15.8
* Python 2.7 (used to build chromium)
* NSIS (optional - needed to create install package)

### Clone the repository
```
git clone https://github.com/hpcc-systems/eclide.git eclide
cd eclide
git submodule update --init --recursive
```

### Fetch the third-party libraries (via vcpkg)
```
cd vcpkg
bootstrap-vcpkg.bat
vcpkg install --keep-going boost-crc boost-asio boost-date-time boost-filesystem boost-format boost-multi-index boost-program-options boost-ptr-container boost-random boost-serialization boost-signals boost-spirit boost-thread scintilla wtl gsoap bugtrap cef-project
```

### Create an out of source build folder
```
cd ..
cd ..
mkdir build
cd build
mkdir eclide
cd eclide
```

### Generate Visual Studio Solution
```
cmake ../../eclide -DCMAKE_TOOLCHAIN_FILE=../../eclide/vcpkg/scripts/buildsystems/vcpkg.cmake
...or...
cmake ../../eclide -G "Visual Studio 16 2019" -A Win32 -DCMAKE_TOOLCHAIN_FILE=../../eclide/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### Build
```
cmake --build . --config RelWithDebInfo -- -m
```

Supported Builds
```
cmake --build . --config Debug -- -m
cmake --build . --config RelWithDebInfo -- -m
cmake --build . --config Release -- -m
cmake --build . --config MinSizeRel -- -m
```

### Create Installer
```
cmake --build . --config RelWithDebInfo --target package -- -m
```
